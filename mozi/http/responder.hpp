#pragma once

#include "mozi/http/llhttp/llhttp.h"
#include "mozi/http/types.hpp"
#include "mozi/nid/worker.hpp"
#include "mozi/utils/traits.hpp"
#include "mozi/variables/const_dec.hpp"
#include "mozi/variables/string_view.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <libcuckoo/cuckoohash_map.hh>
#include <memory>
#include <r3.h>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <uv.h>
#include <uv/unix.h>

namespace mozi::http
{

struct mo_loop_data_s
{
    mo_route_t *rageroute;
    mo_responder_map_t *responders;
    nid::mo_nid_worker_t *nid_worker;
};

class mo_responder_c : public std::enable_shared_from_this<mo_responder_c>
{
  private:
    template <typename T> using box = std::unique_ptr<T>;
    struct mo__message_s
    {
        std::string body = "";
        const uv_buf_t *buf = nullptr;
    };
    struct mo__info_request_s : mo__message_s
    {
        mo_responder_c *self = nullptr;
        std::string url = "";
    };
    struct mo__info_response_s : mo__message_s
    {
        mo_responder_c *self = nullptr;
    };
    struct mo__info_s
    {
        explicit mo__info_s(box<uv_tcp_t> client, llhttp_t req_parser, llhttp_settings_t req_settings,
                            int64_t id) noexcept
            : client(std::move(client)), request_parser(req_parser), request_settings(req_settings), id(id)
        {
        }
        box<uv_tcp_t> client;
        llhttp_t request_parser;
        llhttp_settings_t request_settings;
        int64_t id;
    };

  public:
    explicit mo_responder_c(mo__info_s info) : m_info(std::move(info))
    {
    }
    static std::shared_ptr<mo_responder_c> create(box<uv_tcp_t> client) noexcept
    {
        int64_t id = static_cast<mo_loop_data_s *>(client->loop->data)->nid_worker->next_id();
        auto responder = std::make_shared<mo_responder_c>(mo__info_s{std::move(client), {}, {}, id});
        auto info = &responder->m_info;
        auto request_settings = &info->request_settings;
        auto request_parser = &info->request_parser;

        llhttp_settings_init(request_settings);
        request_settings->on_url = on_url;
        request_settings->on_body = on_body;
        request_settings->on_method_complete = on_method_complete;
        request_settings->on_url_complete = on_url_complete;
        request_settings->on_message_complete = on_message_complete;
        request_settings->on_reset = on_reset;
        llhttp_init(request_parser, HTTP_REQUEST, request_settings);

        mo_responder_c *self_ptr_raw{responder.get()};

        mo__info_request_s *request_parser_data{new mo__info_request_s};
        request_parser_data->self = self_ptr_raw;

        request_parser->data = request_parser_data;
        info->client->data = self_ptr_raw;

        static_cast<mo_loop_data_t *>(client->loop->data)->responders->insert(info->id, responder);

        return responder;
    }
    inline void add_message(uv_buf_t *buf) noexcept
    {
        buf->len = MAX_REQUEST_SIZE;
        buf->base = (char *)malloc(MAX_REQUEST_SIZE); // NOLINT
    }
    inline void read_message(ssize_t nread, const uv_buf_t *buf) noexcept
    {
        if (nread > 0)
        {
            auto data = parser_data(&m_info.request_parser);
            data->buf = buf;

            auto http_errno = llhttp_execute(&m_info.request_parser, buf->base, nread);
            if (http_errno != HPE_OK)
            {
                spdlog::error("RequestParse error {}", llhttp_errno_name(http_errno));
                free(buf->base); // NOLINT
                llhttp_reset(&m_info.request_parser);
                /**
                  TODO: 是否要回收responder 这里调用llhttp_reset后 以下操作会被重置丢失

                  mo_responder_c *self_ptr_raw{responder.get()};

                  mo__info_request_s *request_parser_data{new mo__info_request_s};
                  request_parser_data->self = self_ptr_raw;

                  request_parser->data = request_parser_data;
                */
            };
        }
        else if (nread < 0)
        {
            read_message_error(m_info.client.get(), static_cast<int32_t>(nread));
        }
    }
    static void reset(uv_loop_t *loop, int64_t id) noexcept
    {
        static_cast<mo_loop_data_t *>(loop->data)->responders->erase(id);
    }
    std::shared_ptr<mo_responder_c> self() noexcept
    {
        return shared_from_this();
    }
    inline uv_tcp_t *client() const noexcept
    {
        return m_info.client.get();
    }
    inline int64_t id() const noexcept
    {
        return m_info.id;
    }

  private:
    inline static int32_t on_url(llhttp_t *parser, const char *at, size_t length) noexcept
    {
        auto data = parser_data(parser);
        data->url = std::string(at, length);
        spdlog::info("url: {}", data->url);
        return llhttp_errno::HPE_OK;
    }
    inline static int32_t on_body(llhttp_t *parser, const char *at, size_t length) noexcept
    {
        auto data = parser_data(parser);
        data->body = std::string(at, length);
        spdlog::info("body: {}", data->body);
        return llhttp_errno::HPE_OK;
    }
    inline static int32_t on_method_complete(llhttp_t *parser) noexcept
    {
        auto method = static_cast<llhttp_method_t>(llhttp_get_method(parser));
        auto self = static_cast<mo_responder_c *>(parser->data);
        if (method != llhttp_method::HTTP_GET && method != llhttp_method::HTTP_POST)
        {
            auto client = static_cast<uv_stream_t *>(static_cast<void *>(self->client()));
            http_501_not_implemented(client);
            return llhttp_errno::HPE_INVALID_METHOD;
        }
        return llhttp_errno::HPE_OK;
    }
    inline static int32_t on_url_complete(llhttp_t *parser) noexcept
    {
        auto data = parser_data(parser);
        std::string_view url = data->url;
        auto client = data->self->client();
        mo_route_t *route = nullptr;
        match_entry *entry = match_entry_createl(url.data(), static_cast<int32_t>(url.size()));
        route = r3_tree_match_entry(static_cast<mo_loop_data_t *>(client->loop->data)->rageroute, entry);
        if (route == nullptr)
        {
            http_404_not_found(static_cast<uv_stream_t *>(static_cast<void *>(client)));
            match_entry_free(entry);
            return llhttp_errno::HPE_INVALID_URL;
        };
        match_entry_free(entry);
        return llhttp_errno::HPE_OK;
    }
    inline static int32_t on_message_complete(llhttp_t *parser) noexcept
    {
        auto data = parser_data(parser);
        std::string_view url = data->url;
        auto method = static_cast<llhttp_method_t>(llhttp_get_method(parser));
        auto client = data->self->client();
        match_entry *entry = match_entry_createl(url.data(), static_cast<int32_t>(url.size()));
        if (method == llhttp_method::HTTP_GET)
        {
            entry->request_method = METHOD_GET;
        }
        else if (method == llhttp_method::HTTP_POST)
        {
            entry->request_method = METHOD_POST;
        };
        R3Route *matched_route =
            r3_tree_match_route(static_cast<mo_loop_data_t *>(client->loop->data)->rageroute, entry);
        if (matched_route)
        {
            auto func = static_cast<mozi::mo_function_t<void, mo_responder_c *> *>(matched_route->data);
            (*func)(data->self);
        }
        else
        {
            http_404_not_found(static_cast<uv_stream_t *>(static_cast<void *>(client)));
        };
        match_entry_free(entry);
        return llhttp_errno::HPE_OK;
    }
    inline static int32_t on_reset(llhttp_t *parser) noexcept
    {
        auto data = parser_data(parser);
        if (data->buf)
        {
            free(data->buf->base); // NOLINT
        }
        return llhttp_errno::HPE_OK;
    }
    inline static mo__info_request_s *parser_data(llhttp_t *parser)
    {
        return static_cast<mo__info_request_s *>(parser->data);
    }
    inline static void http_404_not_found(uv_stream_t *client) noexcept
    {
        spdlog::error("404 not found");
        // char *info = new char[HTTP_404_NOT_FOUND.size() + 1];
        // memcpy(info, HTTP_404_NOT_FOUND.data(), HTTP_404_NOT_FOUND.size());
        // info[HTTP_404_NOT_FOUND.size()] = '\0';
        uv_write_t *writer = (uv_write_t *)malloc(sizeof(uv_write_t)); // NOLINT
        std::string_view info = HTTP_404_NOT_FOUND;
        uv_buf_t response_buf = uv_buf_init(const_cast<char *>(info.data()), HTTP_404_NOT_FOUND.size()); // NOLINT
        uv_write(writer, client, &response_buf, 1, [](uv_write_t *write, [[MO_UNUSED]] int status) {
            free(write); // NOLINT
        });
    }
    inline static void http_501_not_implemented(uv_stream_t *client) noexcept
    {
        spdlog::error("501 not implemented");
        uv_write_t *writer = (uv_write_t *)malloc(sizeof(uv_write_t)); // NOLINT
        std::string_view info = HTTP_501_NOT_IMPLEMENTED;
        // Note that this cannot be set to uv_buf_init(info, HTTP_501_NOT_IMPLEMENTED.size() + 1);
        uv_buf_t write_buf = uv_buf_init(const_cast<char *>(info.data()), HTTP_501_NOT_IMPLEMENTED.size()); // NOLINT
        uv_write(writer, client, &write_buf, 1, [](uv_write_t *write, [[MO_UNUSED]] int status) {
            free(write); // NOLINT
        });
    }
    inline static void read_message_error(uv_tcp_t *client, int32_t code) noexcept
    {
        switch (code)
        {
        case UV_EOF:
            // spdlog::info("EOF");
            break;
        default:
            spdlog::info("read fail {}", uv_err_name(code));
            break;
        }
        uv_close(static_cast<uv_handle_t *>(static_cast<void *>(client)), [](uv_handle_t *client) {
            auto self = static_cast<mo_responder_c *>(client->data);
            self->reset(client->loop, self->id());
        });
    }

  private:
    mo__info_s m_info;
};
} // namespace mozi::http
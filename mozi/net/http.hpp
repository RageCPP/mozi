#pragma once

#include "mozi/net/llhttp/llhttp.h"
#include "mozi/net/responder.hpp"
#include "mozi/utils/traits.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <libcuckoo/cuckoohash_map.hh>
#include <memory>
#include <r3.h>
#include <spdlog/spdlog.h>
#include <uv.h>
#include <uv/unix.h>
/**
 * 备注:
 * Currently only GET and POST are allowed, other methods will return 501.
 *
 */
// std::string_view hello{"/hello"};
// auto fib(int n) -> int
// {
//     std::cout << n << std::endl;

//     if (n < 2)
//     {
//         return n;
//     }

//     int a = fib(n - 1);
//     int b = fib(n - 2);

//     return a + b;
// }
namespace mozi::http
{
void mo_on_connect(uv_stream_t *server, int status) noexcept;

template <typename ReturnType, typename... Args>
void mo_route(mo_route_t *tree,                               //
              std::string_view path,                          //
              llhttp_method_t method,                         //
              mozi::mo_function_t<ReturnType, Args...> func); //

void mo_alloc_buffer(uv_handle_t *client,                 //
                     [[MO_UNUSED]] size_t suggested_size, //
                     uv_buf_t *buf) noexcept;             //

void mo_read_buffer(uv_stream_t *client,                         //
                    ssize_t nread,                               //
                    [[MO_UNUSED]] const uv_buf_t *buf) noexcept; //

void mo_write_end(uv_write_t *req, int status) noexcept;

void mo_error_code_handler(uv_handle_t *stream, ssize_t code) noexcept;

void mo_free_handle(uv_handle_t *handle) noexcept;

size_t mo_http_listen(uv_loop_t *loop, int32_t port) noexcept
{
    std::unique_ptr<struct sockaddr_in> addr_in = std::make_unique<struct sockaddr_in>();

    spdlog::info("Listen at port {}", port);
    int32_t address = uv_ip4_addr("0.0.0.0", port, addr_in.get());
    if (address) [[MO_UNLIKELY]]
    {
        spdlog::error("Address error {}", uv_strerror(address));
        return 1;
    };

    std::unique_ptr<uv_tcp_t> server = std::make_unique<uv_tcp_t>();
    uv_tcp_init(loop, server.get());
    uv_tcp_bind(server.get(), static_cast<const sockaddr *>(static_cast<void *>(addr_in.get())), 0);
    int r = uv_listen(static_cast<uv_stream_t *>(static_cast<void *>(server.get())), DEFAULT_BACKLOG, mo_on_connect);
    if (r) [[MO_UNLIKELY]]
    {
        spdlog::error("Listen error {}", uv_strerror(r));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}

void mo_on_connect(uv_stream_t *server, int status) noexcept
{
    if (status < 0) [[MO_UNLIKELY]]
    {
        spdlog::error("New connection error {}", uv_strerror(status));
        return;
    }
    spdlog::info("New connection");

    std::unique_ptr<uv_tcp_t> client = std::make_unique<uv_tcp_t>();
    uv_tcp_init(server->loop, client.get());
    std::shared_ptr<mo_responder_c> responder = mo_responder_c::create(std::move(client));
    uv_stream_t *client_stream = static_cast<uv_stream_t *>(static_cast<void *>(responder->client()));
    if (uv_accept(server, client_stream) == 0) [[MO_LIKELY]]
    {
        auto rr = uv_read_start(
            client_stream,
            [](uv_handle_t *client, [[MO_UNUSED]] size_t suggested_size, uv_buf_t *buf) noexcept {
                auto responder = static_cast<mo_responder_c *>(client->data);
                responder->add_message(buf);
            },
            [](uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) noexcept {
                auto responder = static_cast<mo_responder_c *>(client->data);
                responder->read_message(nread, buf);
            });
        if (UV_EINVAL == rr)
        {
            spdlog::error("uv_read_start error {}", uv_strerror(rr));
        }
    }
    else
    {
        spdlog::info("new connection fail");
        uv_close(static_cast<uv_handle_t *>(static_cast<void *>(responder->client())), [](uv_handle_t *client) {
            auto responder = static_cast<mo_responder_c *>(client->data);
            responder->reset(client->loop, responder->id());
        });
    }
}
} // namespace mozi::http

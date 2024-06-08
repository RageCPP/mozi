#pragma once
#include "mozi/http/llhttp/llhttp.h"
#include "mozi/http/types.hpp"
#include "mozi/utils/traits.hpp"
#include <string_view>

namespace mozi::http
{

template <typename ReturnType, typename... Args>
void mo_route(mo_route_t *tree, std::string_view path, llhttp_method_t method,
              mozi::mo_function_t<ReturnType, Args...> func)
{
    if (method == llhttp_method::HTTP_GET)
    {
        r3_tree_insert_routel(tree, METHOD_GET, path.data(), path.length(), &func);
    }
    else if (method == llhttp_method::HTTP_POST)
    {
        r3_tree_insert_routel(tree, METHOD_POST, path.data(), path.length(), &func);
    }
}
int64_t mo_route_finish(mo_route_t *tree)
{
    return r3_tree_compile(tree, nullptr);
}
} // namespace mozi::http

// auto loop_data = static_cast<mo_loop_data_t *>(loop->data);
//  Route(loop_data->rageroute, hello, Method::GET, mo_function_t<void, mo_responder_c &>([](mo_responder_c
//  &responder) {
//            uv_write_t *writer = (uv_write_t *)malloc(sizeof(uv_write_t));
//            char *info = new char[HTTP_HELLO_MESSAGE.size() + 1];
//            memcpy(info, HTTP_HELLO_MESSAGE.data(), HTTP_HELLO_MESSAGE.size());
//            info[HTTP_HELLO_MESSAGE.size()] = '\0';
//            uv_buf_t response_buf = uv_buf_init(info, HTTP_HELLO_MESSAGE.size());
//            writer->data = info;
//            fib(14);
//            uv_write(writer, (uv_stream_t *)(responder.Client()), &response_buf, 1, writeEnd);
//            return;
//        }));

// cmake -B ./build/ -S .   -DCMAKE_BUILD_TYPE=Release   -DBUILD_SHARED_LIBS=OFF
// -DCMAKE_PREFIX_PATH="/home/rage/code/chessmeta/lib:/home/rage/code/chessmeta/lib64:${CMAKE_PREFIX_PATH}"
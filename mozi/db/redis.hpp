#pragma once

#include "spdlog/spdlog.h"
#include <cstddef>
#include <exception>
#include <hiredis/adapters/libuv.h>
#include <hiredis/async.h>
#include <hiredis/hiredis.h>
#include <uv.h>

namespace mozi::redis
{

namespace
{
void mo_connected(const redisAsyncContext *context, int status)
{
    if (status != REDIS_OK)
    {
        spdlog::info("Error: {}", context->errstr);
    }
    spdlog::info("Connected...");
    return;
}
void mo_disconnect(const redisAsyncContext *context, int status)
{
    if (status != REDIS_OK)
    {
        spdlog::info("Error: {}", context->errstr);
    }
    spdlog::info("Disconnected...");
    return;
}
} // namespace

size_t mo_redis_listen(uv_loop_t *loop, int32_t port) noexcept
{
    redisAsyncContext *context = redisAsyncConnect("127.0.0.1", port);
    if (context->err)
    {
        // TODO 错误处理
        std::terminate();
        /* Let *context to cleanup */
        return 1;
    }
    redisLibuvAttach(context, loop);
    redisAsyncSetConnectCallback(context, mo_connected);
    redisAsyncSetDisconnectCallback(context, mo_disconnect);
    redisAsyncSetTimeout(context, {.tv_sec = 1, .tv_usec = 0});
    return 0;
}
} // namespace mozi::redis
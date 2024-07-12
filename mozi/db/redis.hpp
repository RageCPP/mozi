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

void debugCallback(redisAsyncContext *c, void *r, void *privdata)
{
    (void)privdata; // unused
    redisReply *reply = static_cast<redisReply *>(r);
    if (reply == NULL)
    {
        /* The DEBUG SLEEP command will almost always fail, because we have set a 1 second timeout */
        printf("`DEBUG SLEEP` error: %s\n", c->errstr ? c->errstr : "unknown error");
        return;
    }
    /* Disconnect after receiving the reply of DEBUG SLEEP (which will not)*/
    redisAsyncDisconnect(c);
}
void getCallback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = static_cast<redisReply *>(r);
    if (reply == NULL)
    {
        printf("`GET key` error: %s\n", c->errstr ? c->errstr : "unknown error");
        return;
    }
    printf("`GET key` result: argv[%s]: %s\n", (char *)privdata, reply->str);

    /* start another request that demonstrate timeout */
    redisAsyncCommand(c, debugCallback, NULL, "DEBUG SLEEP %f", 1.5);
}
} // namespace

size_t mo_redis_listen(uv_loop_t *loop, int32_t port) noexcept
{
    [[maybe_unused]] redisAsyncContext *context = redisAsyncConnect("127.0.0.1", port);
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
    redisAsyncCommand(context, NULL, NULL, "SET key %b", "hello", strlen("hello"));
    redisAsyncCommand(context, getCallback, (char *)"end-1", "GET key");
    return 0;
}
} // namespace mozi::redis
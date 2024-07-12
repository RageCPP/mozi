#include "mozi/db/redis.hpp"
#include <mozi/core/run.hpp>
#include <uv.h>

int main()
{
    auto loop = std::make_unique<uv_loop_t>();
    auto loop_raw = loop.release();
    uv_loop_init(loop_raw);
    mozi::redis::mo_redis_listen(loop_raw, 6379);
    uv_run(loop_raw, UV_RUN_DEFAULT);

    return 0;
}
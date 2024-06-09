#pragma once

#include "mozi/utils/ring_buffer.hpp"
#include <cstddef>
namespace mozi::actor
{
template <typename T, size_t N> struct mo_mail_cache_s : public mozi::ring_buffer<T, N>
{
    mo_mail_cache_s() : mozi::ring_buffer<T, N>(){};
    ~mo_mail_cache_s(){};
    mo_mail_cache_s(const mo_mail_cache_s &) = delete;
    mo_mail_cache_s &operator=(const mo_mail_cache_s &) = delete;
    mo_mail_cache_s(mo_mail_cache_s &&) = delete;
    mo_mail_cache_s &operator=(mo_mail_cache_s &&) = delete;
};
} // namespace mozi::actor
#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <sys/types.h>

namespace mozi
{
template <typename T, uint16_t N> struct ring_buffer
{
    ring_buffer()
        : m_size{N}, m_mask{std::numeric_limits<size_t>::max()}, m_buffer(std::make_unique<std::array<T, N>>())
    {
        static_assert(m_size > 0, "size must > 0");
    }
    size_t size() const
    {
        return m_size;
    }
    T &operator[](size_t seq)
    {
        return m_buffer[seq & m_mask];
    }
    const T &operator[](size_t seq) const
    {
        return m_buffer[seq & m_mask];
    }
    ~ring_buffer(){};
    ring_buffer(const ring_buffer &) = delete;
    ring_buffer &operator=(const ring_buffer &) = delete;
    ring_buffer(ring_buffer &&) = delete;
    ring_buffer &operator=(ring_buffer &&) = delete;

  private:
    uint16_t m_size;
    size_t m_mask;
    std::unique_ptr<std::array<T, N>> m_buffer;
};
} // namespace mozi
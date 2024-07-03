#pragma once

#include "mozi/utils/expected.hpp"
#include "mozi/utils/traits.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
namespace mozi
{

// BOTTOM 推入新任务 BOTTOM 取出任务
// TOP窃取任务
template <typename T> class mo_deque_c
{
    static constexpr std::memory_order relaxed = std::memory_order_relaxed;
    static constexpr std::memory_order acquire = std::memory_order_acquire;
    static constexpr std::memory_order release = std::memory_order_release;
    static constexpr std::memory_order seq_cst = std::memory_order_seq_cst;
    static constexpr std::memory_order consume = std::memory_order_consume;

    // clang-format off
    struct mo_array
    {
        int64_t m_capacity;
        int64_t m_mask;
        std::atomic<T> *m_data;
        explicit mo_array(int64_t c) : m_capacity{c}, m_mask{c - 1}, m_data{new std::atomic<T>[static_cast<size_t>(m_capacity)]}
        {
            // TODO 检查 capacity 是否是 2 的幂
        }
        mo_array(const mo_array &) = delete;
        mo_array &operator=(mo_array &&) = delete;
        mo_array &operator=(const mo_array &) = delete;
        mo_array(mo_array &&) = delete;
        ~mo_array()
        {
            delete[] m_data;
        }
        int64_t capacity() const noexcept
        {
            return m_capacity;
        }
        template <typename O> void push(int64_t i, O &&o) noexcept
        {
            m_data[i & m_mask].store(std::forward<O>(o), relaxed);
        }

        T pop(int64_t i) noexcept
        {
            return m_data[i & m_mask].load(relaxed);
        }

        mo_array *resize(int64_t b, int64_t t)
        {
            mo_array *ptr = new mo_array{2 * m_capacity};
            for (int64_t i = t; i != b; ++i)
            {
                ptr->push(i, pop(i));
            }
            return ptr;
        }
    };
    // clang-format on

    std::atomic<int64_t> m_top;
    std::atomic<int64_t> m_bottom;
    std::atomic<mo_array *> m_array;
    std::vector<mo_array *> m_garbage;

  public:
    /**
    @brief constructs the queue with a given capacity

    @param capacity the capacity of the queue (must be power of 2)
    */
    explicit mo_deque_c(int64_t capacity = 1024);

    /**
    @brief destructs the queue
    */
    ~mo_deque_c();

    /**
    @brief queries if the queue is empty at the time of this call
    */
    bool empty() const noexcept;

    /**
    @brief queries the number of items at the time of this call
    */
    size_t size() const noexcept;

    /**
    @brief queries the capacity of the queue
    */
    int64_t capacity() const noexcept;

    /**
    @brief inserts an item to the queue

    Only the owner thread can insert an item to the queue.
    The operation can trigger the queue to resize its capacity
    if more space is required.

    @tparam O data type

    @param item the item to perfect-forward to the queue
    */
    template <typename O> void push(O &&item);

    /**
    @brief pops out an item from the queue

    Only the owner thread can pop out an item from the queue.
    The return can be a @std_nullopt if this operation failed (empty queue).
    */
    std::optional<T> pop();

    /**
    @brief steals an item from the queue

    Any threads can try to steal an item from the queue.
    The return can be a @std_nullopt if this operation failed (not necessary empty).
    */
    std::optional<T> steal();
};

// Constructor
template <typename T> mo_deque_c<T>::mo_deque_c(int64_t c)
{
    assert(c && (!(c & (c - 1))));
    m_top.store(0, relaxed);
    m_bottom.store(0, relaxed);
    m_array.store(new mo_array{c}, relaxed);
    m_garbage.reserve(32);
}

// Destructor
template <typename T> mo_deque_c<T>::~mo_deque_c()
{
    for (auto a : m_garbage)
    {
        delete a;
    }
    delete m_array.load();
}

// Function: empty
template <typename T> bool mo_deque_c<T>::empty() const noexcept
{
    int64_t b = m_bottom.load(relaxed);
    int64_t t = m_top.load(relaxed);
    return b <= t;
}

// Function: size
template <typename T> size_t mo_deque_c<T>::size() const noexcept
{
    int64_t b = m_bottom.load(relaxed);
    int64_t t = m_top.load(relaxed);
    return static_cast<size_t>(b >= t ? b - t : 0);
}

// Function: push
template <typename T> template <typename O> void mo_deque_c<T>::push(O &&o)
{
    int64_t b = m_bottom.load(relaxed);
    int64_t t = m_top.load(acquire);
    mo_array *a = m_array.load(relaxed);

    // queue is full
    if (a->capacity() - 1 < (b - t))
    {
        mo_array *tmp = a->resize(b, t);
        m_garbage.push_back(a);
        std::swap(a, tmp);
        m_array.store(a, relaxed);
    }

    a->push(b, std::forward<O>(o));
    std::atomic_thread_fence(release);
    m_bottom.store(b + 1, relaxed);
}

// Function: pop
template <typename T> std::optional<T> mo_deque_c<T>::pop()
{
    int64_t b = m_bottom.load(relaxed) - 1;
    mo_array *a = m_array.load(relaxed);
    m_bottom.store(b, relaxed);
    std::atomic_thread_fence(seq_cst);
    int64_t t = m_top.load(relaxed);

    std::optional<T> item;

    if (t <= b)
    {
        item = a->pop(b);
        if (t == b)
        {
            // the last item just got stolen
            if (!m_top.compare_exchange_strong(t, t + 1, seq_cst, relaxed))
            {
                item = std::nullopt;
            }
            m_bottom.store(b + 1, relaxed);
        }
    }
    else
    {
        m_bottom.store(b + 1, relaxed);
    }

    return item;
}

// Function: steal
template <typename T> std::optional<T> mo_deque_c<T>::steal()
{
    int64_t t = m_top.load(acquire);
    std::atomic_thread_fence(seq_cst);
    int64_t b = m_bottom.load(acquire);

    std::optional<T> item;

    if (t < b)
    {
        mo_array *a = m_array.load(consume);
        item = a->pop(t);
        if (!m_top.compare_exchange_strong(t, t + 1, seq_cst, relaxed))
        {
            return std::nullopt;
        }
    }

    return item;
}

// Function: capacity
template <typename T> int64_t mo_deque_c<T>::capacity() const noexcept
{
    return m_array.load(relaxed)->capacity();
}
} // namespace mozi
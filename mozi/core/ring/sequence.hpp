#pragma once

#include "mozi/compile/attributes_cpp.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace mozi::ring
{
// TODO: 添加一个移除方法 移除后需要将其从gating_sequences中移除
class mo_sequence_c
{
  public:
    explicit mo_sequence_c(size_t value = INITIAL_VALUE) : mo_value(value)
    {
    }
    [[MO_NODISCARD]] size_t value() const noexcept
    {
        return mo_value.load();
    }
    [[MO_NODISCARD]] std::string to_string() const noexcept
    {
        return std::to_string(mo_value);
    }
    void set(size_t value) noexcept
    {
        mo_value.store(value);
    }
    bool compare_and_set(size_t expected_value, size_t new_value) noexcept
    {
        // TODO: 内存顺序优化
        size_t value = mo_value.load();
        bool o = !static_cast<bool>(value ^ expected_value);
        mo_value.store(value * static_cast<size_t>(!o) + new_value * static_cast<size_t>(o));
        return o;
    }
    size_t add_and_get(size_t increment) noexcept
    {
        // TODO: 内存顺序优化
        return mo_value += increment;
    }
    [[MO_NODISCARD]] size_t increment() noexcept
    {
        // TODO: 内存顺序优化
        return mo_value += 1;
    }
    static const size_t INITIAL_VALUE = SIZE_MAX;

  private:
    std::atomic<size_t> mo_value;
};
using mo_sequence_t = mo_sequence_c;
using mo_arc_sequence_t = std::shared_ptr<mo_sequence_t>;
} // namespace mozi::ring
#pragma once

#include "mozi/compile/attributes_cpp.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>

namespace mozi::ring
{
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
        bool o = !static_cast<bool>(mo_value ^ expected_value);
        mo_value = mo_value * static_cast<size_t>(!o) + new_value * static_cast<size_t>(o);
        return o;
    }
    [[MO_NODISCARD]] size_t increment() noexcept
    {
        return mo_value += 1;
    }
    static const int64_t INITIAL_VALUE = -1;

  private:
    std::atomic<size_t> mo_value;
};
using mo_sequence_t = mo_sequence_c;
} // namespace mozi::ring
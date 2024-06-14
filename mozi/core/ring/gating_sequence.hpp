#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "spdlog/spdlog.h"
#include <cstddef>
namespace mozi::ring
{
struct mo_gating_sequence_c
{
    explicit mo_gating_sequence_c(size_t value = INITIAL_VALUE) : mo_value(value)
    {
    }
    [[MO_NODISCARD]] size_t value() const noexcept
    {
        return mo_value;
    }
    bool set_minimum(size_t new_value) noexcept
    {
        bool mini_n = new_value < mo_value;
        mo_value = mo_value * static_cast<size_t>(!mini_n) + new_value * static_cast<size_t>(mini_n);
        return mini_n;
    }
    template <typename... Args> void set_sequences(Args... sequences) noexcept
    {
        static_assert((std::is_same_v<Args, mo_sequence_t> && ...), "All Args must be of type mo_sequence_t");
        auto list = std::make_tuple(sequences...);
        std::apply([this](auto &&...args) { ((this->set_minimum(args.value())), ...); }, list);
    }

  private:
    static const size_t INITIAL_VALUE = 0;
    size_t mo_value;
};
using mo_gating_sequence_t = mo_gating_sequence_c;
} // namespace mozi::ring
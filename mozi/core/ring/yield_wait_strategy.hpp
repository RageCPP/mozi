#pragma once
#include "mozi/core/ring/wait_strategy.hpp"
#include "mozi/utils/expected.hpp"
#include <cstddef>
namespace mozi::ring
{
class yield_wait_strategy_c : public mo_wait_strategy_t<yield_wait_strategy_c>
{
  public:
    // clang-format off
    mozi::mo_expected_t<size_t, mo_alerted_true> wait_for(
        [[maybe_unused]] const size_t sequence,
        const mo_sequence_t& dependent_sequence) const noexcept
    // clang-format on
    {
        return dependent_sequence.value();
    }

    void signal_all_when_blocking() noexcept
    {
    }
};
using yield_wait_strategy_t = yield_wait_strategy_c;
} // namespace mozi::ring
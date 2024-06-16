#pragma once
#include "mozi/core/ring/processing_sequence_barrier.hpp"
#include "mozi/core/ring/wait_strategy.hpp"
#include "mozi/utils/expected.hpp"
namespace mozi::ring
{
template <class SI>
class yield_wait_strategy_c : public mo_wait_strategy_t<yield_wait_strategy_c<SI>,
                                                        mo_processing_sequence_barrier_c<SI, yield_wait_strategy_c<SI>>>
{
  public:
    // clang-format off
    mozi::mo_expected_t<size_t, mo_alerted_true> wait_for(
        [[maybe_unused]] size_t sequence,
        [[maybe_unused]] mo_sequence_t&& cursor,
        [[maybe_unused]] mo_sequence_t&& dependent_sequence) noexcept
    {
        return 1;
    }
    // clang-format on

    void signal_all_when_blocking() noexcept
    {
    }
};
template <class SI> using yield_wait_strategy_t = yield_wait_strategy_c<SI>;
} // namespace mozi::ring
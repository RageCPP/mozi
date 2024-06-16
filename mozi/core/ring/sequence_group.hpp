#pragma once

#include "mozi/core/ring/sequence.hpp"
#include <cstddef>
#include <memory>
#include <vector>
namespace mozi::ring::sequence_group
{
namespace
{
// template <typename T>
// size_t count_matching([[MO_UNUSED]] const std::vector<T> &gating_sequences, [[MO_UNUSED]] const T &sequence) {};
};
template <class Sequencer, typename... Args> void add_sequences(Sequencer &sequences, Args... sequence)
{
    static_assert((std::is_same_v<Args, std::shared_ptr<mo_sequence_t>> && ...),
                  "All Args must be of type  std::atomic<std::shared_ptr<mo_sequence_t>>");
    auto list = std::make_tuple(sequence...);
    // TODO: src\main\java\com\lmax\ring\SequenceGroups.java
    // 通过 std::atomic::compare_exchange_strong 来更新对应的指针
    [[MO_UNUSED]] std::atomic<std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>>> &gating_sequences =
        &sequences.m_gating_sequences;
    // std::apply(
    //     [&sequences](auto &&...args) {
    //         (([args = std::move(args)]() {

    //          }),
    //          ...);
    //     },
    //     list);
};
template <class Sequencer>
void remove_sequences(Sequencer &sequences, [[MO_UNUSED]] std::shared_ptr<mo_sequence_t> &sequence)
{
    // TODO: src\main\java\com\lmax\ring\SequenceGroups.java
    // 通过 std::atomic::compare_exchange_strong 来更新对应的指针
    [[MO_UNUSED]] std::atomic<std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>>> &gating_sequences =
        &sequences.m_gating_sequences;
};
} // namespace mozi::ring::sequence_group
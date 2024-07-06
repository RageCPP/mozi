#pragma once

#include "mozi/core/ring/sequence.hpp"
#include <cstddef>
#include <memory>
#include <vector>
namespace
{
size_t inline count_match(std::vector<mozi::ring::mo_arc_sequence_t> *sequences,
                          mozi::ring::mo_arc_sequence_t &to_match)
{
    size_t count = 0;
    for (auto &value : *sequences)
    {
        if (value == to_match)
        {
            count++;
        }
    }
    return count;
}
} // namespace
namespace mozi::ring::sequence_group
{
template <class Sequencer, typename... Sequences> void add_sequences(Sequencer *sequencer, Sequences &&...sequences)
{
    static_assert((std::is_same_v<Sequences, mo_arc_sequence_t> && ...),
                  "All Args must be of type  std::atomic<std::shared_ptr<mo_sequence_t>>");
    auto num = sizeof...(sequences);
    auto sequence_add = std::make_tuple(sequences...);
    size_t cursor_sequence;
    std::vector<mo_arc_sequence_t> *current_gating_sequences = nullptr;
    std::vector<mo_arc_sequence_t> *updated_gating_sequences = new std::vector<mo_arc_sequence_t>{};

    while (true)
    {
        updated_gating_sequences->clear();
        current_gating_sequences = sequencer->m_gating_sequences.load();
        updated_gating_sequences->reserve(current_gating_sequences->size() + num);
        cursor_sequence = sequencer->m_cursor->value();

        // clang-format off
        std::copy(current_gating_sequences->begin(),
                  current_gating_sequences->end(),
                  std::back_inserter(*updated_gating_sequences));
        // clang-format on
        std::apply(
            [&](auto &&...sequence) {
                (..., [&] {
                    sequence->set(cursor_sequence);
                    updated_gating_sequences->push_back(sequence);
                }());
            },
            sequence_add);

        if (sequencer->m_gating_sequences.compare_exchange_weak(current_gating_sequences, updated_gating_sequences))
        {
            delete current_gating_sequences;
            break;
        }
    }

    cursor_sequence = sequencer->m_cursor->value();
    std::apply([&](auto &&...sequence) { (..., [&] { sequence->set(cursor_sequence); }()); }, sequence_add);
};

template <class Sequencer> bool remove_sequences(Sequencer *sequences, mo_arc_sequence_t &sequence)
{
    size_t num_to_remove;
    std::vector<mo_arc_sequence_t> *old_sequences;
    std::vector<std::shared_ptr<mo_sequence_t>> *new_sequences = new std::vector<mo_arc_sequence_t>{};

    while (true)
    {
        new_sequences->clear();

        old_sequences = sequences->m_gating_sequences.load();
        num_to_remove = count_match(old_sequences, sequence);

        if (num_to_remove == 0)
        {
            break;
        }

        size_t old_size = old_sequences->size();
        new_sequences->reserve(old_size - num_to_remove);

        for (auto gating_sequence : *old_sequences)
        {
            if (gating_sequence != sequence)
            {
                new_sequences->push_back(gating_sequence);
            }
        }

        if (sequences->m_gating_sequences.compare_exchange_weak(old_sequences, new_sequences))
        {
            delete old_sequences;
            break;
        }
    }

    return num_to_remove != 0;
};
} // namespace mozi::ring::sequence_group
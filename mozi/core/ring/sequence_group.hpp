#pragma once

#include "mozi/core/ring/sequence.hpp"
#include <cstddef>
#include <memory>
#include <vector>
namespace mozi::ring::sequence_group
{
template <class Sequencer, typename... Sequences> void add_sequences(Sequencer *sequencer, Sequences &&...sequences)
{
    static_assert((std::is_same_v<Sequences, std::shared_ptr<mo_sequence_t>> && ...),
                  "All Args must be of type  std::atomic<std::shared_ptr<mo_sequence_t>>");
    auto list = std::make_tuple(sequences...);
    // TODO: src\main\java\com\lmax\ring\SequenceGroups.java
    // 通过 std::atomic::compare_exchange_strong 来更新对应的指针
    std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>> current_gating_sequences;
    std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>> updated_gating_sequences;
    size_t cursor_sequence = sequencer->m_cursor.value();
    for (;;)
    {
        current_gating_sequences = sequencer->m_gating_sequences.load();
        updated_gating_sequences = std::make_shared<std::vector<std::shared_ptr<mo_sequence_t>>>();
        updated_gating_sequences->reserve(current_gating_sequences->size() + sizeof...(sequences));
        std::copy(current_gating_sequences->begin(), current_gating_sequences->end(),
                  std::back_inserter(*updated_gating_sequences));
        std::apply(
            [&updated_gating_sequences, cursor_sequence](auto &&...add_sequence) {
                (([&updated_gating_sequences, add_sequence = std::move(add_sequence), cursor_sequence]() {
                     add_sequence->set(cursor_sequence);
                     updated_gating_sequences->push_back(add_sequence);
                 }),
                 ...);
            },
            list);

        if (sequencer->m_gating_sequences.compare_exchange_strong(current_gating_sequences, updated_gating_sequences))
            [[MO_LIKELY]]
        {
            break;
        }
    }
};
template <class Sequencer> bool remove_sequences(Sequencer *sequences, std::shared_ptr<mo_sequence_t> &sequence)
{
    std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>> old_gating_sequences;
    std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>> new_gating_sequences;
    auto removed = false;
    for (;;)
    {
        old_gating_sequences = sequences->m_gating_sequences.load();
        new_gating_sequences = std::make_shared<std::vector<std::shared_ptr<mo_sequence_t>>>();
        new_gating_sequences->reserve(old_gating_sequences->size());
        for (auto &gating_sequence : *old_gating_sequences)
        {
            if (gating_sequence != sequence)
            {
                new_gating_sequences->push_back(gating_sequence);
            }
            else
            {
                removed = true;
            }
        }
        if (sequences->m_gating_sequences.compare_exchange_strong(old_gating_sequences, new_gating_sequences))
            [[MO_LIKELY]]
        {
            break;
        }
    }
    return removed;
};
} // namespace mozi::ring::sequence_group
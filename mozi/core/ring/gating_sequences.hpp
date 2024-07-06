#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/ring/sequence.hpp"
#include <cstddef>
#include <vector>
namespace mozi::ring
{
struct mo_wait_sequences_c
{
    /**
     * Get the minimum sequence value for the group.
     *
     * @return the minimum sequence value for the group.
     */
    [[MO_NODISCARD]] size_t value() const noexcept
    {
        size_t mini = mo_sequences[0]->value();
        for (auto &sequence : mo_sequences)
        {
            mini = std::min(mini, sequence->value());
        }
        return mini;
    }
    // bool set_minimum(size_t new_value) noexcept
    // {
    //     bool mini_n = new_value < mo_value;
    //     mo_value = mo_value * static_cast<size_t>(!mini_n) + new_value * static_cast<size_t>(mini_n);
    //     return mini_n;
    // }
    template <typename... Sequences> void set_sequences(Sequences &...sequences) noexcept
    {
        // TODO 预分配新增空间优化
        static_assert((std::is_same_v<Sequences, mo_arc_sequence_t> && ...),
                      "All Args must be of type mo_arc_sequence_t");
        auto list = std::make_tuple(sequences...);
        mo_sequences.reserve(sizeof...(Sequences));
        std::apply([this](auto &&...args) { ((this->mo_sequences.push_back(args)), ...); }, list);
    }
    void set_sequences(mo_arc_sequence_t &&sequence) noexcept
    {
        mo_sequences.push_back(sequence);
    }

  private:
    std::vector<mo_arc_sequence_t> mo_sequences{};
};
using mo_wait_sequences_t = mo_wait_sequences_c;
} // namespace mozi::ring
#pragma once

#include "mozi/core/ring/sequence.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace mozi::ring
{
namespace utils
{
inline size_t minimum_sequence(std::vector<mo_arc_sequence_t> *sequences, size_t minimum)
{
    auto _sequences = *sequences;
    size_t minimum_sequence = minimum;
    for (size_t i = 0; i < _sequences.size(); i++)
    {
        minimum_sequence = std::min(minimum_sequence, _sequences[i]->value());
    };
    return minimum_sequence;
}
inline size_t minimum_sequence(std::vector<mo_arc_sequence_t> *sequences)
{
    size_t max = SIZE_MAX;
    return minimum_sequence(sequences, max);
}
} // namespace utils
} // namespace mozi::ring
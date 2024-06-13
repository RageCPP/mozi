#pragma once

#include "mozi/core/disruptor/sequence.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace mozi::disruptor
{
namespace utils
{
inline size_t minimum_sequence(std::vector<std::shared_ptr<mo_sequence_t>> &sequences, size_t minimum)
{
    size_t minimum_sequence = minimum;
    for (size_t i = 0; i < sequences.size(); i++)
    {
        minimum_sequence = std::min(minimum_sequence, sequences[i]->value());
    };
    return minimum_sequence;
}
inline size_t minimum_sequence(std::vector<std::shared_ptr<mo_sequence_t>> &sequences)
{
    size_t max = SIZE_MAX;
    return minimum_sequence(sequences, max);
}
} // namespace utils
} // namespace mozi::disruptor
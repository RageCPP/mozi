#pragma once

#include "mozi/core/ring/sequence.hpp"
#include "spdlog/spdlog.h"
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
    auto mini_size = _sequences.size();
    size_t minimum_sequence = minimum;
    if (mini_size == 0)
    {
        return minimum_sequence;
    }

    minimum_sequence = _sequences[0]->value();

    bool need_greater_minimum = false;

    for (size_t i = 1; i < mini_size; i++)
    {
        if (_sequences[i]->value() > minimum) [[MO_UNLIKELY]]
        {
            need_greater_minimum = true;
            break;
        }
    }

    for (size_t i = 0; i < mini_size; i++)
    {
        if (need_greater_minimum && _sequences[i]->value() < minimum) [[MO_UNLIKELY]]
        {
            continue;
        }
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
// #include "mozi/core/ring/gating_sequence.hpp"
// #include "mozi/core/ring/sequence.hpp"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

// Demonstrate some basic assertions.
TEST(MoziGatingSequenceTest, mo_gating_sequence_t)
{
    // using namespace mozi::ring;

    // mo_gating_sequence_t gatting_sequence{};
    // EXPECT_EQ(gatting_sequence.value(), 0);

    // mo_gating_sequence_t gatting_sequence2{10};
    // EXPECT_EQ(gatting_sequence2.value(), 10);
    // auto mini = gatting_sequence2.set_minimum(5);
    // EXPECT_EQ(gatting_sequence2.value(), 5);
    // EXPECT_TRUE(mini);

    // mini = gatting_sequence2.set_minimum(15);
    // EXPECT_EQ(gatting_sequence2.value(), 5);
    // EXPECT_FALSE(mini);

    // mini = gatting_sequence2.set_minimum(5);
    // EXPECT_EQ(gatting_sequence2.value(), 5);
    // EXPECT_FALSE(mini);
    // auto _1 = mo_sequence_t{1};
    // auto _2 = mo_sequence_t{2};
    // auto _3 = mo_sequence_t{3};
    // gatting_sequence2.set_sequences(_2, _1, _3);
    // EXPECT_EQ(gatting_sequence2.value(), 1);

    // gatting_sequence2.set_sequences(mo_sequence_t{10}, mo_sequence_t{5}, mo_sequence_t{15});
    // EXPECT_EQ(gatting_sequence2.value(), 1);
}

TEST(MoziGatingSequenceTest, mo_sequence_t)
{
    // using namespace mozi::ring;

    // mo_sequence_t sequence{};
    // EXPECT_EQ(sequence.value(), std::numeric_limits<size_t>::max());
    // auto value = sequence.increment();
    // EXPECT_EQ(value, 0);
    // EXPECT_EQ(sequence.value(), 0);

    // mo_sequence_t sequence2{10};
    // EXPECT_EQ(sequence2.value(), 10);
    // auto updated = sequence2.compare_and_set(10, 5);
    // EXPECT_EQ(sequence2.value(), 5);
    // EXPECT_TRUE(updated);

    // updated = sequence2.compare_and_set(10, 5);
    // EXPECT_EQ(sequence2.value(), 5);
    // EXPECT_FALSE(updated);

    // value = sequence2.increment();
    // EXPECT_EQ(value, 6);
}
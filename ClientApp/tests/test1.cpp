#include <gtest/gtest.h>

#include "math_utils.h"

TEST(HelloTest, BasicAssertions)
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(MathTest, BasicAddition)
{
    EXPECT_EQ(add(1, 2), 3);
    EXPECT_EQ(add(-1, -1), -2);
}
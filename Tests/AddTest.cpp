
#include "Lib/Add.h"
#include <gtest/gtest.h>

TEST(AddTest, PositiveAddition) {
    EXPECT_EQ(3, add(1, 2));
}

TEST(AddTest, NegativeAddition) {
    EXPECT_EQ(-2, add(-1, -1));
}

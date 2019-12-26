
#include "Lib/IntegralImage.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(IntegralImage, All) {
    // 0 1 2
    // 1 2 3
    Image<int> image(3, 2);
    image.get({0, 0}) = 0;
    image.get({1, 0}) = 1;
    image.get({2, 0}) = 2;
    image.get({0, 1}) = 1;
    image.get({1, 1}) = 2;
    image.get({2, 1}) = 3;

    EXPECT_EQ(image.get({0, 0}), 0);
    EXPECT_EQ(image.get({2, 1}), 3);
    IntegralImage<int> integral(image);

    EXPECT_EQ(integral.get({1, 1}, {2, 2}), 2);
    EXPECT_EQ(integral.get({0, 0}, {3, 2}), 9);

    EXPECT_EQ(integral.getInRange({0, 0}, 1), 4);
    EXPECT_EQ(integral.getInRange({2, 1}, 0), 3);
    EXPECT_EQ(integral.getInRange({2, 1}, 1), 8);

    EXPECT_EQ(image.sizeX(), 3);
    EXPECT_EQ(image.sizeY(), 2);
    EXPECT_EQ(integral.sizeX(), 3);
    EXPECT_EQ(integral.sizeY(), 2);
}

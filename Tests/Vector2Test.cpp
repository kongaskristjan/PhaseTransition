
#include "Lib/Particle.h"
#include <gtest/gtest.h>
#include <cmath>

TEST(Vector2Test, Mixed) {
    Vector2D a(4, 3), b(1, 0);
    EXPECT_VECTOR2_EQ(a + b, Vector2D(5, 3));
    EXPECT_VECTOR2_EQ(a - b, Vector2D(3, 3));
    EXPECT_VECTOR2_EQ(a * 2., Vector2D(8, 6));
    EXPECT_VECTOR2_EQ(a / 2., Vector2D(2, 1.5));
    EXPECT_VECTOR2_EQ(-a, Vector2D(-4, -3));

    EXPECT_DOUBLE_EQ(a.magnitude(), 5);
    EXPECT_DOUBLE_EQ(dotProduct(a, a), 5 * 5);
    EXPECT_DOUBLE_EQ(dotProduct(a, b), 4);
    EXPECT_DOUBLE_EQ(crossProduct(a, a), 0);
}

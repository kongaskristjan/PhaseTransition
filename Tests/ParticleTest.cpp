
#include "Lib/Particle.h"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>

TEST(ParticleTest, ForcePlausibility) {
    ParticleType type0(1, 1, 1, 1, 10);
    ParticleType type1(2, 2, 2, 2, 20);
    ParticleState state0(Vector2D(0, 0));
    ParticleState state1(Vector2D(2, 0));

    Vector2D force0 = type0.computeForce(type1, state0, state1);
    Vector2D force1 = type1.computeForce(type0, state1, state0);
    Vector2D forceSelf = type0.computeForce(type0, state0, state0);

    EXPECT_GT(force0.magnitude(), 0);
    EXPECT_DOUBLE_EQ(force0.y, 0);
    EXPECT_VECTOR2_EQ(forceSelf, Vector2D(0, 0));
    EXPECT_VECTOR2_EQ(force0, -force1);
}

TEST(ParticleTest, ForceSmoothness) {
    ParticleType type(1, 1, 1, 1, 10);
    ParticleState state0(Vector2D(0, 0));
    double range = type.getRange();

    std::vector<double> forces;
    const double diff = 1e-3;
    for(double x = -range - 1; x < range + 1; x += diff) {
        ParticleState state(Vector2D(x, 0));
        forces.push_back(type.computeForce(type, state, state0).x);
    }

    for(size_t i = 1; i < forces.size() - 1; i += ++i) {
        ASSERT_NEAR((forces[i - 1] + forces[i + 1]) / 2, forces[i], 10 * diff * diff);
    }
}

TEST(ParticleTest, ForceRange) {
    ParticleType type0(1, 1, 1, 1, 3);
    ParticleType type1(2, 2, 2, 2, 3);

    double range = std::min(type0.getRange(), type1.getRange());
    ParticleState state0(Vector2D(0, 0));
    ParticleState state1(Vector2D(range - 1., 0));
    ParticleState state2(Vector2D(range, 0));

    EXPECT_DOUBLE_EQ(type0.computeForce(type1, state0, state2).x, 0);
}

TEST(ParticleTest, Derivative) {
    ParticleType type(2, 1, 1, 1, 10);
    ParticleState state(Vector2D(1, 1), Vector2D(1, 1));
    Vector2D force(Vector2D(1, 1));

    ParticleState der = type.derivative(state, force);
    EXPECT_VECTOR2_EQ(der.pos, Vector2D(1, 1));
    EXPECT_VECTOR2_EQ(der.v, Vector2D(0.5, 0.5));
}

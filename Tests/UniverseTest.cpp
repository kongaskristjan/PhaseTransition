
#include "Lib/Universe.h"
#include <gtest/gtest.h>

TEST(UniverseTest, ParticleScatter) {
    Universe universe(400, 400, 1, 0);
    ParticleType type(1, 1, 1, 1, 10);
    ParticleState state0(Vector2D(200, 200)); // Standing particle
    ParticleState state1(Vector2D(150, 199), Vector2D(1, 0)); // Particle to be deflected
    universe.addParticle(type, state0);
    universe.addParticle(type, state1);

    const double dT = 1e-2;
    double t = 0;
    for(; t < 30; t += dT) universe.advance(dT); // Particles haven't yet interacted

    EXPECT_NE(std::get<ParticleState>(universe.getParticle(1)).pos.x, state1.pos.x);
    EXPECT_DOUBLE_EQ(std::get<ParticleState>(universe.getParticle(1)).pos.y, state1.pos.y);

    for(; t < 70; t += dT) universe.advance(dT); // Particles have interacted
    EXPECT_NE(std::get<ParticleState>(universe.getParticle(1)).pos.y, state1.pos.y);
}

TEST(UniverseTest, Bounds) {
    Universe universe(10, 10, 1, 0);
    ParticleType type(1, 1, 0, 0, 0); // Non-interacting particle
    ParticleState state0(Vector2D(5, 5), Vector2D(1, 0));
    ParticleState state1(Vector2D(5, 5), Vector2D(0, 1));
    universe.addParticle(type, state0);
    universe.addParticle(type, state1);

    const double dT = 1e-2;
    for(double t = 0; t < 100; t += dT) universe.advance(dT); // Particles would be out of Universe if not bounded

    ASSERT_GT(std::get<ParticleState>(universe.getParticle(0)).pos.x, -5);
    ASSERT_LT(std::get<ParticleState>(universe.getParticle(0)).pos.x, 15);
    ASSERT_GT(std::get<ParticleState>(universe.getParticle(1)).pos.y, -5);
    ASSERT_LT(std::get<ParticleState>(universe.getParticle(1)).pos.y, 15);
}

TEST(UniverseTest, Gravity) {
    Universe universe(10, 10, 0, 1);
    ParticleType type(1, 1, 0, 0, 0); // Non-interacting particle
    ParticleState state(Vector2D(5, 5));
    universe.addParticle(type, state);

    const double dT = 1e-2;
    for(double t = 0; t < 100; t += dT) universe.advance(dT); // Particles would be out of Universe if not bounded

    ASSERT_GT(std::get<ParticleState>(universe.getParticle(0)).pos.y, state.pos.y);
}

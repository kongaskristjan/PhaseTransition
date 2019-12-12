
#include "Lib/Universe.h"
#include <gtest/gtest.h>

TEST(UniverseTest, IteratorTest) {
    Universe universe({ 400, 400, 1, 0 }, { ParticleType(1, 1, 1, 1, 10) });
    ParticleState state(Vector2D(200, 200)); // Standing particle
    universe.addParticle(0, state);
    universe.addParticle(0, state);
    universe.addParticle(0, state);

    size_t size = 0;
    for(auto it = universe.begin(); it != universe.end(); ++it) ++size;
    EXPECT_EQ(3, size);
    EXPECT_EQ(3, universe.size());

    universe.erase(universe.begin());
    size = 0;
    for(auto it = universe.begin(); it != universe.end(); ++it) ++size;
    EXPECT_EQ(2, size);
    EXPECT_EQ(2, universe.size());
}

TEST(UniverseTest, ParticleScatter) {
    Universe universe({ 20, 20, 1, 0 }, { ParticleType(1, 1, 1, 1, 10) });
    ParticleState state0(Vector2D(0, 10.1), Vector2D(1, 0)); // Particle to be deflected
    ParticleState state1(Vector2D(0, 10)); // Standing particle
    universe.addParticle(0, state0);
    universe.addParticle(0, state1);
    const double dT = 1e-1;
    double t = 0;

    for(; t < 1.0; t += dT) {
        universe.advance(dT); // Particles have interacted
    }
    EXPECT_NE(universe.begin()->pos.y, state0.pos.y);
}

TEST(UniverseTest, Bounds) {
    Universe universe({ 1, 1, 1, 0 }, { ParticleType(1, 1, 0, 0, 10) }); // Non-interacting particle
    ParticleState state0(Vector2D(0.5, 0.5), Vector2D(1, 0));
    ParticleState state1(Vector2D(0.5, 0.5), Vector2D(0, 1));
    universe.addParticle(0, state0);
    universe.addParticle(0, state1);
    const double dT = 2e-2;
    for(double t = 0; t < 5; t += dT) universe.advance(dT); // Particles would be out of Universe if not bounded

    ASSERT_GT((universe.begin())->pos.x, -2);
    ASSERT_LT((universe.begin())->pos.x, 3);
    ASSERT_GT((++universe.begin())->pos.y, -2);
    ASSERT_LT((++universe.begin())->pos.y, 3);
}

TEST(UniverseTest, Gravity) {
    Universe universe({ 10, 10, 0, 1 }, { ParticleType(1, 1, 0, 0, 0) });
    ParticleState state(Vector2D(5, 5));
    universe.addParticle(0, state);

    const double dT = 1e-1;
    for(double t = 0; t < 10; t += dT) universe.advance(dT); // Particles would be out of Universe if not bounded

    ASSERT_GT((universe.begin())->pos.y, state.pos.y);
}

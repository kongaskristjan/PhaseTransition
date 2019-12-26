
#include "Lib/StatsMap.h"
#include "Lib/Universe.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(StatsTest, All) {
    Universe universe({ 3, 3, 0, 0 }, { ParticleType(1, 1, 0, 0, 10) }); // Non-interacting particle
    universe.addParticle(0, ParticleState({-5, 0}, {0, 0}));
    universe.addParticle(0, ParticleState({1.5, 0}, {0, 0}));
    universe.addParticle(0, ParticleState({2.5, 0}, {1, 0}));
    universe.addParticle(0, ParticleState({5, 0}, {1, 0}));

    Image<Stats> stats1 = computeStatsImage(universe, 2, 1);
    EXPECT_EQ(stats1.get({0, 0}).n, 2);
    EXPECT_EQ(stats1.get({1, 0}).n, 3);
    EXPECT_EQ(stats1.get({2, 0}).n, 3);

    EXPECT_NEAR(stats1.get({0, 0}).temp, 0, 1e-5);
    EXPECT_GT(stats1.get({1, 0}).temp, 1e-5);

    Image<Stats> stats2 = computeStatsImage(universe, 2, 4);
    EXPECT_EQ(stats2.get({0, 0}).n, 4); // All particles in a single box
}

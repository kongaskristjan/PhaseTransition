
#include "Lib/Integrators.h"
#include <gtest/gtest.h>
#include <cmath>

// Integrate x(0) = 1, dx/dt = x. x(1) should be e

class Linear {
public:
    double derivative(double x) const {
        return x;
    }
};

TEST(IntegrableTest, Euler) {
    double x = 1;
    int nSteps = 1000;
    double dT = 1. / nSteps;

    Linear diff;
    for(int i = 0; i < nSteps; ++i) {
        advanceEuler(x, diff, dT);
    }

    EXPECT_NEAR(x, M_E, 1e-2);
}

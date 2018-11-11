
#include "Lib/Integrators.h"
#include <gtest/gtest.h>
#include <cmath>
#include <iostream>

// Integrate x(0) = 1, dx/dt = x. x(1) should be e

class Linear {
public:
    void prepareDifferentiation(double) const {
    }

    void derivative(double &der, double x) const {
        der = x;
    }
};

TEST(IntegratorTest, Exponent) {
    double xEuler = 1, xRK4 = 1;
    int nSteps = 1000;
    double dT = 1. / nSteps;

    Linear diff;
    for(int i = 0; i < nSteps; ++i) {
        advanceEuler(xEuler, diff, dT);
        advanceRungeKutta4(xRK4, diff, dT);
    }

    EXPECT_NEAR(xEuler, M_E, 1e-2);
    EXPECT_NEAR(xRK4, M_E, 1e-8);
}

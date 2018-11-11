#ifndef __INTEGRATORS_H__
#define __INTEGRATORS_H__

/*
    Required functionality for IntegrableState and Differetiator:

    class IntegrableState {
    public:
        IntegrableState operator+=(const IntegrableState &);
        IntegrableState operator*=(double);
    };

    class Differetiator {
    public:
        void prepareDifferentiation(IntegrableState &state) const; // Run every time before an integration step
        void derivative(IntegrableState &der, const IntegrableState &state) const;
    };
*/

template<typename IntegrableState, typename Differetiator>
void advanceEuler(IntegrableState &x, const Differetiator &diff, double dT);

template<typename IntegrableState, typename Differetiator>
void advanceRungeKutta4(IntegrableState &x, const Differetiator &diff, double dT);

template<typename IntegrableState, typename Differetiator>
void advanceEuler(IntegrableState &x, const Differetiator &diff, double dT) {
    // Compact form: x = x + diff.derivative(x) * dT;

    diff.prepareDifferentiation(x);
    static IntegrableState k1;
    diff.derivative(k1, x);
    k1 *= dT;
    x += k1;
}

template<typename IntegrableState, typename Differetiator>
void advanceRungeKutta4(IntegrableState &x, const Differetiator &diff, double dT) {
    /* Equations from https://en.wikipedia.org/wiki/Runge-Kutta_methods.
       Can be written in more compact form:

       IntegrableState k1 = diff.derivative(x) * dT;
       IntegrableState k2 = diff.derivative(x + k1 * 0.5) * dT;
       IntegrableState k3 = diff.derivative(x + k2 * 0.5) * dT;
       IntegrableState k4 = diff.derivative(x + k3) * dT;
       x = x + (k1 + k2 * 2 + k3 * 2 + k4) * (1. / 6.); */

    diff.prepareDifferentiation(x);

    static IntegrableState xInitial, xAdditive;
    static IntegrableState k1, k2, k3, k4;

    xInitial = x;

    // Compute k1 and add to x
    diff.derivative(k1, x);

    k1 *= dT;
    xAdditive = k1;
    xAdditive *= 1. / 6.;
    x += xAdditive;

    // Compute k2 and add to x
    k1 *= .5;
    k1 += xInitial;
    diff.derivative(k2, k1);
    k2 *= dT;
    xAdditive = k2;
    xAdditive *= 2. / 6.;
    x += xAdditive;

    // Compute k3 and add to x
    k2 *= .5;
    k2 += xInitial;
    diff.derivative(k3, k2);
    k3 *= dT;
    xAdditive = k3;
    xAdditive *= 2. / 6.;
    x += xAdditive;

    // Compute k4 and add to x
    k3 += xInitial;
    diff.derivative(k4, k3);
    k4 *= dT;
    xAdditive = k4;
    xAdditive *= 1. / 6.;
    x += xAdditive;
}

#endif

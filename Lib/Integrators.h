#ifndef __INTEGRATORS_H__
#define __INTEGRATORS_H__

/*
    Required functionality for IntegrableState and Differetiator:

    class IntegrableState {
    public:
        IntegrableState operator+(const IntegrableState &) const;
        IntegrableState operator*(double) const;
    };

    class Differetiator {
    public:
        IntegrableState derivative(const IntegrableState &state) const;
    };
*/

template<typename IntegrableState, typename Differetiator>
void advanceEuler(IntegrableState &x, const Differetiator &diff, double dT);

template<typename IntegrableState, typename Differetiator>
void advanceRungeKutta4(IntegrableState &x, const Differetiator &diff, double dT);


template<typename IntegrableState, typename Differetiator>
void advanceEuler(IntegrableState &x, const Differetiator &diff, double dT) {
    x = x + diff.derivative(x) * dT;
}

template<typename IntegrableState, typename Differetiator>
void advanceRungeKutta4(IntegrableState &x, const Differetiator &diff, double dT) {
    // Equations from https://en.wikipedia.org/wiki/Runge-Kutta_methods

    IntegrableState k1 = diff.derivative(x) * dT;
    IntegrableState k2 = diff.derivative(x + k1 * 0.5) * dT;
    IntegrableState k3 = diff.derivative(x + k2 * 0.5) * dT;
    IntegrableState k4 = diff.derivative(x + k3) * dT;
    x = x + (k1 + k2 * 2 + k3 * 2 + k4) * (1. / 6.);
}

#endif

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
void advanceEuler(IntegrableState &state, const Differetiator &differetiator, double step);


template<typename IntegrableState, typename Differetiator>
void advanceEuler(IntegrableState &x, const Differetiator &differetiator, double dT) {
    x = x + differetiator.derivative(x) * dT;
}

#endif

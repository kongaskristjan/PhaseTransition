#ifndef __UNIVERSE_H__
#define __UNIVERSE_H__

#include <vector>
#include "Lib/Particle.h"

struct UniverseState {
    std::vector<ParticleState> state;
};

UniverseState operator+(const UniverseState &lhs, const UniverseState &rhs);
UniverseState operator*(const UniverseState &lhs, double rhs);

struct UniverseDifferentiator {
    std::vector<ParticleType> particles;
    double sizeX, sizeY, forceFactor;

    UniverseDifferentiator(double sizeX, double sizeY, double forceFactor);
    void addParticle(UniverseState &state, const ParticleType &pType, const ParticleState &pState);
    UniverseState derivative(const UniverseState &state) const;

private:
    double boundForce(double overEdge) const;
};

class Universe {
public:
    Universe(double _sizeX, double _sizeY, double _forceFactor);
    void addParticle(const ParticleType &pType, const ParticleState &pState);
    void advance(double dT);

    inline size_t size() const { return state.state.size(); }
    inline std::tuple<ParticleType, ParticleState> getParticle(size_t idx) const {
        return std::make_tuple(diff.particles[idx], state.state[idx]);
    }

private:
    UniverseDifferentiator diff;
    UniverseState state;
};

#endif

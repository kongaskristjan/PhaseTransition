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
    double sizeX, sizeY, forceFactor, gravity;

    UniverseDifferentiator(double _sizeX, double _sizeY, double _forceFactor, double _gravity);
    UniverseState derivative(const UniverseState &state) const;

private:
    double boundForce(double overEdge) const;
};

class Universe {
public:
    Universe(double sizeX, double sizeY, double forceFactor, double gravity);
    void addParticle(const ParticleType &pType, const ParticleState &pState);
    void removeParticle(int index);
    void advance(double dT);
    Vector2D clampInto(const Vector2D &pos);

    inline size_t size() const { return state.state.size(); }
    inline std::tuple<const ParticleType &, const ParticleState &> getConstParticle(size_t idx) const {
        return std::make_tuple(std::cref(diff.particles[idx]), std::cref(state.state[idx]));
    }
    inline std::tuple<ParticleType &, ParticleState &> getParticle(size_t idx) {
        return std::make_tuple(std::ref(diff.particles[idx]), std::ref(state.state[idx]));
    }

private:
    UniverseDifferentiator diff;
    UniverseState state;
};

#endif

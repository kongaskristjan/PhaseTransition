#ifndef __UNIVERSE_H__
#define __UNIVERSE_H__

#include <vector>
#include "Lib/Particle.h"

struct UniverseConfig {
    int sizeX, sizeY;
    double forceFactor, gravity;
};


struct UniverseState {
    std::vector<ParticleState> state;

    UniverseState & operator+=(const UniverseState &rhs);
    UniverseState & operator*=(double rhs);
};


struct UniverseDifferentiator {
    UniverseConfig config;
    std::vector<ParticleType> types;

    UniverseDifferentiator(const UniverseConfig &config, const std::vector<ParticleType> &_types);
    void derivative(UniverseState &der, const UniverseState &state) const;

private:
    double boundForce(double overEdge) const;
};


class Universe {
public:
    Universe(const UniverseConfig &_config, const std::vector<ParticleType> &_types);
    void addParticle(int typeIndex, ParticleState pState);
    void removeParticle(int index);
    void advance(double dT);
    Vector2D clampInto(const Vector2D &pos);

    inline size_t size() const { return state.state.size(); }
    inline const ParticleType & getParticleType(size_t idx) const { return * state.state[idx].type; }
    inline ParticleState & getState(size_t idx) { return state.state[idx]; }
    inline const ParticleState & getState(size_t idx) const { return state.state[idx]; }

private:
    UniverseDifferentiator diff;
    UniverseState state;
};

#endif

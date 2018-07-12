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
    std::vector<ParticleType> particles;

    UniverseDifferentiator(const UniverseConfig &config);
    void derivative(UniverseState &der, const UniverseState &state) const;

private:
    double boundForce(double overEdge) const;
};


class Universe {
public:
    Universe(const UniverseConfig &_config);
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

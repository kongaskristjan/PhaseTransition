
#include "Lib/Integrators.h"
#include "Lib/Universe.h"
#include <vector>
#include <cassert>
#include <algorithm>

UniverseState & UniverseState::operator+=(const UniverseState &rhs) {
    assert(state.size() == rhs.state.size());
    for(size_t i = 0; i < state.size(); ++i) {
        state[i] += rhs.state[i];
    }
    return *this;
}

UniverseState & UniverseState::operator*=(double rhs) {
    for(size_t i = 0; i < state.size(); ++i) {
        state[i] *= rhs;
    }
    return *this;
}

UniverseDifferentiator::UniverseDifferentiator(const UniverseConfig &_config): config(_config) {
}

void UniverseDifferentiator::derivative(UniverseState &der, const UniverseState &state) const {
    static std::vector<Vector2D> forces;
    forces.assign(particles.size(), Vector2D());
    for(size_t i = 0; i < particles.size(); ++i) {
        for(size_t j = 0; j < i; ++j) {
            Vector2D f = particles[i].computeForce(particles[j], state.state[i], state.state[j]);
            forces[i] += f;
            forces[j] -= f;
        }

        forces[i].x += boundForce(-state.state[i].pos.x);
        forces[i].x -= boundForce(state.state[i].pos.x - config.sizeX);
        forces[i].y += boundForce(-state.state[i].pos.y);
        forces[i].y -= boundForce(state.state[i].pos.y - config.sizeY);

        forces[i].y += config.gravity * particles[i].getMass();
    }

    der.state.resize(particles.size());
    for(size_t i = 0; i < particles.size(); ++i) {
        der.state[i] = particles[i].derivative(state.state[i], forces[i]);
    }
}

double UniverseDifferentiator::boundForce(double overEdge) const {
    if(overEdge < 0) return 0;
    return config.forceFactor * overEdge * overEdge * overEdge * overEdge;
}

Universe::Universe(const UniverseConfig &_config): diff(_config) {
}

void Universe::addParticle(const ParticleType &pType, const ParticleState &pState) {
    diff.particles.push_back(pType);
    state.state.push_back(pState);
}

void Universe::removeParticle(int index) {
    diff.particles.erase(diff.particles.begin() + index);
    state.state.erase(state.state.begin() + index);
}

void Universe::advance(double dT) {
    advanceRungeKutta4(state, diff, dT);
}

Vector2D Universe::clampInto(const Vector2D &pos) {
    double newX = std::min(std::max(pos.x, 0.), (double) diff.config.sizeX);
    double newY = std::min(std::max(pos.y, 0.), (double) diff.config.sizeY);
    return Vector2D(newX, newY);
}

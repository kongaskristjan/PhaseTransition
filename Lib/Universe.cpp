
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

UniverseDifferentiator::UniverseDifferentiator(const UniverseConfig &_config, const std::vector<ParticleType> &_types):
    config(_config), types(_types) {
}

void UniverseDifferentiator::derivative(UniverseState &der, const UniverseState &state) const {
    const size_t size = state.state.size();
    static std::vector<Vector2D> forces;
    forces.assign(size, Vector2D());
    for(size_t i = 0; i < size; ++i) {
        const ParticleType &typeI = * state.state[i].type;
        for(size_t j = 0; j < i; ++j) {
            const ParticleType &typeJ = * state.state[j].type;
            Vector2D f = typeI.computeForce(typeJ, state.state[i], state.state[j]);
            forces[i] += f;
            forces[j] -= f;
        }

        forces[i].x += boundForce(-state.state[i].pos.x);
        forces[i].x -= boundForce(state.state[i].pos.x - config.sizeX);
        forces[i].y += boundForce(-state.state[i].pos.y);
        forces[i].y -= boundForce(state.state[i].pos.y - config.sizeY);

        forces[i].y += config.gravity * typeI.getMass();
    }

    der.state.resize(size);
    for(size_t i = 0; i < size; ++i) {
        const ParticleType &type = * state.state[i].type;
        type.derivative(state.state[i], forces[i]);
        der.state[i] = type.derivative(state.state[i], forces[i]);
        der.state[i].type = &type;
    }
}

double UniverseDifferentiator::boundForce(double overEdge) const {
    if(overEdge < 0) return 0;
    return config.forceFactor * overEdge * overEdge * overEdge * overEdge;
}

Universe::Universe(const UniverseConfig &_config, const std::vector<ParticleType> &_types): diff(_config, _types) {
}

void Universe::addParticle(int typeIndex, ParticleState pState) {
    pState.type = & diff.types[typeIndex];
    state.state.push_back(pState);
}

void Universe::removeParticle(int index) {
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

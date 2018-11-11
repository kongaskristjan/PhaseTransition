
#include "Lib/Integrators.h"
#include "Lib/Universe.h"
#include <vector>
#include <cassert>
#include <algorithm>

void UniverseState::setInteractionDistance(const UniverseConfig &config, double dist) {
    assert(size_ == 0);
    sizePerBlock = std::max(dist, 1.0);
    state.resize(config.sizeY / sizePerBlock + 1);
    for(auto &ySlice: state) {
        ySlice.resize(config.sizeX / sizePerBlock + 1);
    }
}

void UniverseState::prepareDifferentiation() {
    std::vector<ParticleState> current;
    for(size_t y = 0; y < state.size(); ++y) {
        for(size_t x = 0; x < state[y].size(); ++x) {
            current.swap(state[y][x]);
            state[y][x].clear();
            size_ -= current.size();
            for(size_t i = 0; i < current.size(); ++i) {
                insert(current[i]);
            }
        }
    }
}

UniverseState & UniverseState::operator=(const UniverseState &rhs) {
    state.resize(rhs.state.size());
    for(size_t y = 0; y < state.size(); ++y) {
        state[y].resize(rhs.state[y].size());
        for(size_t x = 0; x < state[y].size(); ++x) {
            state[y][x] = rhs.state[y][x];
        }
    }
    return *this;
}

UniverseState & UniverseState::operator+=(const UniverseState &rhs) {
    assert(state.size() == rhs.state.size());
    for(size_t y = 0; y < state.size(); ++y) {
        assert(state[y].size() == rhs.state[y].size());
        for(size_t x = 0; x < state[y].size(); ++x) {
            assert(state[y][x].size() == rhs.state[y][x].size());
            for(size_t i = 0; i < state[y][x].size(); ++i) {
                state[y][x][i] += rhs.state[y][x][i];
            }
        }
    }
    return *this;
}

UniverseState & UniverseState::operator*=(double rhs) {
    for(size_t y = 0; y < state.size(); ++y) {
        for(size_t x = 0; x < state[y].size(); ++x) {
            for(size_t i = 0; i < state[y][x].size(); ++i) {
                state[y][x][i] *= rhs;
            }
        }
    }
    return *this;
}


bool UniverseState::iterator::operator==(const UniverseState::iterator &rhs) const {
    return obj == rhs.obj && itY == rhs.itY && itX == rhs.itX && itI == rhs.itI;
}

bool UniverseState::iterator::operator!=(const UniverseState::iterator &rhs) const {
    return ! (*this == rhs);
}

UniverseState::iterator& UniverseState::iterator::operator++() { // prefix increment
    ++itI;
    return normalize();
}

ParticleState & UniverseState::iterator::operator*() const {
    return *itI;
}

ParticleState * UniverseState::iterator::operator->() const {
    return &*itI;
}

UniverseState::iterator & UniverseState::iterator::normalize() {
    while(itI == itX->end()) {
        ++itX;
        if(itX == itY->end()) {
            ++itY;
            if(itY == obj->state.end()) {
                return *this;
            }
            itX = itY->begin();
        }
        itI = itX->begin();
    }
    return *this;
}


UniverseState::iterator UniverseState::begin() {
    UniverseState::iterator it{ this, state.begin(), state.front().begin(), state.front().front().begin() };
    return it.normalize();
}

UniverseState::iterator UniverseState::end() {
    return { this, state.end(), state.back().end(), state.back().back().end() };
}

void UniverseState::insert(const ParticleState &pState) {
    assert(! state.empty());
    int x = std::max(0, std::min((int) state[0].size() - 1, (int) (pState.pos.x / sizePerBlock)));
    int y = std::max(0, std::min((int) state.size() - 1, (int) (pState.pos.y / sizePerBlock)));
    state[y][x].push_back(pState);
    ++size_;
}

UniverseState::iterator UniverseState::erase(UniverseState::iterator it) {
    --size_;
    it.itX->erase(it.itI);
    return it.normalize();
}


UniverseDifferentiator::UniverseDifferentiator(const UniverseConfig &_config, const std::vector<ParticleType> &_types):
    config(_config), types(_types) {
}
void UniverseDifferentiator::prepareDifferentiation(UniverseState &state) const {
    state.prepareDifferentiation();
}

void UniverseDifferentiator::derivative(UniverseState &der, UniverseState &state) const {
    initForces(der, state);
    computeForces(der, state);
    forcesToAccel(der);
}

void UniverseDifferentiator::initForces(UniverseState &der, const UniverseState &state) const {
    der.state.resize(state.state.size());
    for(size_t y = 0; y < state.state.size(); ++y) {
        der.state[y].resize(state.state[y].size());
        for(size_t x = 0; x < state.state[y].size(); ++x) {
            der.state[y][x].assign(state.state[y][x].size(), ParticleState());
            for(size_t i = 0; i < state.state[y][x].size(); ++i) {
                der.state[y][x][i].type = state.state[y][x][i].type;
                der.state[y][x][i].pos = state.state[y][x][i].v;
            }
        }
    }
}

void UniverseDifferentiator::computeForces(UniverseState &der, const UniverseState &state) const {
    for(int y0 = 0; y0 < (int) state.state.size(); ++y0)
        for(int x0 = 0; x0 < (int) state.state[y0].size(); ++x0)
            for(int i0 = 0; i0 < (int) state.state[y0][x0].size(); ++i0) {
                const auto &pState0 = state.state[y0][x0][i0];
                auto &pDer0 = der.state[y0][x0][i0];
                for(int y1 = std::max(0, y0 - 1); y1 < std::min((int) state.state.size(), y0 + 2); ++y1)
                    for(int x1 = std::max(0, x0 - 1); x1 < std::min((int) state.state[y1].size(), x0 + 2); ++x1)
                        for(int i1 = 0; i1 < (int) state.state[y1][x1].size(); ++i1) {
                            if(y0 == y1 && x0 == x1 && i0 == i1) goto break3;

                            const auto &pState1 = state.state[y1][x1][i1];
                            auto &pDer1 = der.state[y1][x1][i1];
                            Vector2D f = pState0.computeForce(pState1);
                            pDer0.v += f;
                            pDer1.v -= f;
                        }
                break3:

                pDer0.v.x += boundForce(-pState0.pos.x);
                pDer0.v.x -= boundForce(pState0.pos.x - config.sizeX);
                pDer0.v.y += boundForce(-pState0.pos.y);
                pDer0.v.y -= boundForce(pState0.pos.y - config.sizeY);
                pDer0.v.y += config.gravity * pState0.type->getMass();
            }
}

void UniverseDifferentiator::forcesToAccel(UniverseState &der) const {
    for(size_t y = 0; y < der.state.size(); ++y)
        for(size_t x = 0; x < der.state[y].size(); ++x)
            for(size_t i = 0; i < der.state[y][x].size(); ++i) {
                auto &pDer = der.state[y][x][i];
                pDer.v *= 1. / pDer.type->getMass();
            }
}

double UniverseDifferentiator::boundForce(double overEdge) const {
    if(overEdge < 0) return 0;
    return config.forceFactor * overEdge * overEdge * overEdge * overEdge;
}


Universe::Universe(const UniverseConfig &_config, const std::vector<ParticleType> &_types): diff(_config, _types) {
    double interDist = 0;
    for(const auto &type: _types) {
        interDist = std::max(interDist, type.getRange());
    }

    state.setInteractionDistance(_config, interDist);
}

void Universe::addParticle(int typeIndex, ParticleState pState) {
    pState.type = & diff.types[typeIndex];
    state.insert(pState);
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

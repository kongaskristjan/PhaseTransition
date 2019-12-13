
#include "Lib/Integrators.h"
#include "Lib/Universe.h"
#include <vector>
#include <cassert>
#include <future>

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


UniverseDifferentiator::UniverseDifferentiator(const UniverseConfig &_config, std::vector<ParticleType> _types):
    config(_config), types(std::move(_types)) {
}
void UniverseDifferentiator::prepareDifferentiation(UniverseState &state) const {
    state.prepareDifferentiation();
}

void UniverseDifferentiator::derivative(UniverseState &der, UniverseBuffers &derBuffers, UniverseState &state) const {
    // Using derivative cache as another accumulator for forces to avoid data race

    initForces(der, state);
    for (size_t i = 0; i < derBuffers.size(); ++i)
        initForces(derBuffers[i], state);

    computeForces(der, derBuffers, state);
    forcesToAccel(der, derBuffers);
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

void UniverseDifferentiator::computeForces(UniverseState &der, UniverseBuffers &derBuffers, const UniverseState &state) const {
    assert(! state.state.empty());

    AtomicCounter counter(state.state.size() * state.state[0].size());

#ifdef __EMSCRIPTEN__
    computeForcesOneThread(der, derBuffers, state, counter);
#else
    size_t nThreads = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1;
    std::vector<std::future<void>> futures;
    for(size_t i = 0; i < nThreads; ++i) {
        futures.push_back(threadPool.enqueue(& UniverseDifferentiator::computeForcesOneThread, this,
                std::ref(der), std::ref(derBuffers), std::cref(state), std::ref(counter)));
    }
    for(size_t i = 0; i < futures.size(); ++i)
        futures[i].wait();
#endif
}

void UniverseDifferentiator::computeForcesOneThread(UniverseState &der, UniverseBuffers &derBuffers,
        const UniverseState &state, AtomicCounter &counter) const {
    assert(! state.state.empty());

    struct OtherCell { UniverseState &other; int x; int y; };
    std::array<OtherCell, 5> cells = {
            OtherCell{der, 0, 0},
            OtherCell{derBuffers[0], 1, 0},
            OtherCell{derBuffers[1], -1, 1},
            OtherCell{derBuffers[2], 0, 1},
            OtherCell{derBuffers[3], 1, 1},
    };

    const int sizeX = state.state[0].size();
    for (int idx = counter.next(); idx < counter.total(); idx = counter.next()) {
        int y0 = idx / sizeX;
        int x0 = idx % sizeX;
        for (size_t i0 = 0; i0 < state.state[y0][x0].size(); ++i0) { // Compute forces by edges and gravity
            const auto &pState0 = state.state[y0][x0][i0];
            auto &pDer0 = der.state[y0][x0][i0];
            pDer0.v.x += boundForce(-pState0.pos.x);
            pDer0.v.x -= boundForce(pState0.pos.x - config.sizeX);
            pDer0.v.y += boundForce(-pState0.pos.y);
            pDer0.v.y -= boundForce(pState0.pos.y - config.sizeY);
            pDer0.v.y += config.gravity * pState0.type->getMass();
        }

        for (size_t cellIdx = 0; cellIdx < cells.size(); ++cellIdx) { // Compute interaction forces
            OtherCell &cell = cells[cellIdx];
            int y1 = y0 + cell.y;
            int x1 = x0 + cell.x;
            if (y1 < 0 || x1 < 0 || y1 >= state.state.size() || x1 >= state.state[y1].size())
                continue;

            for (size_t i0 = 0; i0 < state.state[y0][x0].size(); ++i0) {
                const auto &pState0 = state.state[y0][x0][i0];
                auto &pDer0 = der.state[y0][x0][i0];
                size_t maxI1 = cellIdx == 0 ? i0 : state.state[y1][x1].size();
                for (size_t i1 = 0; i1 < maxI1; ++i1) {
                    const auto &pState1 = state.state[y1][x1][i1];
                    auto &pDer1 = cell.other.state[y1][x1][i1];
                    Vector2D f = pState0.computeForce(pState1);
                    pDer0.v += f;
                    pDer1.v -= f;
                }
            }
        }
    }
}

double UniverseDifferentiator::boundForce(double overEdge) const {
    if(overEdge < 0) return 0;
    return config.forceFactor * overEdge * overEdge * overEdge * overEdge;
}

void UniverseDifferentiator::forcesToAccel(UniverseState &der, const UniverseBuffers &derBuffers) const {
    for(size_t y = 0; y < der.state.size(); ++y)
        for(size_t x = 0; x < der.state[y].size(); ++x)
            for(size_t i = 0; i < der.state[y][x].size(); ++i) {
                auto &pDer = der.state[y][x][i];
                for(const UniverseState &buffer: derBuffers) {
                    auto &pDerCache = buffer.state[y][x][i];
                    pDer.v += pDerCache.v;
                }
                pDer.v *= 1. / pDer.type->getMass();
            }
}

Universe::Universe(const UniverseConfig &_config, const std::vector<ParticleType> &_types):
        diff(_config, _types) {
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
    advanceRungeKutta4<UniverseState, UniverseDifferentiator, UniverseBuffers>(state, diff, dT);
}

Vector2D Universe::clampInto(const Vector2D &pos) {
    double newX = std::min(std::max(pos.x, 0.), (double) diff.config.sizeX);
    double newY = std::min(std::max(pos.y, 0.), (double) diff.config.sizeY);
    return Vector2D(newX, newY);
}

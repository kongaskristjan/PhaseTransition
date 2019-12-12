#ifndef __UNIVERSE_H__
#define __UNIVERSE_H__

#include <array>
#include <vector>
#include "Lib/Particle.h"
#include "Lib/AtomicCounter.h"
#include "Lib/ThreadPool.h"

/*
 * Universe handles the creation and destruction of particles and provides methods for iterating over them.
 * Universe::advance() advances the current state with the help of an integrator and UniverseState/-Differentiator.
 *
 * UniverseState holds the position/velocity information of all the particles in the universe.
 * UniverseDifferentiator has a method that returns the derivative of UniverseState, which is also a
 * UniverseState. Internally, UniverseState and -Differentiator hold their particles as put into a grid of
 * boxes. This allows checking interactions only between particles at nearby boxes, thus speeding up computations.
 *
 * Each box of the UniverseState is processed single-threadedly, and parallelization is achieved by
 * concurrently processing several boxes. Also, in order to save time, it is appropriate to compute
 * each interaction only once. These requirements however create a race condition, because thread 1 writing
 * to box 1 must also write to box 2, which thread 2 might be writing to at the same time. This problem
 * is eliminated by adding a few more accumulation buffers (UniverseBuffers), each of which can only be
 * written from a box at a pose relative to destination box. See UniverseDifferentiator::computeForcesOneThread
 * for details (Relative poses are set by std::array<OtherCell>).
 */

class UniverseState;
typedef std::array<UniverseState, 4> UniverseBuffers;

struct UniverseConfig {
    int sizeX, sizeY;
    double forceFactor, gravity;
};

struct UniverseState {
    std::vector<std::vector<std::vector<ParticleState>>> state;
    size_t size_ = 0;
    double sizePerBlock = 1;

    void setInteractionDistance(const UniverseConfig &config, double dist);
    void prepareDifferentiation();
    UniverseState & operator=(const UniverseState &rhs);
    UniverseState & operator+=(const UniverseState &rhs);
    UniverseState & operator*=(double rhs);
    size_t size() const { return size_; }

    class iterator {
    public:
        bool operator==(const iterator &rhs) const;
        bool operator!=(const iterator &rhs) const;
        iterator & operator++(); // prefix increment
        ParticleState & operator*() const;
        ParticleState * operator->() const;
        iterator & normalize();
    
        UniverseState *obj = nullptr;
        std::vector<std::vector<std::vector<ParticleState>>>::iterator itY;
        std::vector<std::vector<ParticleState>>::iterator itX;
        std::vector<ParticleState>::iterator itI;
    };

    iterator begin();
    iterator end();

    void insert(const ParticleState &state);
    iterator erase(iterator it);
};

struct UniverseDifferentiator {
    UniverseConfig config;
    std::vector<ParticleType> types;

    UniverseDifferentiator(const UniverseConfig &config, std::vector<ParticleType> _types);
    void prepareDifferentiation(UniverseState &state) const; // Has to be called once before every iteration
    void derivative(UniverseState &der, UniverseBuffers &derBuffers, UniverseState &state) const;

private:
    void initForces(UniverseState &der, const UniverseState &state) const;
    void computeForces(UniverseState &der, UniverseBuffers &derBuffers, const UniverseState &state) const;
    void computeForcesOneThread(UniverseState &der, UniverseBuffers &derBuffers, const UniverseState &state,
            AtomicCounter &counter) const;
    double boundForce(double overEdge) const;

    void forcesToAccel(UniverseState &der, const UniverseBuffers &derBuffers) const;
};


class Universe {
public:
    Universe(const UniverseConfig &_config, const std::vector<ParticleType> &_types);
    void addParticle(int typeIndex, ParticleState pState);
    void removeParticle(int index);
    void advance(double dT);
    Vector2D clampInto(const Vector2D &pos);

    inline size_t size() const { return state.size(); }
    inline const UniverseConfig & getConfig() const { return diff.config; }
    inline const std::vector<ParticleType> & getParticleTypes() const { return diff.types; }

    inline auto begin() { return state.begin(); }
    inline auto end() { return state.end(); }
    inline auto erase(const UniverseState::iterator &it) { return state.erase(it); }
private:
    UniverseDifferentiator diff;
    UniverseState state;
};

#endif

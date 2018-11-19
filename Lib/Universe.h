#ifndef __UNIVERSE_H__
#define __UNIVERSE_H__

#include <vector>
#include "Lib/Particle.h"

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

    UniverseDifferentiator(const UniverseConfig &config, const std::vector<ParticleType> &_types);
    void prepareDifferentiation(UniverseState &state) const; // Has to be called once before every iteration
    void derivative(UniverseState &der, UniverseState &state) const;

private:
    void initForces(UniverseState &der, const UniverseState &state) const;
    void computeForces(UniverseState &der, const UniverseState &state) const;
    void forcesToAccel(UniverseState &der) const;

    double boundForce(double overEdge) const;

public:
	class ParallelForces : public cv::ParallelLoopBody {
	public:
		ParallelForces(const UniverseDifferentiator &diff, UniverseState &der, const UniverseState &state);
		void operator()(const cv::Range& range) const;
		ParallelForces& operator=(const ParallelForces &);

	private:
        const UniverseDifferentiator &diff;
        UniverseState &der;
        const UniverseState &state;
	};
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

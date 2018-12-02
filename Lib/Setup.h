#ifndef __SETUP_H__
#define __SETUP_H__

#include <string>
#include <vector>
#include "Lib/Particle.h"
#include "Lib/Universe.h"

struct ParticleSetup {
    int type;
    Vector2D pos, v;
};

struct Setup {
    std::string recordingPrefix;
    std::string displayedCaption;
    std::vector<ParticleType> particleTypes;
    std::vector<ParticleSetup> particles;
    int sizeX = 0, sizeY = 0;
    double gravity = 0;
    double forceFactor = 1e-2;
    double dT = 0.5;

    Setup(std::string filePath);
    inline Setup() {
    }

    void addParticlesToUniverse(Universe &universe) const;
};

#endif

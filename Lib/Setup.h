#ifndef __SETUP_H__
#define __SETUP_H__

#include <string>
#include <vector>
#include "Lib/Particle.h"

struct Setup {
    std::string recordingPrefix;
    std::string displayedCaption;
    std::vector<ParticleType> particleTypes;
    int sizeX = 0, sizeY = 0;
    double gravity = 0;
    double forceFactor = 1e-2;
    double dT = 0.5;

    Setup(std::string filePath);
    inline Setup() {
    }
};

#endif

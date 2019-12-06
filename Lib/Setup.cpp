
#include "Lib/Particle.h"
#include "Lib/Setup.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <cassert>

Setup::Setup(std::string filePath) {
    size_t lastBackslash = filePath.rfind('/');
    std::string directoryPath = lastBackslash == std::string::npos ? "" : filePath.substr(0, lastBackslash + 1);

    std::ifstream fin(filePath);
    while(fin.good()) {
        std::string key;
        fin >> key;

        if(key == "recordingPrefix") {
            fin >> recordingPrefix;
            recordingPrefix = directoryPath + recordingPrefix;
        }
        if(key == "displayedCaption") {
            fin >> displayedCaption;
            std::replace(displayedCaption.begin(), displayedCaption.end(), '_', ' ');
        }
        if(key == "particleType") {
            double mass, radius, exclusionConstant, dipoleMoment, range;
            fin >> mass >> radius >> exclusionConstant >> dipoleMoment >> range;
            std::string name, spriteLocation;
            fin >> name >> spriteLocation;
            std::replace(name.begin(), name.end(), '_', ' ');

            particleTypes.emplace_back(name, directoryPath + spriteLocation, mass, radius, exclusionConstant, dipoleMoment, range);
        }
        if(key == "particle") {
            ParticleSetup p;
            fin >> p.pos.x >> p.pos.y >> p.v.x >> p.v.y >> p.type;
            particles.push_back(p);
        }
        if(key == "sizeX") fin >> sizeX;
        if(key == "sizeY") fin >> sizeY;
        if(key == "gravity") fin >> gravity;
        if(key == "forceFactor") fin >> forceFactor;
        if(key == "dT") fin >> dT;
    }

    assert(particleTypes.size() > 0);
    assert(sizeX > 0 && sizeY > 0);
}


void Setup::addParticlesToUniverse(Universe &universe) const {
    for(const ParticleSetup &p: particles)
        universe.addParticle(p.type, ParticleState(p.pos, p.v));
}

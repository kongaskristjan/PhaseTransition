
#include <iostream>
#include "StatsMap.h"

StatsMap::StatsMap(Universe &universe, int _minParticlesInRange, double _squareLength):
        minParticlesInRange(_minParticlesInRange), squareLength(_squareLength) {
    int sizeX = (int) (universe.getConfig().sizeX / squareLength + 1);
    int sizeY = (int) (universe.getConfig().sizeY / squareLength + 1);
    imageSize = {sizeX, sizeY};

    Image<int> nParticles(sizeX, sizeY);
    Image<double> mass(sizeX, sizeY), energy(sizeX, sizeY);
    Image<Vector2D> pxy(sizeX, sizeY);
    for(const ParticleState &state: universe) {
        double pMass = state.type->getMass();
        Vector2I coords = getCoords(state.pos);
        nParticles.get(coords) += 1;
        mass.get(coords) += pMass;
        energy.get(coords) += state.v.magnitude2() * (pMass / 2);
        pxy.get(coords) += state.v * pMass;
    }

    intNParticles = IntegralImage<int>(nParticles);
    intMass = IntegralImage<double>(mass);
    intEnergy = IntegralImage<double>(energy);
    intPxy = IntegralImage<Vector2D>(pxy);
}

Stats StatsMap::evaluate(Vector2I pos) const {
    // Find optimal area for averaging using binary search
    int range;
    for(range = 1; range <= 1024 && intNParticles.getInRange(pos, range - 1) < minParticlesInRange; range *= 2) {}
    for(int deltaRange = range / 2; deltaRange > 0; deltaRange /= 2)
        if (intNParticles.getInRange(pos, range - deltaRange - 1) >= minParticlesInRange)
            range -= deltaRange;
    range -= 1;

    // Compute stats
    Stats stats;
    stats.range = range;
    stats.n = intNParticles.getInRange(pos, range);

    double mass = intMass.getInRange(pos, range);
    stats.density = mass / (squareLength * (2 * range + 1) * squareLength * (2 * range + 1));

    // Computing temperature:
    // ((degrees of freedom) / 2) * nkT = kinetic heat energy = total kinetic energy - directed kinetic energy
    // Note that ((degrees of freedom) / 2) * nkT = nT (as (degrees of freedom) = 2, k = 1)
    double totalKineticEnergy = intEnergy.getInRange(pos, range);
    double directedKineticEnergy = intPxy.getInRange(pos, range).magnitude2() / mass;
    stats.temp = (totalKineticEnergy - directedKineticEnergy) / (stats.n + 1e-6);

    return stats;
}

Vector2I StatsMap::getCoords(const Vector2D &pos) {
    int cX = std::max(0, std::min(imageSize.x - 1, (int) (pos.x / squareLength)));
    int cY = std::max(0, std::min(imageSize.y - 1, (int) (pos.y / squareLength)));
    Vector2I coords(cX, cY);
    return coords;
}

Image<Stats> computeStatsImage(Universe &universe, int minParticlesInRange, double squareLength) {
    StatsMap statsMap(universe, minParticlesInRange, squareLength);
    Image<Stats> stats(statsMap.sizeX(), statsMap.sizeY());

    for(int y = 0; y < stats.sizeY(); ++y)
        for(int x = 0; x < stats.sizeX(); ++x)
            stats.get({x, y}) = statsMap.evaluate({x, y});

    return stats;
}


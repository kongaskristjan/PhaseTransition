#ifndef __STATS_MAP_H__
#define __STATS_MAP_H__

#include "Vector2.h"
#include "Universe.h"
#include "IntegralImage.h"

struct Stats {
    int range;
    int n;
    double temp;
    double density;
};

class StatsMap {
public:
    StatsMap(Universe &universe, int _minParticlesInRange, double _squareLength);
    Stats evaluate(Vector2I pos) const;
    inline int sizeX() const { return intNParticles.sizeX(); }
    inline int sizeY() const { return intNParticles.sizeY(); }

private:
    Vector2I getCoords(const Vector2D &position);

    Vector2I imageSize; // Initialized immediately
    IntegralImage<int> intNParticles;
    IntegralImage<double> intMass, intEnergy;
    IntegralImage<Vector2D> intPxy;
    double squareLength;
    int minParticlesInRange;
};

Image<Stats> computeStatsImage(Universe &universe, int minParticlesInRange, double squareLength);

#endif

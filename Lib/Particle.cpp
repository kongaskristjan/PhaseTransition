
#include "Lib/Particle.h"
#include <cmath>
#include <SDL2/SDL_image.h>


ParticleState::ParticleState(): pos(Vector2D(0, 0)), v(Vector2D(0, 0)) {}
ParticleState::ParticleState(const Vector2D &_pos): pos(_pos), v(Vector2D(0, 0)) {}
ParticleState::ParticleState(const Vector2D &_pos, const Vector2D &_v): pos(_pos), v(_v) {}
ParticleState & ParticleState::operator+=(const ParticleState & rhs) { pos += rhs.pos; v += rhs.v; return *this; }
ParticleState & ParticleState::operator*=(double rhs) { pos *= rhs; v *= rhs; return *this; }
ParticleState operator+(const ParticleState & lhs, const ParticleState & rhs) { return ParticleState(lhs.pos + rhs.pos, lhs.v + rhs.v); }
ParticleState operator*(const ParticleState & lhs, double rhs) { return ParticleState(lhs.pos * rhs, lhs.v * rhs); }
Vector2D ParticleState::computeForce(const ParticleState &rhs) const { return type->computeForce(* rhs.type, *this, rhs); }


ParticleType::ParticleType(double _mass, double _radius, double _exclusionConstant, double _dipoleMoment, double _range):
    ParticleType("", "", _mass, _radius, _exclusionConstant, _dipoleMoment, _range) {
}

ParticleType::ParticleType(const std::string &_name, const std::string &spritePath,
        double _mass, double _radius, double _exclusionConstant, double _dipoleMoment, double _range) {
    name = _name;
    if(! spritePath.empty()) spriteSurface = SDL_LoadBMP(spritePath.c_str());
    mass = _mass;
    radius = _radius;
    exclusionConstant = _exclusionConstant;
    dipoleMoment = _dipoleMoment;
    range = _range;
}

Vector2D ParticleType::computeForce(const ParticleType &other, const ParticleState &myState, const ParticleState &otherState) const {
    const double totalRadius = radius + other.radius;
    const double totalExclusionFactor = exclusionConstant * other.exclusionConstant;
    const double totalDipoleFactor = dipoleMoment * other.dipoleMoment;
    const double minRange = std::min(range, other.range);

    Vector2D dVec = myState.pos - otherState.pos;
    double d = dVec.magnitude();
    double forceFactor = computeForceFactor(totalRadius, minRange, d);
    Vector2D direction = dVec.norm();
    if(forceFactor == 0 || dVec.magnitude() < 1e-6) {
        return Vector2D(0, 0);
    }

    double dNorm = d / totalRadius;
    double exclusionForce = totalExclusionFactor * computeForceComponent(dNorm);
    double dipoleForce = -totalDipoleFactor * computeForceComponent(0.5 * dNorm);
    double totalForce = exclusionForce + dipoleForce;
    double cutoffForce = totalForce * forceFactor;

    return direction * cutoffForce;
}

double ParticleType::computeForceComponent(double d) const {
    double d2 = d * d;
    double d4 = d2 * d2;
    double d8 = d4 * d4;
    double d16 = d8 * d8;
    return exp(-d16);
}

double ParticleType::computeForceFactor(double totalRadius, double minRange, double d) const {
    return superSmoothZeroToOne((minRange - d) / totalRadius);
}

// Super smooth function with f(x <= 0) == 0, f(x >= 1) == 1
double ParticleType::superSmoothZeroToOne(double x) const {
    const double cutoff = 5e-2;
    if(x < cutoff) return 0;
    if(x > 1 - cutoff) return 1;

    double factor0 = exp(1 / -x);
    double factor1 = exp(1 / (x - 1));
    double weightedSum = factor1 / (factor0 + factor1);
    return weightedSum;
}

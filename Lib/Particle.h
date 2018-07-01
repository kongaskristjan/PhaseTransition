#ifndef __PARTICLE_TYPE_H__
#define __PARTICLE_TYPE_H__

#include "Lib/Vector2.h"
#include <opencv2/core.hpp>

struct ParticleState {
    Vector2D pos, v;

    ParticleState();
    ParticleState(const Vector2D &_pos);
    ParticleState(const Vector2D &_pos, const Vector2D &_v);
};

ParticleState operator+(const ParticleState & lhs, const ParticleState & rhs);
ParticleState operator*(const ParticleState & lhs, double rhs);

class ParticleType {
public:
    ParticleType(double _mass, double _radius, double _exclusionConstant, double _dipoleMoment, double _range,
        const cv::Scalar &_color = cv::Scalar(255, 255, 255));
    ParticleState derivative(const ParticleState &state, const Vector2D &force) const;
    Vector2D computeForce(const ParticleType &other, const ParticleState &myState, const ParticleState &otherState) const;

    inline double getRange() const { return range; }
    inline double getMass() const { return mass; }
    inline double getRadius() const { return radius; }
    inline const cv::Scalar& getColor() const { return color; }

private:
    double computeForceComponent(double d) const;
    double computeForceFactor(double totalRadius, double minRange, double dist) const;
    double superSmoothZeroToOne(double x) const;

    double mass, radius, exclusionConstant, dipoleMoment, range;
    cv::Scalar color;
};

#endif

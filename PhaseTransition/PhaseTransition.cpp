
#include <iostream>
#include <random>
#include "Lib/Display.h"
#include "Lib/Universe.h"
#include "Lib/Particle.h"

ParticleType getParticleType();

int main() {
	int sizeX = 1920, sizeY = 1080;
	const double dT = 0.5;
	double gravity = 1e-2;
	Universe universe({ sizeX, sizeY, 1e-2, gravity });
	ParticleType type = getParticleType();
	Display display(sizeX, sizeY, "Phase Transition");

	for(int i = 0; i < 200000; ++i) {
		const CallbackHandler &handler = display.update(universe, 1. / 60);
		UniverseModifier::modify(universe, handler, dT, type);

		for(int j = 0; j < 5; ++j) {
			universe.advance(dT / 5);
		}
	}

	return 0;
}

ParticleType getParticleType() {
	double mass = 1.0;
	double radius = 4.0;

	double forceFactor = 1.0;
	double exclusionConstant = 2 * forceFactor;
	double dipoleMoment = 0.8 * forceFactor;
	double range = 5 * radius;

	return ParticleType(mass, radius, exclusionConstant, dipoleMoment, range);
}

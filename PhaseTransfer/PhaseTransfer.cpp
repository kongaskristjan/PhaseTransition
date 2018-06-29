
#include <iostream>
#include <random>
#include "Lib/Display.h"
#include "Lib/Universe.h"
#include "Lib/Particle.h"

ParticleType getParticleType();
void addParticles(Universe &universe, const ParticleType &type, size_t sizeX, size_t sizeY, size_t nParticles);

int main() {
	size_t sizeX = 1920, sizeY = 1080;
	Universe universe(sizeX, sizeY, 1e-2);
	ParticleType type = getParticleType();
	addParticles(universe, type, sizeX, sizeY, 500);
	Display display(sizeX, sizeY, "Phase Transfer");
	for(int i = 0; i < 200000; ++i) {
		display.update(universe, 1. / 60);
		universe.advance(1.0);
	}

	return 0;
}

ParticleType getParticleType() {
	double mass = 1.0;
	double radius = 4.0;
	double exclusionConstant = 5.0;
	double dipoleMoment = 10.0;
	double range = 50.0;

	return ParticleType(mass, radius, exclusionConstant, dipoleMoment, range);
}

void addParticles(Universe &universe, const ParticleType &type, size_t sizeX, size_t sizeY, size_t nParticles) {
	auto gen = std::mt19937(1);
	auto xUniform = std::uniform_real_distribution<double>(0., sizeX);
	auto yUniform = std::uniform_real_distribution<double>(0., sizeY);
	for(size_t i = 0; i < nParticles; ++i) {
		auto state = ParticleState(Vector2D(xUniform(gen), yUniform(gen)));
		universe.addParticle(type, state);
	}
}

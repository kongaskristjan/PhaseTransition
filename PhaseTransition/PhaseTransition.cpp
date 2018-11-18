
#include <iostream>
#include <random>
#include <ctime>
#include "Lib/Display.h"
#include "Lib/Universe.h"
#include "Lib/Particle.h"

ParticleType getParticleType();
const std::string currentDateTime();

int main(int argc, char **argv) {
	std::string recordingPath;
	if(argc > 1) {
		recordingPath = "recording/" + currentDateTime() + ".avi";
		std::cout << "Recording..." << std::endl;
	}

	const int sizeX = 1920, sizeY = 1080;
	const double dT = 0.5;
	const double gravity = 1e-2;
	Universe universe({ sizeX, sizeY, 1e-2, gravity }, { getParticleType() });
	Display display(sizeX, sizeY, "Phase Transition", recordingPath);

	while(true) {
		const CallbackHandler &handler = display.update(universe);
		UniverseModifier::modify(universe, handler, dT, 0);

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

const std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

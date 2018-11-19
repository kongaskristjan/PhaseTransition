
#include <iostream>
#include <random>
#include <ctime>
#include <cmath>
#include "Lib/Display.h"
#include "Lib/Universe.h"
#include "Lib/Particle.h"

std::vector<ParticleType> createAllParticleTypes();
ParticleType createParticleType(std::string name, cv::Scalar color,
	double massFactor, double lengthFactor, double timeFactor, double dipoleFactor);
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
	Universe universe({ sizeX, sizeY, 1e-2, gravity }, createAllParticleTypes());
	Display display(universe, "Phase Transition", recordingPath);

	while(true) {
		const CallbackHandler &handler = display.update();
		UniverseModifier::modify(universe, handler, dT);

		for(int j = 0; j < 5; ++j) {
			universe.advance(dT / 5);
		}
	}

	return 0;
}

std::vector<ParticleType> createAllParticleTypes() {
	std::vector<ParticleType> types;
	types.push_back(createParticleType("small", cv::Scalar(255, 255, 255), 1, 1.0, 1, 0.8));
	types.push_back(createParticleType("large", cv::Scalar(255, 255, 0), 1.0, 1.4, 1, 0.8));
	types.push_back(createParticleType("heavy inert", cv::Scalar(0, 255, 255), 4.0, 1.4, 1, 0.0));
	return types;
}


ParticleType createParticleType(std::string name, cv::Scalar color, double massFactor, double lengthFactor, double timeFactor, double dipoleFactor) {
	// All constants are multiplied by mass-, length- and timeFactors according to dimensional analysis
	double mass = massFactor;
	double radius = 4.0 * lengthFactor;

	double forceFactor = massFactor * lengthFactor / (timeFactor * timeFactor);
	double exclusionConstant = 2 * forceFactor;
	double dipoleMoment = dipoleFactor * forceFactor;
	double range = 5 * radius;

	return ParticleType(name, color, mass, radius, exclusionConstant, dipoleMoment, range);
}

ParticleType createLargeParticleType() {
	double mass = 1.0;
	double radius = 8.0;

	double forceFactor = 2.0;
	double exclusionConstant = 2 * forceFactor;
	double dipoleMoment = 0.8 * forceFactor;
	double range = 5 * radius;

	return ParticleType("large", cv::Scalar(255, 255, 255), mass, radius, exclusionConstant, dipoleMoment, range);
}

const std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

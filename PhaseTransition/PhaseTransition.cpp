
#include <iostream>
#include <random>
#include <ctime>
#include <cmath>
#include "Lib/Setup.h"
#include "Lib/Display.h"
#include "Lib/Universe.h"
#include "Lib/Particle.h"

std::string currentDateTime();

int main(int argc, char **argv) {
	assert(argc == 2 && "Expected setup file as argument");
	Setup setup(argv[1]);

	std::string recordingPath;
	if(setup.recordingPrefix != "") recordingPath = setup.recordingPrefix + currentDateTime() + ".avi";
	Universe universe({ setup.sizeX, setup.sizeY, setup.forceFactor, setup.gravity }, setup.particleTypes);
	setup.addParticlesToUniverse(universe);
	Display display(universe, "Phase Transition", setup.displayedCaption, recordingPath);

	while(true) {
		const CallbackHandler &handler = display.update();
		UniverseModifier::modify(universe, handler, setup.dT);

		for(int j = 0; j < 5; ++j) {
			universe.advance(setup.dT / 5);
		}
	}

	return 0;
}

std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

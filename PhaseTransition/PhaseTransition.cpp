
#include <iostream>
#include <random>
#include <ctime>
#include <cmath>
#include <cassert>
#include "Lib/Setup.h"
#include "Lib/Display.h"
#include "Lib/Universe.h"
#include "Lib/Particle.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


std::unique_ptr<Setup> globalSetup = nullptr;
std::unique_ptr<Universe> globalUniverse = nullptr;
std::unique_ptr<Display> globalDisplay = nullptr;
bool exitFlag = false;


void oneStep();
std::string currentDateTime();

int main(int argc, char **argv) {
#ifdef __EMSCRIPTEN__
    globalSetup.reset(new Setup("/PhaseTransition/web.txt"));
#else
    assert(argc == 2 && "Expected setup file as argument");
	globalSetup.reset(new Setup(argv[1]));
#endif

	std::string recordingPath;
	if(! globalSetup->recordingPrefix.empty()) recordingPath = globalSetup->recordingPrefix + currentDateTime() + ".avi";
	globalUniverse.reset(new Universe({ globalSetup->sizeX, globalSetup->sizeY, globalSetup->forceFactor, globalSetup->gravity },
	        globalSetup->particleTypes));
	globalSetup->addParticlesToUniverse(*globalUniverse);
	globalDisplay.reset(new Display(*globalUniverse, "Phase Transition",
	        globalSetup->displayedCaption, globalSetup->directoryPath, recordingPath));

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(oneStep, 60, 1);
#else
	while(! exitFlag) {
	    oneStep();
    }
#endif

	return 0;
}

void oneStep() {
    const CallbackHandler &handler = globalDisplay->update();
    UniverseModifier::modify(*globalUniverse, handler, globalSetup->dT);

    if (handler.quit) {
        exitFlag = true;
        return;
    }

    for (int j = 0; j < 5; ++j)
        globalUniverse->advance(globalSetup->dT / 5);
}

std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

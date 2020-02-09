# Phase Transition

[Try it out in your browser](https://kongaskristjan.github.io/2020/02/09/phase-transition.html) or watch this video:

[![Video of the simulation](https://img.youtube.com/vi/SFf3pcE08NM/0.jpg)](https://youtu.be/SFf3pcE08NM)

### Build

Requirements:

* C++14 compatible compiler
* cmake 3.10+
* pthreads
* SDL 2 and SDL_ttf 2
* SDL_image 2 (optional - recording video)
* gtest (optional - running tests)
* emscripten (optional - building for web)

Building:

* Create and change working directory to build directory: `mkdir build && cd build`
* Build, run tests and program on Linux: `cmake -D CMAKE_BUILD_TYPE=Release .. && make && ./RunTests && ./PhaseTransition ../Setups/default.txt`,
where `./RunTests` is optional.
* Build for web using emscripten: `emconfigure cmake -D CMAKE_BUILD_TYPE=Release .. && emmake make`. This should generate PhaseTransition html, js and wasm files. You probably need a web server to actually run this in your browser: `python3 -m http.server 8080` (still from the build directory). Then go to <http://localhost:8080/PhaseTransition.html>. Currently the web build is slow because it's single-threaded.

Default simulation resolution, particle properties, etc. can be modified in Setups/default.txt. For web build, modify Setups/web.txt and force a rebuild by removing all files in the build directory.

### Usage

Use your mouse to create and influence the particles. There are four modes of interaction: creating, spraying, pushing and heating, each of which can be activated with keys c, s, p and h respectively.
Once in a mode, this action can be carried out on particles by holding the left mouse button.
Right mouse button does the opposite of the activated mode's function. Range of influence can be altered with mouse wheel.

The number of particles, average velocity, and average temperature (inside the range of influence) are displayed in the upper left corner of display.

### Acknowledgements

This project contains
* ThreadPool library by Jakob Progsch and Václav Zeman.
* DroidSans fonts by Google


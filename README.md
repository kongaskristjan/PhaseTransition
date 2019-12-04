# Phase Transition

Check out the following video for demo (a slightly more detailed descripion is in video description):
[![Video of the simulation](https://img.youtube.com/vi/SFf3pcE08NM/0.jpg)](https://youtu.be/SFf3pcE08NM)

### Build

The following is necessary for building and running the simulation:

* C++14 compatible compiler
* cmake 3.10+ - for building
* OpenCV 3.4.x - for rendering
* gtest - for testing. Can be skipped.

Building, running tests and running program on Linux: from root directory, run `mkdir build && cd build` and
`cmake -DCMAKE_BUILD_TYPE=Release .. && make && ./RunTests && ./PhaseTransition ../Setups/default.txt`,
where `./RunTests` is optional.

Simulation resolution, particle properties, etc. can be modified in Setups/default.txt.

### Usage

Use your mouse to create and influence the particles. There are four modes of interaction: creating, spraying, pushing and heating, each of which can be activated with keys c, s, p and h respectively. Once in a mode, this action can be carried out on particles by holding the left mouse button. Right mouse button does the opposite. Range of influence can be altered with mouse wheel.

The number of particles, average veloity of them, and average temperature (inside the range of influence) are displayed in the upper left corner of display.

### Issues

* Opencv mouse control (especially right button and wheel) does not work with some window managers
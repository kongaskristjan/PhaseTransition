# Phase Transition

Check out the following video for demo (a slightly more detailed descripion is in video description):
[![Video of simulation](https://img.youtube.com/vi/SvmOGQdZ5BU/0.jpg)](https://youtu.be/SvmOGQdZ5BU)

### Build and run

The following is necessary for building and running the simulation:

* C++17 compatible compiler (g++-8 is good enough)
* Bazel - for building
* OpenCV 3.4.x - for rendering
* gtest - for testing. Can be skipped.

Run ./RunOptimized.sh for optimized bazel build and running. If testing is not intended, copy the build and running command from RunOptimized.

Simulation resolution, particle properties, etc. can be modified in Setups/default.txt.

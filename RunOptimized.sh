#!/bin/bash

bazel test --compilation_mode=opt //... && bazel build --compilation_mode=opt //... && bazel-bin/PhaseTransition/PhaseTransition

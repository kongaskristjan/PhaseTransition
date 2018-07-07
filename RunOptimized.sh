#!/bin/bash

bazel test --compilation_mode=dbg //... && bazel build --compilation_mode=opt //... && bazel-bin/PhaseTransition/PhaseTransition

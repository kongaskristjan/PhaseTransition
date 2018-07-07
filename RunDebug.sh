#!/bin/bash

bazel test --compilation_mode=dbg //... && bazel build --compilation_mode=dbg //... && gdb bazel-bin/PhaseTransition/PhaseTransition

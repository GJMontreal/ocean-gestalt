#!/bin/sh
set -e

git submodule update --init --recursive
patch -p1 -N -r - < patches/glew-cmake-darwin25.patch -d lib/glew || true

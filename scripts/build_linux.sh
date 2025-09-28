#!/bin/bash

set -e
PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE="Release"

echo "Build successful."
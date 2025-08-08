#!/bin/bash

set -e
PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE="Release"

if [[ "$1" == "-p" || "$1" == "--package" ]]; then
    echo "Packaging enabled. Running installer script..."
    ./scripts/debian_package.sh
else 
    echo "You can automatically make the package for your os by running this script with -p or --package flag."
    echo "You can also just run the package script directly now."
fi

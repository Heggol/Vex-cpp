#!/bin/bash

set -e
set -x

PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE="Release"

cmake --build . --config Release

mkdir -p Release
if [ -f vex ]; then
    mv -f vex Release/
elif ls vex_* 1> /dev/null 2>&1; then
    mv -f vex_* Release/
fi

if [ -f tests ]; then
    mv -f tests Release/
elif ls tests_* 1> /dev/null 2>&1; then
    mv -f tests_* Release/
fi

if [[ "$1" == "-p" || "$1" == "--package" ]]; then
    echo "Packaging enabled. Running installer script..."
    if [ -f /etc/debian_version ]; then
        ./scripts/debian_package.sh
    elif [ -f /etc/arch-release ]; then
        ./scripts/arch_package.sh
    else
        echo "Unsupported Linux distribution for packaging.
        exit 1
    fi
else 
    echo "You can automatically make the package for your os by running this script with -p or --package flag."
    echo "You can also just run the package script directly now."
fi

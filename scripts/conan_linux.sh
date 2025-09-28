#!/bin/bash

set -e
PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"

if ! command -v conan &> /dev/null; then
    echo "Conan is not installed. Please install Conan before proceeding."
    echo "You may also install it using pip if installed by running this script with the --pip flag."
    exit 1
fi

if [[ "$1" == "--pip" ]]; then
    echo "Installing Conan via pip..."
    pip install conan
fi

conan profile detect --force
conan install . --output-folder=build --build=missing

echo "Conan dependencies installed successfully. You can now build with CMake using the build script 'scripts/build_linux.sh'."
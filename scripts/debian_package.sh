#!/bin/bash

set -e
set -x

PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"

APP_NAME="vex"
VERSION=$(git describe --tags --abbrev=0 2>/dev/null | sed 's/^v//' || echo "0.1.0")
ARCH=$(dpkg --print-architecture)

BUILD_DIR="build/release"
PKG_DIR="${APP_NAME}_${VERSION}_${ARCH}"

rm -rf "$BUILD_DIR" "$PKG_DIR"
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"
conan install ../.. --build=missing
cmake ../.. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE="Release"
cmake --build . --config Release
cd "$PROJECT_ROOT"

mkdir -p "$PKG_DIR/DEBIAN"
mkdir -p "$PKG_DIR/usr/bin"

install -Dm755 "$BUILD_DIR/vex" "$PKG_DIR/usr/bin/vex"

cat <<EOF > "$PKG_DIR/DEBIAN/control"
Package: $APP_NAME
Version: $VERSION
Architecture: $ARCH
Maintainer: Heggo <heggo@heggo.dev>
Description: A C++ command-line tool for package management.
Section: utils
Priority: optional
# TODO: Add runtime dependencies here. E.g. Depends: libssl1.1, libarchive13
Depends:
EOF

dpkg-deb --build "$PKG_DIR"

rm -rf "$BUILD_DIR"

echo "Debian package created: ${PKG_DIR}.deb"

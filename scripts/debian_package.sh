#!/bin/bash

set -e

PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"

APP_NAME="vex"
VERSION="0.1.0"
BIN_DIR="build/Release"
INSTALL_DIR="/usr/local/bin"

PKG_DIR="${APP_NAME}_${VERSION}"
mkdir -p "$BIN_DIR/$PKG_DIR/DEBIAN"
mkdir -p "$PKG_DIR$INSTALL_DIR"

cp "$BIN_DIR/vex" "$PKG_DIR$INSTALL_DIR/"

cat <<EOF > "$PKG_DIR/DEBIAN/control"
Package: $APP_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Maintainer: Heggo <heggo@heggo.dev>
Description: A C++ command-line tool for package management.
EOF

dpkg-deb --build "$PKG_DIR"

echo "Debian package created: ${PKG_DIR}.deb"
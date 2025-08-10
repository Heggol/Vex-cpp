#!/bin/bash

set -e
set -x

PROJECT_ROOT="$(dirname "$0")/.."
cd "$PROJECT_ROOT"

APP_NAME="vex"
VERSION=$(git describe --tags --abbrev=0 2>/dev/null | sed 's/^v//' || echo "0.1.0")
ARCH="x86_64"
PKG_BUILD_DIR="build/arch_pkg_build"

rm -rf "$PKG_BUILD_DIR"
mkdir -p "$PKG_BUILD_DIR"

cat <<EOF > "$PKG_BUILD_DIR/PKGBUILD"
# Maintainer: Heggo <heggo@heggo.dev>
pkgname='$APP_NAME'
pkgver='$VERSION'
pkgrel=1
pkgdesc='A C++ command-line tool for package management.'
arch=('$ARCH')
url='https://github.com/Heggol/Vex-cpp'
license=('MIT')
# TODO: Add runtime dependencies here. E.g. depends=('openssl' 'libarchive')
depends=()
makedepends=('cmake' 'gcc' 'conan' 'git')
source=("\$pkgname::git+file://\$(pwd)/..") # Use local git repo as source
sha256sums=('SKIP')

prepare() {
    cd "\$srcdir/\$pkgname"
    git submodule update --init --recursive
}

build() {
    cd "\$srcdir/\$pkgname"
    mkdir -p build/release
    cd build/release
    conan install ../.. --build=missing
    cmake ../.. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE="Release"
    cmake --build . --config Release
}

package() {
    cd "\$srcdir/\$pkgname"
    install -Dm755 "build/release/vex" "\$pkgdir/usr/bin/vex"
}
EOF

cd "$PKG_BUILD_DIR"
makepkg -f

mv *.pkg.tar.zst ../
cd ..
rm -rf arch_pkg_build

echo "Arch Linux package created in build/ directory."

#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEPS_DIR="$PROJECT_ROOT/.deps"
CURL_IMPERSONATE_SRC="$DEPS_DIR/curl-impersonate-src"
CURL_IMPERSONATE_INSTALL="$DEPS_DIR/curl-impersonate"

sudo apt update
sudo apt install -y \
  build-essential \
  git \
  curl \
  cmake \
  ninja-build \
  pkg-config \
  zlib1g-dev \
  autoconf \
  automake \
  libtool \
  golang-go \
  unzip \
  python3

mkdir -p "$DEPS_DIR"

if [ ! -d "$CURL_IMPERSONATE_SRC" ]; then
  git clone https://github.com/lwthiker/curl-impersonate.git "$CURL_IMPERSONATE_SRC"
fi

cd "$CURL_IMPERSONATE_SRC"

rm -rf build
mkdir build
cd build

../configure --prefix="$CURL_IMPERSONATE_INSTALL"

make chrome-build
make chrome-install

mkdir -p "$CURL_IMPERSONATE_INSTALL/include"
cp -R "$CURL_IMPERSONATE_SRC/build/curl-8.1.1/include/curl" \
      "$CURL_IMPERSONATE_INSTALL/include/"

echo
echo "curl-impersonate installed to:"
echo "$CURL_IMPERSONATE_INSTALL"
#!/bin/bash

# Download and install CMake for docker
# The apt package depends on GCC, which we
# just built ourselves.

# This is called separately in a script to
# avoid creating multiple layers

set -e
set -x

CMAKE_VERSION="$1"

apt-get install -y wget gpg

# Download CMake
cd /tmp
wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-SHA-256.txt
wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh
wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-SHA-256.txt.asc

# Verify CMake download
gpg --import /opt/psx/cmake/cmake_release.crt
gpg --verify cmake-${CMAKE_VERSION}-SHA-256.txt.asc cmake-${CMAKE_VERSION}-SHA-256.txt
sha256sum -c --ignore-missing cmake-${CMAKE_VERSION}-SHA-256.txt

# Install CMake
chmod +x cmake-${CMAKE_VERSION}-Linux-x86_64.sh
./cmake-${CMAKE_VERSION}-Linux-x86_64.sh --prefix=/ --skip-license

# Remove wget & GPG again to conserve space
apt-get remove --purge -y wget gpg
apt-get autoremove -y

# Remove intermediate files in /tmp
rm -rf /tmp/cmake-*
#!/bin/bash

if [ -d "external/catch" ]; then
    exit 0
fi

set -eux

git clone --depth=1 --branch=v2.13.4 https://github.com/catchorg/Catch2

cmake -S Catch2 -B Catch2/build \
    -D "CMAKE_INSTALL_PREFIX=${PWD}/external/catch" \
    -D CMAKE_BUILD_TYPE=Release \
    -D CATCH_BUILD_TESTING=NO \
    -D CATCH_INSTALL_DOCS=NO \
    -D BUILD_TESTING=NO

cmake --build Catch2/build

cmake --install Catch2/build

rm -rf Catch2

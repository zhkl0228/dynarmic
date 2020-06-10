#!/bin/sh

set -e
set -x

apt-get -yq update
apt-get -yq --no-install-suggests --no-install-recommends --force-yes install cmake g++ libboost-dev make

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j2

./tests/dynarmic_tests --durations yes

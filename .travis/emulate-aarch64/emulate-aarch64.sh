#!/bin/sh -ex

docker run --rm --privileged multiarch/qemu-user-static:register --reset
docker run --volume $(pwd):/dynarmic multiarch/debian-debootstrap:arm64-sid /bin/bash -c 'cd /dynarmic; /dynarmic/.travis/emulate-aarch64/build.sh'

#!/usr/bin/env bash

######################################################################
# @author      : Ruan E. Formigoni (ruanformigoni@gmail.com)
# @file        : build
# @created     : Monday Sep 30, 2024 09:13:29 -03
#
# @description :
######################################################################

set -x

DIR_SCRIPT="$(cd -- "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

#Set Docker Arch
if [ -n "${D_ARCH+x}" ] && [ -n "$D_ARCH" ]; then
   if [ "$(uname  -m)" == "aarch64" ]; then
    D_ARCH="arm64v8"
   elif [ "$(uname  -m)" == "x86_64" ]; then
    D_ARCH="amd64"
   fi
else
  export D_ARCH="${D_ARCH}"
fi

# Create build environment
cd "$DIR_SCRIPT"/.docker

docker build . -t dwarfs -f ./Dockerfile.ubuntu \
  --build-arg ARCH="$D_ARCH" \
  --build-arg SCRIPT=build-linux.sh

# Build dwarfs
cd "$DIR_SCRIPT"
mkdir -p dist
docker run --rm --cap-add SYS_ADMIN --device /dev/fuse --privileged \
  -u 0:0 \
  --mount type=bind,source="$PWD",target=/workspace \
  --env BUILD_TYPE=gcc-release-ninja-static-notest \
  --env BUILD_ARCH="$D_ARCH" \
  --env BUILD_DIST=arch \
  --env CC=/usr/bin/gcc-14 \
  --env CXX=/usr/bin/g++-14 \
  dwarfs

set +x

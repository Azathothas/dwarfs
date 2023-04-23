#!/usr/bin/env bash

# shellcheck disable=2016

set -e

mkdir -p dist && cd dist

mkdir -p dwarfs-libs

# Copy libs
for i; do
  for j in $(ldd "$i" | awk '{print $3}'); do
    cp -v "$j" dwarfs-libs 2>&1 | sed "s/^/--/"
  done
done

# Copy libc & preloader
cp /lib/ld-musl-x86_64.so.1 dwarfs-libs
cp /lib/libc.musl-x86_64.so.1 dwarfs-libs

# Patch libs
for i in dwarfs-libs/*; do
  if [[ "$i" =~ ld-musl-x86_64.so.1    ]] ||
     [[ "$i" =~ libc.musl-x86_64.so.1  ]]; then
    continue
  fi
  echo "-- Patch lib: $i"
  patchelf --set-rpath '$ORIGIN' "$i"
done

mkdir -p bin
for i; do
  cp "$i" bin
  echo "-- Patch elf: $i"
  patchelf --set-interpreter '../dwarfs-libs/ld-musl-x86_64.so.1' "bin/$(basename "$i")"
done

{ tee dwarfs.sh | sed 's/^/-- /'; } <<-'EOF'
#!/bin/bash
set -e
HERE=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$HERE/bin"
exec="$(find . -iname "$1" -print -quit)"
LC_ALL="C" LD_LIBRARY_PATH="../dwarfs-libs" "$exec"
EOF

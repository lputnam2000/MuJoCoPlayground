#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)

docker run --rm -t \
  -v "$ROOT_DIR":/work \
  -w /work \
  -e MUJOCO_EGL=off \
  local/cpp-mujoco:1.0 \
  bash -lc "conan profile detect --force && conan install . --output-folder=build --build=missing && cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake && cmake --build build && ./build/mj_offscreen"



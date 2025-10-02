#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)

docker run --rm -t \
  -v "$ROOT_DIR":/work \
  -w /work \
  local/cpp-mujoco:1.0 \
  bash -lc "cmake -S . -B build -G Ninja && cmake --build build && ./build/mj_offscreen"



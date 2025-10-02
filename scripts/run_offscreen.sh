#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
ARG_MODEL=${1:-g1}
if [ "$ARG_MODEL" = "g1" ]; then
  "$ROOT_DIR/scripts/fetch_menagerie_g1.sh"
  ARG_MODEL="models/menagerie/unitree_g1/unitree_g1.xml"
fi

# Clean stale CMake cache/toolchain from previous runs
rm -rf "$ROOT_DIR/build"

docker run --rm -t \
  -v "$ROOT_DIR":/work \
  -w /work \
  local/cpp-mujoco:1.0 \
  bash -lc "cmake -S . -B build -G Ninja && cmake --build build && ./build/mj_offscreen $ARG_MODEL"



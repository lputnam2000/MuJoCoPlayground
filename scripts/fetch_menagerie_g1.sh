#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)
DEST_DIR="$ROOT_DIR/models/menagerie/unitree_g1"

if [ -d "$DEST_DIR" ]; then
  echo "G1 menagerie already present: $DEST_DIR" >&2
  exit 0
fi

mkdir -p "$DEST_DIR"

TMPDIR=$(mktemp -d)
cleanup() { rm -rf "$TMPDIR"; }
trap cleanup EXIT

# Shallow fetch only the unitree_g1 folder using sparse-checkout
git -C "$TMPDIR" init
git -C "$TMPDIR" remote add origin https://github.com/google-deepmind/mujoco_menagerie.git
git -C "$TMPDIR" config core.sparseCheckout true
echo "unitree_g1/*" > "$TMPDIR/.git/info/sparse-checkout"
git -C "$TMPDIR" pull --depth=1 origin main

cp -a "$TMPDIR/unitree_g1/." "$DEST_DIR/"
echo "Fetched Unitree G1 to $DEST_DIR"



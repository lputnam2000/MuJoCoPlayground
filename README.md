## MuJoCo Offscreen (C++23) – Single Docker image workflow

This repo builds and runs a minimal MuJoCo offscreen renderer that saves a video, using a single Docker image.

### What you get
- Ubuntu 24.04 base with toolchain (cmake, ninja, clang, etc.)
- MuJoCo 3.3.5 installed system-wide
- OSMesa for headless OpenGL and FFmpeg for encoding
- One target: `mj_offscreen`

### Build the image
```bash
docker build -t local/cpp-mujoco:1.0 -f Dockerfile .
```

### Run (produces output.mp4 in repo root)
```bash
./scripts/run_offscreen.sh
open output.mp4  # macOS
```

### Project layout
- `src/offscreen.cpp`: offscreen renderer piping frames to FFmpeg
- `CMakeLists.txt`: builds only `mj_offscreen`
- `cmake/FindMuJoCo.cmake`: finds `MuJoCo::mujoco`
- `scripts/run_offscreen.sh`: build and run inside container

### Notes
- Conan is not required for the current minimal setup.
- Real-time viewer and XQuartz are intentionally omitted.

### License
See upstream MuJoCo license and this repository’s respective licenses.

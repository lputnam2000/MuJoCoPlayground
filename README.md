# MuJoCo Playground (C++23) – Docker + CLion + Conan

This repo provides a clean C++23 MuJoCo setup with:
- Stable base dev image (Ubuntu 24.04 arm64)
- Layered image with MuJoCo 3.3.5, OpenGL/GLFW, Conan 2
- Headless demo target (`mj_demo`) and a simple viewer (`mj_viewer`)

## Prerequisites
- macOS with Docker Desktop
- (For on-screen viewer) XQuartz
- CLion (optional)

## Images
- Base: `clion/ubuntu/cpp-env:1.0`
- Layered: `local/cpp-mujoco:1.0`

Rebuild layered image if needed:
```bash
docker build -t local/cpp-mujoco:1.0 -f Dockerfile.mujoco .
```

## Project layout
- `src/main.cpp`: headless minimal MJCF demo
- `src/viewer.cpp`: GLFW windowed demo
- `CMakeLists.txt`: C++23, `find_package(MuJoCo)` + Conan integration
- `cmake/FindMuJoCo.cmake`: finds `MuJoCo::mujoco`
- `conanfile.txt`: fmt, spdlog, catch2 with CMakeToolchain/Deps
- `scripts/`: convenience run scripts for macOS

## Run outside CLion (macOS)

Headless demo:
```bash
./scripts/run_demo.sh
```

Viewer on macOS (opens a window):
```bash
./scripts/run_viewer_mac.sh
```
The script will:
- Ensure XQuartz is running
- Allow network clients via localhost with `xhost +localhost`
- Run the container using TCP DISPLAY `host.docker.internal:0`
- Configure+build via Conan/CMake and run `mj_viewer`

## CLion setup (no manual Docker flags needed)

1) Toolchain (Docker)
- Settings → Build, Execution, Deployment → Toolchains
- Add Toolchain: Name: Docker (cpp-mujoco)
- Environment: Docker, Image: `local/cpp-mujoco:1.0`
- Generator: Ninja

2) Conan plugin
- Settings → Build, Execution, Deployment → Conan
- Enable Conan, Enable Auto install
- Profile: detect/create `default` once
- Additional args: `--build=missing -s build_type=$CMAKE_BUILD_TYPE`
- Ensure “Generate CMake Toolchain” is enabled

3) CMake Profiles
- Settings → Build, Execution, Deployment → CMake
- Add `Debug`:
  - Build dir: `$PROJECT_DIR$/cmake-build-debug`
  - Toolchain: Docker (cpp-mujoco)
  - CMake options: `-DCMAKE_TOOLCHAIN_FILE=cmake-build-debug/conan_toolchain.cmake`
- Add `Release` similarly with `cmake-build-release`
- Reload CMake project

4) Run/Debug configs
- Add Application: `mj_demo` (target `mj_demo`)
- Add Application: `mj_viewer` (target `mj_viewer`)

Viewer display in CLion:
- Preferred: use `./scripts/run_viewer_mac.sh` outside CLion for windowed runs
- Advanced: customize CLion Docker toolchain container settings to map `/tmp/.X11-unix` and set `DISPLAY`, so no manual variables per run are required

## Troubleshooting
- If Conan complains about missing default profile: run `conan profile detect` once (the scripts do this).
- If `mj_viewer` doesn’t show a window:
  - Ensure XQuartz is installed and open
  - In XQuartz → Preferences → Security, check “Allow connections from network clients”, then restart XQuartz
  - Run `xhost +localhost`
  - Use `scripts/run_viewer_mac.sh`

## License
See upstream MuJoCo license and this repository’s respective licenses.

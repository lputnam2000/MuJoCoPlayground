cmake_minimum_required(VERSION 3.20)
project(mujoco_offscreen LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(MuJoCo REQUIRED)

# Only build the offscreen renderer

# Offscreen renderer (OSMesa-based)
add_executable(mj_offscreen
  src/offscreen.cpp
)

target_link_libraries(mj_offscreen PRIVATE
  MuJoCo::mujoco
  OSMesa
  GL
  dl
  pthread
  m
)

install(TARGETS mj_offscreen RUNTIME DESTINATION bin)


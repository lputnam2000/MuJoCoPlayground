cmake_minimum_required(VERSION 3.20)
project(mj_demo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(MuJoCo REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(mj_demo
  src/main.cpp
)

target_link_libraries(mj_demo PRIVATE
  MuJoCo::mujoco
  dl
  pthread
  m
)

install(TARGETS mj_demo RUNTIME DESTINATION bin)

add_executable(mj_viewer
  src/viewer.cpp
)

target_link_libraries(mj_viewer PRIVATE
  MuJoCo::mujoco
  glfw
  GL
  dl
  pthread
  m
)

install(TARGETS mj_viewer RUNTIME DESTINATION bin)

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


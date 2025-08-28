find_path(MUJOCO_INCLUDE_DIR
  NAMES mujoco/mujoco.h
  PATHS /usr/local/include /opt/mujoco/include
)

find_library(MUJOCO_LIBRARY
  NAMES mujoco
  PATHS /usr/local/lib /opt/mujoco/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MuJoCo DEFAULT_MSG MUJOCO_INCLUDE_DIR MUJOCO_LIBRARY)

if(MuJoCo_FOUND)
  add_library(MuJoCo::mujoco UNKNOWN IMPORTED)
  set_target_properties(MuJoCo::mujoco PROPERTIES
    IMPORTED_LOCATION "${MUJOCO_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${MUJOCO_INCLUDE_DIR}"
  )
endif()



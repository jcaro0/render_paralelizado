set(COMMON_SRC_FILES 
  "${CMAKE_SOURCE_DIR}/common/src/vector.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/camara.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/config.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/figuras.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/material.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/render_config.cpp"
  "${CMAKE_SOURCE_DIR}/common/src/scene_config.cpp"
)

set(CURRENT_DIR_SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/test_vector.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_camara.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_config.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_figuras.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_material.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_render_config.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_scene_config.cpp"
)

add_unit_test_target(
  TARGET_NAME utcommon
  SOURCE_FILES ${COMMON_SRC_FILES} ${CURRENT_DIR_SRC_FILES}
  LIBRARY_FILTER common
  COVERAGE_DIR coverage-common
  LIBRARY_TO_LINK common
  INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(render-soa)
target_sources(render-soa 
    PRIVATE 
      src/main.cpp
      src/imagen_soa.cpp
)
target_include_directories(render-soa PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(render-soa PRIVATE Microsoft.GSL::GSL common)

file(GLOB TEST_DATA_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.ppm"
)

add_custom_command(
  TARGET render-soa POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${TEST_DATA_FILES}
    "$<TARGET_FILE_DIR:render-soa>"
  COMMENT "Copiando ficheros de examples/ a $<TARGET_FILE_DIR:render-soa>"
)
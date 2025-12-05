add_executable(render-aos)
target_sources(render-aos 
    PRIVATE 
      src/main.cpp
      src/imagen_aos.cpp
)

target_include_directories(render-aos PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(render-aos PRIVATE Microsoft.GSL::GSL common)

file(GLOB TEST_DATA_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.ppm"
)

add_custom_command(
  TARGET render-aos POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${TEST_DATA_FILES}
    "$<TARGET_FILE_DIR:render-aos>"
  COMMENT "Copiando ficheros de examples/ a $<TARGET_FILE_DIR:render-aos>"
)
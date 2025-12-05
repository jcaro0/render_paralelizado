add_executable(render-par)
target_sources(render-par 
    PRIVATE 
      src/main.cpp
      src/imagen_par.cpp
)

target_include_directories(render-par PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(render-par PRIVATE Microsoft.GSL::GSL common TBB::tbb)

file(GLOB TEST_DATA_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.ppm"
)

add_custom_command(
  TARGET render-par POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${TEST_DATA_FILES}
    "$<TARGET_FILE_DIR:render-par>"
  COMMENT "Copiando ficheros de examples/ a $<TARGET_FILE_DIR:render-par>"
)
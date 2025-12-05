set(COMMON_SRC_FILES 
  "${CMAKE_SOURCE_DIR}/par/src/imagen_par.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/image_diff.cpp"
)
set(CURRENT_DIR_SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/test_par.cpp"
)

add_executable(utpar ${COMMON_SRC_FILES} ${CURRENT_DIR_SRC_FILES})

target_link_libraries(utpar 
  PRIVATE
    common
    Microsoft.GSL::GSL
    GTest::gtest_main
)

target_include_directories(utpar 
  PRIVATE
    ${CMAKE_SOURCE_DIR}/par/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(utpar PRIVATE
  par_EXECUTABLE_PATH="$<TARGET_FILE:render-par>"
)
    
add_dependencies(utpar render-par)

target_compile_definitions(utpar PRIVATE
  TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/examples"
)
    
include(GoogleTest)
gtest_discover_tests(utpar
  DEPENDENCIES 
    render-par
)
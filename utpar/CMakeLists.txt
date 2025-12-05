set(COMMON_SRC_FILES 
  "${CMAKE_SOURCE_DIR}/aos/src/imagen_aos.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/image_diff.cpp"
)
set(CURRENT_DIR_SRC_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/test_aos.cpp"
)

add_executable(utaos ${COMMON_SRC_FILES} ${CURRENT_DIR_SRC_FILES})

target_link_libraries(utaos 
  PRIVATE
    common
    Microsoft.GSL::GSL
    GTest::gtest_main
)

target_include_directories(utaos 
  PRIVATE
    ${CMAKE_SOURCE_DIR}/aos/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(utaos PRIVATE
  AOS_EXECUTABLE_PATH="$<TARGET_FILE:render-aos>"
)
    
add_dependencies(utaos render-aos)

target_compile_definitions(utaos PRIVATE
  TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/examples"
)
    
include(GoogleTest)
gtest_discover_tests(utaos
  DEPENDENCIES 
    render-aos
)
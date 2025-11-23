
set(COMMON_SRC_FILES
  "${CMAKE_SOURCE_DIR}/soa/src/imagen_soa.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../utaos/image_diff.cpp" )
set(CURRENT_DIR_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/test_soa.cpp"
)

add_executable(utsoa ${COMMON_SRC_FILES} ${CURRENT_DIR_SRC_FILES})

target_link_libraries(utsoa
  PRIVATE
    common
    Microsoft.GSL::GSL
    GTest::gtest_main
)

target_include_directories(utsoa
  PRIVATE
    ${CMAKE_SOURCE_DIR}/soa/include     
    ${CMAKE_CURRENT_SOURCE_DIR}/../utaos )

target_compile_definitions(utsoa PRIVATE
  SOA_EXECUTABLE_PATH="$<TARGET_FILE:render-soa>"
)

add_dependencies(utsoa render-soa)

target_compile_definitions(utsoa PRIVATE
  TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/examples"
)

include(GoogleTest)
gtest_discover_tests(utsoa
  DEPENDENCIES
    render-soa
)
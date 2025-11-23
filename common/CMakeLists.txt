add_library(common STATIC)

target_sources(common 
    PRIVATE 
        src/vector.cpp
        src/material.cpp
        src/figuras.cpp
        src/render_config.cpp
        src/camara.cpp
        src/config.cpp
        src/scene_config.cpp
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(common PUBLIC Microsoft.GSL::GSL)
add_library(arpg_runtime STATIC
    src/RuntimeVersion.cpp
)

target_include_directories(arpg_runtime
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(arpg_runtime PUBLIC arpg_compile_options)

set_target_properties(arpg_runtime PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "Runtime"
)

add_library(arpg::runtime ALIAS arpg_runtime)

# Shared compile options for every first-party target in this repository.
# Third party targets fetched via FetchContent intentionally do NOT use this.

add_library(arpg_compile_options INTERFACE)

if(MSVC)
    target_compile_options(arpg_compile_options INTERFACE
        /permissive-          # strict standard conformance
        /W4                   # high warning level
        /EHsc                 # standard C++ exception handling
        /Zc:__cplusplus       # make __cplusplus report the real standard
        /Zc:preprocessor      # conforming preprocessor
        /utf-8                # sources are UTF-8
    )
    target_compile_definitions(arpg_compile_options INTERFACE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )
else()
    target_compile_options(arpg_compile_options INTERFACE
        -Wall
        -Wextra
    )
endif()

# Helper: apply the shared options to a target.
function(arpg_apply_common_options target)
    target_link_libraries(${target} PRIVATE arpg_compile_options)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
endfunction()

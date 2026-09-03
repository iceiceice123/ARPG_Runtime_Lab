# Third party dependencies, acquired with FetchContent so the repository stays
# clean and every machine resolves the exact same versions.

include(FetchContent)

# Keep downloaded sources outside the source tree but inside the workspace.
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/fetchcontent" CACHE PATH "FetchContent cache" FORCE)
set(FETCHCONTENT_QUIET OFF)

# ---------------------------------------------------------------------------
# GoogleTest
# ---------------------------------------------------------------------------
if(ARPG_BUILD_TESTS)
    # Required so GTest links against the same (dynamic) CRT as our targets.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST         OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK           OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(googletest)
endif()

# ---------------------------------------------------------------------------
# Google Benchmark
# ---------------------------------------------------------------------------
if(ARPG_BUILD_BENCHMARKS)
    set(BENCHMARK_ENABLE_TESTING   OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL   OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_WERROR    OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.9.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(googlebenchmark)
endif()

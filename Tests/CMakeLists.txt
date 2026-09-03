# Runtime 独立性的机器化验证：扫描 Runtime 全部源码，禁止出现任何 Unreal 头文件。
# 单元测试是事实，口头约定不是。
add_executable(arpg_tests
    TestRuntimeVersion.cpp
    TestRuntimeIndependence.cpp
)

target_link_libraries(arpg_tests
    PRIVATE
        arpg::runtime
        arpg_compile_options
        GTest::gtest
        GTest::gtest_main
)

set_target_properties(arpg_tests PROPERTIES FOLDER "Tests")

# Path handed to TestRuntimeIndependence.cpp; normalised to forward slashes so
# it survives being embedded into a C++ string literal.
set(ARPG_RUNTIME_ROOT_DIR "${CMAKE_SOURCE_DIR}/Runtime")
string(REPLACE "\\" "/" ARPG_RUNTIME_ROOT_DIR "${ARPG_RUNTIME_ROOT_DIR}")
target_compile_definitions(arpg_tests
    PRIVATE ARPG_RUNTIME_ROOT_DIR="${ARPG_RUNTIME_ROOT_DIR}"
)

include(GoogleTest)
gtest_discover_tests(arpg_tests)

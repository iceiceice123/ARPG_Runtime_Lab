add_executable(arpg_benchmarks
    BenchSmoke.cpp
)

target_link_libraries(arpg_benchmarks
    PRIVATE
        arpg::runtime
        arpg_compile_options
        benchmark::benchmark
        benchmark::benchmark_main
)

set_target_properties(arpg_benchmarks PROPERTIES FOLDER "Benchmarks")

// Task 1 smoke benchmark: proves the benchmark toolchain works and records a
// first baseline. Real workloads (Empty Tick, AoS/SoA, ...) start at Task 7.

#include <arpg/RuntimeVersion.hpp>

#include <benchmark/benchmark.h>

#include <string>
#include <string_view>
#include <vector>

namespace
{
    // Lower bound of the measurement: an empty loop body.
    void BM_EmptyLoop(benchmark::State& state)
    {
        for (auto _ : state)
        {
            benchmark::DoNotOptimize(0);
        }
    }
    BENCHMARK(BM_EmptyLoop);

    // Touch the Runtime from a benchmark to make sure both link together.
    void BM_VersionString(benchmark::State& state)
    {
        for (auto _ : state)
        {
            std::string_view version = arpg::VersionString();
            benchmark::DoNotOptimize(version);
        }
    }
    BENCHMARK(BM_VersionString);

    // Baseline for the cache locality experiments of Week 3: sequential walk
    // over a contiguous buffer.
    void BM_SequentialSum(benchmark::State& state)
    {
        const std::size_t count = static_cast<std::size_t>(state.range(0));
        std::vector<float> data(count, 1.0f);

        for (auto _ : state)
        {
            float sum = 0.0f;
            for (float value : data)
            {
                sum += value;
            }
            benchmark::DoNotOptimize(sum);
        }

        state.SetComplexityN(state.range(0));
        state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) *
                                static_cast<int64_t>(count) * static_cast<int64_t>(sizeof(float)));
    }
    BENCHMARK(BM_SequentialSum)->RangeMultiplier(8)->Range(1 << 10, 1 << 20)->Complexity();

} // namespace

BENCHMARK_MAIN();

# Task 01 — 工程骨架

日期：2026-09-03

## 任务目的

建立一个最小、独立、可测试的 C++ Runtime 工程。它是后面 93 个 Task 的地基：
任何实验都要能「编译 → 测试 → 测量」三步闭环，否则结论不可信。

## 今天要解决的问题

1. 机器上没有 `cmake` / `git` / `ninja` 的 PATH，必须先定位工具链。
2. Runtime 如何物理上保证不依赖 Unreal（Decision 003）。
3. GoogleTest / Google Benchmark 如何纳入工程且不污染仓库。

## 实现内容

- CMake 工程：C++20、强制标准一致性、关闭编译器扩展、导出 `compile_commands.json`。
- 四个 target：`arpg_runtime`（静态库）、`arpg_tests`、`arpg_benchmarks`、`arpg_runtime_app`。
- 依赖靠 `FetchContent` 拉取并锁定版本，仓库里不存放第三方源码。
- 全局统一 MSVC 动态 CRT，避免第三方库与自有代码运行时库不匹配。
- 把「Runtime 不依赖 Unreal」写成可执行测试，而不是口头约定。
- `scripts/env.ps1` + `scripts/build.ps1`，屏蔽本机工具链路径问题。

## 新增 / 修改文件

```text
CMakeLists.txt                         顶层工程配置
CMakePresets.json                      debug / relwithdebinfo / release 三套配置
cmake/CompileOptions.cmake             共享编译选项（arpg_compile_options）
cmake/Dependencies.cmake               FetchContent：GoogleTest v1.15.2 / Benchmark v1.9.1
Runtime/CMakeLists.txt
Runtime/include/arpg/RuntimeVersion.hpp
Runtime/src/RuntimeVersion.cpp
Tests/CMakeLists.txt
Tests/TestRuntimeVersion.cpp
Tests/TestRuntimeIndependence.cpp      Runtime 零 Unreal 依赖的机器化校验
Benchmarks/CMakeLists.txt
Benchmarks/BenchSmoke.cpp
Apps/CMakeLists.txt
Apps/RuntimeApp/CMakeLists.txt
Apps/RuntimeApp/main.cpp
UnrealClient/README.md                 占位，说明依赖方向
scripts/env.ps1                        工具链进 PATH
scripts/build.ps1                      一键 configure / build / test / bench
Docs/README.md
Docs/build.md
Docs/tasks/task01.md
.gitignore
PROGRESS.md
```

## Unit Test

```text
4/4 通过

RuntimeVersion.NumbersMatchStringLiteral              Passed
RuntimeVersion.RuntimeLibraryIsLinkable               Passed
RuntimeIndependence.RuntimeDirectoryContainsSourceFiles Passed
RuntimeIndependence.NoUnrealHeadersIncluded           Passed
```

前两个证明 Runtime 静态库能被链接、版本宏与字符串一致。
后两个是 Decision 003 的机器化实现：递归扫描 `Runtime/` 下所有 C/C++ 源文件，
解析 `#include` 指令，命中 Unreal 特征串（`CoreMinimal.h`、`Engine/`、`UObject/`、`Unreal` 等）即失败。

## Benchmark

```text
Workload : BenchSmoke（空循环 / 版本字符串 / 顺序遍历求和）
Hardware : 20 X 2918 MHz；L1d 48 KiB(x10)，L2 1280 KiB(x10)，L3 24576 KiB(x1)
Build    : MSVC 19.36.32546.0，RelWithDebInfo，Ninja

Benchmark                         Time             CPU   Iterations
BM_EmptyLoop                  0.706 ns        0.419 ns    224000000
BM_VersionString              0.764 ns        0.558 ns    224000000
BM_SequentialSum/1024           445 ns          279 ns       224000  bytes_per_second=13.6719Gi/s
BM_SequentialSum/4096          1898 ns         1250 ns       100000  bytes_per_second=12.207Gi/s
BM_SequentialSum/32768        16023 ns         9342 ns        20070  bytes_per_second=13.0664Gi/s
BM_SequentialSum/262144      120240 ns        94169 ns         4480  bytes_per_second=10.3704Gi/s
BM_SequentialSum/1048576     473359 ns       383650 ns          448  bytes_per_second=10.1818Gi/s
BM_SequentialSum_BigO          0.45 N          0.37 N
BM_SequentialSum_RMS              1 %             1 %

Before     : 无（首次基线）
After      : 无（本次只验证链路）
Conclusion : 基准框架可用。顺序遍历带宽从 L1 命中区的 ~13.7 Gi/s 衰减到
             超出 L2/L3 后的 ~10.2 Gi/s，后续 Week 3 的 cache locality
             实验以此为对照基线。
```

## 遇到的问题

1. **`cmake` / `git` / `ninja` 不在系统 PATH**
   VS2022 装了但没暴露到 PATH。解决：`scripts/env.ps1` 按 Professional/Enterprise/Community/Preview 顺序探测 VS 安装目录并前置到 PATH。

2. **MSVC 环境缺失导致 `No CMAKE_CXX_COMPILER could be found`**
   Ninja 生成器下必须能找到 `cl.exe`。解决：`scripts/build.ps1` 调用 `vcvarsall.bat x64`，把输出的环境变量导入当前 PowerShell 会话。

3. **MSVC 静态/动态 CRT 混用风险**
   GoogleTest 默认 `/MT`，我们的目标默认 `/MD`，混链会 LNK2038。
   解决：顶层设 `CMAKE_MSVC_RUNTIME_LIBRARY` 统一动态 CRT，并对 GTest 强制 `gtest_force_shared_crt=ON`。

## 学到的东西

- **约定必须可执行**。「Runtime 不依赖 Unreal」如果只是写在文档里，迟早会被违反；写成测试后它就是事实。
- **构建脚本是工程的一部分**。本机工具链路径混乱是常态，把它固化进脚本，后面 90 多个 Task 就不会反复踩。
- **依赖版本要锁定**。`GIT_TAG v1.15.2` / `v1.9.1`，保证换机器后 benchmark 数据可比。

## 工业实现对照

暂不适用。Task 1 只建骨架，没有可对照的自研实现。Week 1 结束（Task 7 之后）再统一做 Game Loop 的 UE 对照。

## 最终决定

- 生成器：**Ninja + MSVC**（比 MSBuild 快，适合高频 build / benchmark）；同时提供 `CMakePresets.json` 给 IDE。
- 依赖获取：**FetchContent 锁定版本**，不手工安装、不入库。
- 目录划分：`Runtime / Tests / Benchmarks / Apps / UnrealClient / Docs / cmake / scripts`。
- 独立性保障：**扫描测试**而非人工检查。
- 构建入口：**`scripts/build.ps1`**，后续 Task 复用同一入口。

## 下一步

**Task 2 — Log / Assert**：实现最小日志和断言，完成标准是 Runtime 不依赖 Unreal 也能输出诊断信息。

## Git Commit

仓库已初始化（`git init -b main`），**尚未提交**。首次 commit 内容为本 Task 的全部新增文件。

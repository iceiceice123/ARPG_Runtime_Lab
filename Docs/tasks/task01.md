# Task 01 — 工程骨架

日期：2026-09-03 ~ 09-07（搭建 + 逐层理解）

## 任务目的

建立一个最小、独立、可测试的 C++ Runtime 工程。它是后面 93 个 Task 的地基：
任何实验都要能「编译 → 测试 → 测量」三步闭环，否则结论不可信。

> 补充（学习视角）：Task 1 的完成标准本质不是"工程能 build"，而是"**我能理解这个工程的每一层**"。
> 理解路线：scratch 亲手从零搭一遍 → 再逐层读懂正式工程（0 全貌 / 1 代码 / 2 CMake / 3 cmake目录 / 4 脚本 / 5 独立性测试）。

## 今天要解决的问题

1. 机器上没有 `cmake` / `git` / `ninja` 的 PATH，必须先定位工具链。
2. Runtime 如何物理上保证不依赖 Unreal（Decision 003）。
3. GoogleTest / Google Benchmark 如何纳入工程且不污染仓库。
4. （学习视角）CMake 多 target 工程为什么这样组织、构建系统底层如何工作。

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

> 学习备注：第 4 个测试能"分别执行两个 .cpp 里的用例"，靠的是 gtest 的注册表机制（见文末问答 Q：测试 main）。

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

> 学习备注：**Benchmark 必须在 RelWithDebInfo/Release 下跑**。Debug 无优化，数据比 Release 慢约 5 倍
> （实测 BM_EmptyLoop：Debug 3.27 ns vs RelWithDebInfo 0.706 ns），输出还会带
> `WARNING: Library was built as DEBUG`。

## 遇到的问题

### 工程层面（搭建时的环境/工具问题）

1. **`cmake` / `git` / `ninja` 不在系统 PATH**
   VS2022 装了但没暴露到 PATH。解决：`scripts/env.ps1` 按 Professional/Enterprise/Community/Preview 顺序探测 VS 安装目录并前置到 PATH。

2. **MSVC 环境缺失导致 `No CMAKE_CXX_COMPILER could be found`**
   Ninja 生成器下必须能找到 `cl.exe`。解决：`scripts/build.ps1` 调用 `vcvarsall.bat x64`，把输出的环境变量导入当前 PowerShell 会话（`cmd /c "vcvarsall && set"` 技巧）。

3. **MSVC 静态/动态 CRT 混用风险**
   GoogleTest 默认 `/MT`，我们的目标默认 `/MD`，混链会 LNK2038。
   解决：顶层设 `CMAKE_MSVC_RUNTIME_LIBRARY` 统一动态 CRT，并对 GTest 强制 `gtest_force_shared_crt=ON`。

4. **PowerShell 默认禁止运行 .ps1 脚本（ExecutionPolicy）**
   跑 `./scripts/build.ps1` 报"在此系统上禁止运行脚本"。解决：`Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser`。

5. **CLI 与 IDE 构建状态不一致 / STL1001 误报**
   命令行 `build.ps1 -Clean` 能过，但 IDE 里构建失败、报大量 STL1001（`<hash>` 系列错误）。原因：IDE 的 CMake 缓存（含 FetchContent 拉的依赖）与 CLI 是两套，且 IntelliSense 报错 ≠ 真实编译错误。解决：在 IDE 里重新 Configure（或删 build 重新 configure），并让 IDE 用与 CLI 一致的 preset（relwithdebinfo）。经验：**"曾经能跑" ≠ "现在能跑"，真实状态以亲手 build 为准。**

6. **`BenchSmoke.cpp` 里 main 来源重复（冗余）**
   CMakeLists 链接了 `benchmark::benchmark_main`（自带 main），文件里又写了 `BENCHMARK_MAIN()`（宏展开 main）。能跑是因为静态库按需提取、宏的 main 胜出。**二选一即可**，当前保留两者是冗余但无害。用实验验证：注释宏能跑（main 来自库），删链接也能跑（main 来自宏）。

### 学习层面（概念理解过程中解决的问题，完整问答见文末）

- 从"没有 AI 就无法下手"→ 亲手在 scratch（`D:\ARPG_Demo`）从零搭出 库 + exe + 测试 + benchmark 四件套。
- 一开始连"CMake 是什么、为什么没有 sln、为什么三个 exe"都懵——全部通过逐层问答内化（见文末记录）。

## 学到的东西

### 工程层面

- **约定必须可执行**。「Runtime 不依赖 Unreal」如果只是写在文档里，迟早会被违反；写成测试后它就是事实。
- **构建脚本是工程的一部分**。本机工具链路径混乱是常态，把它固化进脚本，后面 90 多个 Task 就不会反复踩。
- **依赖版本要锁定**。`GIT_TAG v1.15.2` / `v1.9.1`，保证换机器后 benchmark 数据可比。

### 学习层面

- **从上层到底层，最大的转变是"引擎帮你藏的东西，现在要自己管"**：构建、编译、链接、CRT、依赖、路径——这些 Unity/C# 时代从未见过的东西，正是"底层能力"的组成部分。
- **CMake 的核心思想是"配置复用与显式优于隐式"**：INTERFACE 库抽象编译选项、ALIAS 起命名空间别名、PUBLIC/PRIVATE 控制可见性——本质是软件工程的抽象思维，会迁移到以后写架构。
- **读懂工程 ≠ 读懂每个文件**：基础设施（编译选项、脚本）知道职责、出问题知道去哪找即可；必懂的是自己写的和频繁改的。
- **验证理解的最好方式是"改代码看结果"**：注释宏看入口变化、删链接看是否冲突、切 Debug 看数据变化——比读十遍记得牢。

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

仓库已初始化（`git init -b main`），**已完成首次提交**（内容为本 Task 的全部文件）。

---

# 学习问答记录（我的理解版）

> 以下整理了 Task 1 学习过程中的所有问答，按主题分组，作为"理解存折"。
> 复习自测：能用自己的话讲出来 = 懂；陌生 = 回去重看。

## 1. 命令行与构建工具基础

**Q: 为什么用 PowerShell 而不是 cmd？**
A: 两者跑 CMake 命令没区别，PowerShell 是现代默认 shell。唯一注意：PowerShell 运行本目录 exe 要写 `.\xxx.exe`（前面加 `.\`）。

**Q: `cmake -S . -B build` 是什么意思？**
A: `-S` 指定源码目录（去哪读 CMakeLists.txt），`.` 是当前目录；`-B` 指定构建目录（产物放哪）。源码与构建分离：删掉 `build/` 就能干净重来。

**Q: Ninja 是什么？和 CMake 什么关系？**
A: Ninja 是高速构建"执行器"。CMake 负责"描述工程 + 生成构建脚本"（生成菜谱），Ninja 负责"执行编译"（按菜谱做菜）。VS 生成器则产出 `.sln`。

**Q: 没有 `.sln` 怎么编码？**
A: 编码写的是 `.cpp/.h` 文本文件，跟 `.sln` 无关。VS 用"打开文件夹"就能获得完整编码体验（IntelliSense、断点调试）。CMake 工程天然无 `.sln`，这是现代工作流不是缺陷。

**Q: `.h` 和 `.hpp` 有区别吗？**
A: 无功能区别，纯命名习惯（`.hpp` 强调"这是 C++"）。一个工程统一一种即可。

## 2. CMake 工程组织

**Q: 为什么有多个 CMakeLists.txt？**
A: CMake 模块化设计：每个构建单元（一个库/exe）一个 CMakeLists，顶层用 `add_subdirectory` 串成一棵树。优点是职责单一、可独立开关、可复用。

**Q: 每个模块都有了 CMakeLists，为什么顶层还要一个？**
A: CMake 需要唯一入口。顶层负责三件子模块管不了的事：① `project()` 只能定义一次；② 全局设置（C++20/CRT）必须先于子模块；③ `add_subdirectory` 总调度、决定挂哪些模块。

**Q: 每加新文件都要改 CMakeLists，很麻烦吗？**
A: 只在"**新建 .cpp**"时才加一行（在已有文件里写代码不用动）。显式列出是官方推荐（构建可预测）；`file(GLOB)` 可自动收集但有代价，不推荐依赖。

**Q: CMakeLists 里文件名带双引号是什么？**
A: 合法写法（quoted argument），带不带效果一样。带引号能兜住含空格/特殊字符的路径，VS 自动生成时习惯性加引号，是防御写法。

**Q: 顶层 `include(CompileOptions)` 找的是什么？**
A: 找 `cmake/CompileOptions.cmake` **文件**并内联执行（CMake 自动补 `.cmake` 后缀），`CMAKE_MODULE_PATH` 提供搜索路径。**文件 ≠ 里面的 target**：文件是"菜谱"，执行后创建 target（菜）`arpg_compile_options`。

## 3. 库、可执行文件与链接

**Q: 为什么编译成多个 exe（tests / benchmarks / app）？**
A: 一个核心库（`arpg_runtime`）+ 三个壳。逻辑写进库，三个 exe 分别"用/验/测"同一份逻辑。必须分开的原因：main 冲突（gtest_main 和 benchmark_main 都要接管 main）、构建配置不同（benchmark 必须 Release）、运行方式不同、故障隔离。

**Q: 游戏逻辑写在哪？App 里吗？**
A: **不**。逻辑写在 Runtime 库（能被 Tests/Benchmarks 链接、能独立于 UE）；App 只是壳/展示层。三个壳调用库里的**同一个函数**。

**Q: `STATIC` 是什么？为什么显式写？**
A: 静态库（`.lib`，链接时焊进 exe）。不写默认也是 STATIC，但显式写是"显式优于隐式"——防止全局 `BUILD_SHARED_LIBS` 开关悄悄把库变成 dll。

**Q: 静态库和静态 CRT 是一回事吗？**
A: 不是。静态库是"代码怎么打包"（.lib）；静态 CRT（/MT）是"运行时库要不要编进 exe"。两回事。

**Q: STATIC → SHARED 麻烦吗？**
A: 麻烦不在改 CMakeLists，在 Windows 符号导出：每个公共类/函数要加 `__declspec(dllexport/dllimport)` 宏，还要管 dll 运行时部署。当前项目静态是正确选择。

**Q: 编译和链接的区别？**
A: 编译：`.cpp → .obj`；归档：`.obj → .lib`；链接：exe 的 `.obj + .lib → .exe`。`add_library` 只做前两步，"链接"发生在 `target_link_libraries` 时。`RuntimeVersion.cpp` 被"编译成库"，不是被"链接"。

**Q: exe 的 main 从哪来？**
A: 每个 exe 必须有且仅有一个 main。可以自己写，也可以来自链接的库——`gtest_main` / `benchmark_main` 内部自带 main。文件里写 `BENCHMARK_MAIN()` 宏与链接 `benchmark_main` 库**二选一**即可。

**Q: 为什么"宏的 main + 库的 main"共存不冲突？**
A: 静态库**按需提取**：exe 已有 main 符号时，链接器不再去 `.lib` 里找 main。所以宏展开的 main 胜出、库里的 main 闲置（冗余但无害）。验证：注释掉宏还能跑（入口换成库的）；删掉库链接保留宏也能跑（入口换成宏的）。

**Q: 链接器怎么"找" main？**
A: 不"找第一个"。exe 真正的第一行入口是 CRT 的 `mainCRTStartup`，它负责初始化并 `call main`。链接器要求**恰好一个** main：没有 → LNK1561，多个 → LNK2005。

**Q: `PRIVATE` / `PUBLIC` / `INTERFACE` 区别？**
A: `PUBLIC` 对外传播（include 目录、链接关系、编译选项传给下游）；`PRIVATE` 不传播（自己用）。exe 是依赖链终点用 `PRIVATE`；库公开自己的头文件目录必须 `PUBLIC`，否则下游 include 不到。

**Q: `target_link_libraries(arpg_runtime PUBLIC arpg_compile_options)` 到底做了什么？**
A: 不是"焊库"，而是"采纳一个 INTERFACE 库打包的要求"：让 runtime 编译时自动带上 `/W4 /utf-8 /DNOMINMAX` 等，并因 PUBLIC 传染给所有下游。本质是**配置复用/抽象**——"把重复的编译配置写一次，处处用"。

**Q: `ALIAS` 是什么？为什么叫它"起艺名"？**
A: `add_library(arpg::runtime ALIAS arpg_runtime)` 给已有库注册第二个名字，不产生新库。带 `::` 的名字（命名空间 target）只能是 ALIAS/IMPORTED，语义清晰 + 与第三方（`GTest::gtest`）风格统一。

**Q: 为什么 `Runtime/CMakeLists.txt` 只列 .cpp 不列 .hpp？**
A: `add_library` 列的是"要拿去编译的文件"，编译器只编译 .cpp。.hpp 靠 `#include` 拉入、不单独编译；通过 `target_include_directories` 公开它的目录位置即可。

**Q: `$<BUILD_INTERFACE:...>` / `$<INSTALL_INTERFACE:...>` 是什么？**
A: 生成器表达式，让同一个 include 目录按"库被谁用"取不同路径：开发期内指向源码目录；安装后被外部用时指向安装目录。当前工程不做 install，INSTALL 那条实际用不上，是标准姿势。

**Q: `FOLDER "Runtime"` 是干嘛的？**
A: 纯 IDE 显示分组（VS 目标视图里按 Runtime/Tests/Apps 归类），对命令行构建零影响。

## 4. 测试与第三方依赖

**Q: GoogleTest 和 Google Benchmark 分别是什么？**
A: gtest 验证"对不对"（断言），benchmark 测"快不快"（耗时）。配合：先保证代码正确（gtest），再测性能（benchmark）。不是每个函数都要测：test 测"易错有逻辑"的，benchmark 只测"做决策或怀疑瓶颈"的。

**Q: gtest 什么时候下载到本地？**
A: FetchContent 在 **configure 时**（跑 `cmake -S . -B build`）下载到 `build/_deps/`。只下第一次，之后复用；`GIT_TAG` 锁版本（上游更新不会自动拉）；删 `build/_deps` 或整个 build 可强制重下。

**Q: 自己的代码改了，要重新拉依赖吗？**
A: 不用。`cmake --build` 自动增量编译改动文件。只有"新增 .cpp 并登记到 CMakeLists"或"改了 CMakeLists"时才需要重新 configure（`cmake -S . -B build`）。

**Q: `GTest::gtest` 前面的 `GTest::` 是什么？**
A: 第三方库导出 target 时自带的命名空间前缀，标识归属、防名字冲突。来源是 googletest 被 FetchContent 引入后，它自己的 CMakeLists 定义的 target。

**Q: 怎么知道一个库导出了哪些名字？**
A: 查它的**官方文档**（CMake 集成章节会写明），或看下载源码里的 CMakeLists（`add_library`/`ALIAS` 定义处）。不猜。

**Q: `add_test` 是干嘛的？ctest 又是什么？**
A: `add_test` = 把"运行某 exe"登记为 ctest 能管理的测试。ctest = CMake 自带的测试指挥中心。分工：gtest 管 exe **内部**判对错，ctest 管项目**层面**统一调度、汇总、给 CI 返回码。

**Q: `gtest_discover_tests` 和 `add_test` 有什么区别？**
A: `add_test` 把整个 exe 当 1 个测试；`gtest_discover_tests` 运行 exe 让它自报用例（`--gtest_list_tests`），**逐个 TEST 注册**，ctest 报告里能看到每个用例通过/失败。

**Q: 测试 exe 的 main 在哪？两个测试文件怎么都被执行了？**
A: main 来自 `gtest_main` 库。核心机制：每个 `TEST(...)` 宏会生成一个**全局静态注册对象**，它在 main 执行前就被构造、把自己登记进 gtest 注册表；`RUN_ALL_TESTS()` 按注册表点名执行。是"**报名 + 点名**"，不是"谁调用谁"。这也是加新测试不用改任何注册代码的原因。

**Q: `target_compile_definitions(... ARPG_RUNTIME_ROOT_DIR=...)` 是干嘛？**
A: 把"Runtime 目录路径"以预处理宏注入测试代码（等价于代码里写 `#define ARPG_RUNTIME_ROOT_DIR "D:/..."`），供 `TestRuntimeIndependence.cpp` 扫描用——避免硬编码路径，项目挪位置 CMake 自动更新。反斜杠换正斜杠是因为路径进了 C++ 字符串字面量（`\R` 会被当转义符）。

## 5. Windows / MSVC / 构建配置

**Q: CRT 是什么？**
A: C Runtime Library，C/C++ 程序的"公共运行时"：`printf`/`malloc`/字符串实现 + **`mainCRTStartup` 启动代码** + 全局对象构造管理 + 堆管理。你的代码只是"调用者"，实现都在 CRT 里。

**Q: `/MD` 和 `/MT` 的区别？**
A: `/MT` 把 CRT 复制一份编进 exe（自带小灶）；`/MD` 运行时去系统共享 dll（`vcruntime140.dll`，公共食堂）调用。混用会 LNK2038（堆/内部状态不匹配）。工程必须**统一一种**。

**Q: vcruntime140.dll 是构建机器需要还是运行机器需要？**
A: **运行 exe 的那台机器**需要；构建机器链接时只需要 `.lib`（导入库 = "目录卡片"，不复制 dll）。dll 由 VC++ Redistributable 提供，不是 Windows 原生自带。个人开发自用无感，发布才考虑。

**Q: 既然 /MT 自包含，为什么不干脆用它？**
A: /MT 的代价：每个 exe 自带一份（体积）、CRT 状态隔离（跨库传指针会崩）、补丁要重新编译才能吃到。更关键：**统一比选哪种更重要，而生态（UE、第三方库）默认 /MD**。所以跟随生态 /MD。

**Q: Debug / Release / RelWithDebInfo 区别？**
A: Debug 无优化、可全功能调试；Release 全优化（/O2）、难调试；RelWithDebInfo = **Release 性能 + 保留调试符号**。训练项目既写代码调试又要跑真实 benchmark，所以默认 RelWithDebInfo。

**Q: NDEBUG 和断言有什么关系？**
A: Release/RelWithDebInfo 都定义 `NDEBUG` 宏，会裁掉标准库 `assert()`——这就是"发布构建断言零开销"。Task 2 自己实现 ASSERT 时会用到这个机制。

**Q: IntelliSense 报错但编译能过？**
A: IDE 的 IntelliSense 和真实编译是**两套独立机制**。IDE 波浪线/找不到头 ≠ 编译错误，以 `cmake --build` 结果为准。FetchContent 依赖偶尔让 IntelliSense 找不到（STL1001 那次），重新 configure 可解。

**Q: `mainCRTStartup` 是什么？**
A: exe 真正的第一行执行入口（CRT 提供），负责初始化堆、**构造所有全局/静态对象**，然后才调用你的 `main`。这解释了"静态对象在 main 前构造"。

**Q: `scripts/build.ps1` 解决了什么？**
A: 本机 cmake/git/ninja 不在 PATH + MSVC 环境未加载。脚本自动定位 VS、通过 `cmd /c vcvarsall && set` 导入 MSVC 环境、一键 configure/build/test。日常开发用 `./scripts/build.ps1`（加 `-Configuration`/`-Clean`/`-RunBenchmarks` 等参数），不用碰环境配置。

## 6. 理念层

**Q: 为什么把"Runtime 不依赖 Unreal"写成测试？**
A: **约定必须可执行**。写在文档里的规则迟早被违反；写成测试（TestRuntimeIndependence）它就是机器检查的事实。局限：只能抓 `#include` 层面的直接依赖，抓不到间接/藏起来的依赖——但足够威慑"手滑"。

**Q: 为什么要自己从零搭工程，而不是直接用 AI 搭好的？**
A: 训练目标是"理解"，不是"拥有能跑的东西"。亲手搭一遍（哪怕是最小版）才能看懂工程、才能在出问题时自己修。AI 搭的工程当"参考答案"对照即可。

**Q: "读懂工程"不等于"读懂每个文件"？**
A: 对。工程里有些是基础设施（编译选项、脚本），需要知道"存在、负责什么、出问题去哪找"，不需要逐行精通。优先级：必懂（自己写的/频繁改的）> 略懂（知道职责）> 放一放（用到再查）。

**Q: 每次写代码都要动 CMakeLists / 每次加文件都要登记？**
A: 不是"每次编写"，是"每次新建 .cpp"。加一行是一秒的肌肉记忆，不是负担。C++ 的"显式优于隐式"和"亲自管理本该自动化的事"正是底层能力的一部分。

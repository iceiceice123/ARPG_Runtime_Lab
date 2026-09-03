# 构建说明

## 工具链

| 工具 | 版本 | 位置 |
|---|---|---|
| CMake | 3.29.5 | Visual Studio 2022 自带，不在系统 PATH |
| Ninja | 1.12.1 | Visual Studio 2022 自带，不在系统 PATH |
| MSVC | 19.36.32546.0 (工具集 14.36) | VS2022 Professional |
| Git | 2.55.0 | `C:\Program Files\Git\cmd`，不在系统 PATH |
| C++ 标准 | C++20 | 强制，关闭编译器扩展 |

依赖通过 `FetchContent` 自动拉取，无需手工安装：

| 依赖 | 版本 |
|---|---|
| GoogleTest | v1.15.2 |
| Google Benchmark | v1.9.1 |

## 快速开始

推荐方式，脚本会自动定位 VS、初始化 MSVC 环境、configure、build、跑测试：

```powershell
./scripts/build.ps1
```

常用参数：

```powershell
./scripts/build.ps1 -Configuration Debug                 # 切构建类型
./scripts/build.ps1 -Clean                               # 清空 build 目录后重建
./scripts/build.ps1 -SkipTests                           # 只 build
./scripts/build.ps1 -RunApp                              # build 完运行 arpg_runtime_app
./scripts/build.ps1 -RunBenchmarks                       # build 完跑 benchmark
./scripts/build.ps1 -ConfigureOnly                       # 只 configure
```

## 手工方式

先给当前 shell 加上工具链：

```powershell
. ./scripts/env.ps1      # 注意前面的点，脚本要影响当前会话
```

然后：

```powershell
cmake -S . -B build/relwithdebinfo -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/relwithdebinfo
ctest --test-dir build/relwithdebinfo --output-on-failure
./build/relwithdebinfo/Benchmarks/arpg_benchmarks.exe
```

MSVC 需要在开发者环境下运行。若 `cl.exe` 不在 PATH，执行：

```cmd
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
```

## 产物

```text
build/<config>/
├─ Runtime/arpg_runtime.lib
├─ Tests/arpg_tests.exe
├─ Benchmarks/arpg_benchmarks.exe
└─ Apps/RuntimeApp/arpg_runtime_app.exe
```

## 工程约定

1. **CRT 一致性**：`CMAKE_MSVC_RUNTIME_LIBRARY` 全局设为动态 CRT（`/MD`、`/MDd`），GoogleTest 也强制 `gtest_force_shared_crt`。否则会出现 LNK2038 运行时库不匹配。
2. **构建目录固定为 out-of-source**：`build/<config>`，便于 `TestRuntimeIndependence.cpp` 扫描 `Runtime/` 时不会误扫到生成物。
3. **依赖缓存**在 `.cache/fetchcontent`，已加入 `.gitignore`。
4. **编译选项统一**由 `cmake/CompileOptions.cmake` 提供（`arpg_compile_options`），第三方库不使用它。
5. `NOMINMAX` / `WIN32_LEAN_AND_MEAN` 对所有一方可代码生效。

## FAQ

**Q：`cmake 不是可运行程序`**
A：CMake 没进 PATH。先执行 `. ./scripts/env.ps1`，或直接用 `scripts/build.ps1`。

**Q：`No CMAKE_CXX_COMPILER could be found`**
A：当前 shell 没有 MSVC 环境。用 `scripts/build.ps1`（会自动导入 vcvars），或手工执行 `vcvarsall.bat x64`。

**Q：想换生成器 / 用 Visual Studio IDE 打开**
A：`CMakePresets.json` 提供 `debug` / `relwithdebinfo` / `release` 三个 Ninja 配置。VS 直接打开文件夹时会读取 presets。

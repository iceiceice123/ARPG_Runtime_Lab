# Task 02 — Log / Assert（进行中）

日期：2026-09-09

## 任务目的

实现最小日志和断言。完成标准（PROGRESS.md）：

```text
Runtime 不依赖 Unreal 也能输出诊断信息
Release 构建中断言零开销
日志内容可被测试捕获并断言
```

## 当前进度

- [x] 理解断言机制（零开销、触发条件、触发动作、do-while）
- [x] Assert 第 1 步：最小能跑的断言，Debug 下验证触发成功
- [ ] 清理 main.cpp 里的验证断言 `ARPG_ASSERT(1 == 2)`
- [ ] Log 系统（级别 + Sink + LogMessage + 宏）
- [ ] Assert 升级：cout → 日志、拆函数、可替换 handler
- [ ] 单元测试（日志捕获 + 断言触发）

## 实现内容（已完成部分）

### `Runtime/include/arpg/Assert.hpp`（第 1 步，最小版）

```cpp
#include <source_location>
#include <iostream>
#include <cstdlib>
#include <intrin.h>

#ifdef NDEBUG
    #define ARPG_ASSERT(expr)  ((void)0)          // Release：零开销
#else
    #define ARPG_ASSERT(expr)                    \
        do {                                     \
            if (!(expr)) {                       \
                std::source_location loc = std::source_location::current(); \
                std::cout << "Assert failed: " << #expr << " at " << loc.file_name() << ":" << loc.line() << '\n'; \
                __debugbreak();                   \
                std::abort();                     \
            }                                    \
        } while (0)
#endif
```

状态：**已验证**。Debug 下 `ARPG_ASSERT(1 == 2)` 输出 `Assert failed: 1 == 2 at main.cpp:xx` 后崩溃。cout 是临时方案，等 Log 写好换掉。

## 新增 / 修改文件

```text
Runtime/include/arpg/Assert.hpp          新增：最小断言宏（第 1 步）
Docs/tasks/task02_reference/Log.md       新增：Log 参考答案（卡住时再看）
```

## Unit Test

暂无（断言尚未接日志、未抽函数，测试等 Log 写好后一起写）。

## 遇到的问题

暂无。

## 学到的东西

- **零开销 = 参数不求值**：宏体里若没引用 `expr`，`expr` 彻底消失（`ARPG_ASSERT(expensive())` 在 Release 下不执行 expensive）。函数做不到。
- **`((void)0)` vs 空**：空是"碰运气地没有"（表达式位置会语法错误），`((void)0)` 是"体面地没有"（合法表达式，任何位置安全，`(void)` 压掉"未使用值"警告）。
- **`do { } while(0)` 防悬空 else**：把多行代码打包成"一条语句"，否则宏里的 `if` 会和调用处的 `else` 错配。
- **断言的语义**：`expr` 为假才触发；断言是"开发期安全网"，Release 拆掉后 bug 不是消失而是潜伏。
- **触发动作顺序**：日志报错 → 调试器断点（`__debugbreak`）→ `abort`。断点必须在 abort 前（"冻结现场"）。
- **格式化 vs 输出分层**：格式化用 `std::format`（类型安全），输出用 printf 的 `%.*s`（对 string_view 友好）。
- **sink = 数据汇**：日志数据流的"归宿"，source→sink 解耦了"产生"与"去向"，让日志可被测试捕获。

## 学习问答记录（本次会话核心问答）

### 1. 断言机制

**Q: 断言零开销怎么实现？**
A: 靠预处理 `#ifdef NDEBUG` 在编译前删代码，不是运行时判断。Release 下 `ARPG_ASSERT(x)` 整句蒸发，`x` 不求值。

**Q: Release 下断言不会打断了吗？**
A: 对，但别理解成"更安全"——断言是开发期安全网，Release 拆网后 bug 潜伏到别处爆发。

**Q: 断言什么时候触发？**
A: `expr` 为 false。断言写的是"期望为真的条件"，别写反（`b != 0` 而非 `b == 0`）。

**Q: 触发时要做什么？**
A: 日志报错（表达式+文件+行号）→ 断点 → abort。弹窗不行（破坏 Runtime 独立性）；堆栈是进阶项（平台相关，需栈回溯+符号解析）。

### 2. 宏的本质

**Q: 为什么不直接 if，要 do-while？**
A: 防悬空 else——宏里的 if 会抢走调用处 else。

```cpp
// 假设宏不用 do-while，而是 if 开头：
#define ASSERT(x) if (!(x)) { 报错; }

if (player)
    ASSERT(hp > 0);
else
    heal();   // else 错配到宏里的 if(!(hp>0))，而不是 if(player)
```

**Q: 为什么必须用宏？**
A: 两个目的：捕获调用点（宏在调用点文本替换）+ 关闭时参数不求值。函数做不到。

**Q: C# 的 Log.Warning() 有消耗吗？**
A: `[CallerFilePath]` 拿行号是编译期、无消耗；但参数求值只有靠 `[Conditional]` 才能让编译器移除调用。C# 是"编译器特性自动化了 C++ 宏手动做的事"，C++ 没有这些特性才退回宏。

### 3. 位置与格式化

**Q: 怎么获取文件名/函数名/行号？**
A: `__FILE__`/`__LINE__`/`__FUNCTION__` 预定义宏（必须在调用点展开），或 C++20 `std::source_location`（默认参数自动捕获，更干净）。

**Q: printf 和 cout 哪个好？**
A: 都不最优。格式化用 `std::format`（类型安全+简洁），输出用 printf 的 `%.*s`。

**Q: 需要 {0} 这种吗？**
A: 不需要，`{}` 按位置自动对应。`{0}` 只在"重复用参数/打乱顺序"时才要。两者不能混用。

### 4. 日志架构

**Q: sink 是什么？**
A: 数据流的"汇"（水槽）。source（产生日志）→ sink（日志去向），解耦"产生"与"去向"，换 sink 就能让日志流到控制台/内存/文件。

**Q: 先写 Log 还是 Assert？**
A: 先 Log（Assert 依赖 Log 报错）。类比：先铺电线（Log）再装灯（Assert）。

**Q: UE 风格还是 Unity 风格？**
A: UE 风格（宏+级别参数）。C++ 没有 C# 的 CallerMemberName 特性，被迫用宏。

**Q: 目录怎么分类？**
A: 一个头文件一个职责，Assert 和 Log 平级放 `arpg/` 下。改路径不麻烦，关键是养成 `<arpg/xxx.hpp>` 完整路径 include 的习惯。

## 工业实现对照

Task 2 完成后再做（UE 的 UE_LOG / check / ensure / verify 对照）。

## 最终决定

- 断言零开销：`NDEBUG` 预处理裁掉
- 位置捕获：`std::source_location`（C++20）
- 格式化：`std::format`（类型安全）
- 入口风格：UE 风格宏（级别作参数）
- 可测试性：`ILogSink`（日志）+ `AssertHandler`（断言，后续加）

## 下一步

1. 清理 `main.cpp` 里的验证断言
2. 写 Log：`LogLevel` → `ILogSink` → `LogMessage` → `ARPG_LOG` 宏
3. Assert 升级：cout 换成日志、拆 `ReportAssertFailure` 函数、加 `AssertHandler`

## Git Commit

未提交。

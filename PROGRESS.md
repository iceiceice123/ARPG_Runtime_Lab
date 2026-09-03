# ARPG Runtime Lab — PROGRESS.md

**版本：v1.3**  
**用途：项目恢复 / 当前进度 / 下一步**

> 这是项目的“存档点”。每次开发结束后更新。  
> 日历日期不是硬性进度，项目使用 Task Unit。

# 当前状态

## 阶段
**Month 1 — Week 1 / Task 2（Log / Assert）**

## 当前版本
- 项目总纲：v1.3
- 前三个月计划：v1.3
- 执行方式：Task Unit
- 工程骨架：v0.1.0（已完成，Task 1）

## 当前核心路线

```text
独立 Runtime
    ↓
数据组织实验
    ↓
ARPG Combat Sandbox
    ↓
Spatial / AI / Job
    ↓
Unreal Bridge
    ↓
规模化 Simulation
```

# 已完成的规划工作

- [x] 明确 ARPG 是底层能力训练载体
- [x] 明确同时追求游戏和 Runtime
- [x] 选择 Unreal Engine 5 + C++
- [x] 确定 Runtime 尽量独立于 Unreal
- [x] 确定 ECS 不预设为最终架构
- [x] 确定 Month 1 比较多种数据组织方案
- [x] 确定 A 类事实 / B 类建议分离
- [x] 将 91 天改成 91 个 Task Unit
- [x] 将 Unreal 深度接入后置
- [x] 将 PoE 内容复刻降级为极小玩法切片
- [x] 强化“自己实现 → Benchmark → 工业实现对照”
- [x] 提前引入 Memory / Allocator / Pool
- [x] 建立项目状态文件体系

# 进度

## 环境
- [x] 初始化 Git
- [x] 验证 C++20
- [x] 验证 CMake（3.29.5，VS2022 自带）
- [x] 安装 GoogleTest（v1.15.2，FetchContent）
- [x] 安装 Google Benchmark（v1.9.1，FetchContent）
- [ ] 安装 / 验证 Tracy 或等价工具
- [ ] 安装 RenderDoc
- [ ] 验证 Unreal Engine 5 Source Build
- [x] 建立 Runtime / Tests / Benchmarks / Apps / Docs / UnrealClient

## Month 1
- [ ] Game Loop
- [ ] Fixed Timestep
- [ ] Frame Spike
- [ ] Entity 生命周期
- [ ] Generation
- [ ] Free List
- [ ] Memory 初步实验
- [ ] AoS
- [ ] SoA
- [ ] Sparse Set
- [ ] 轻量 ECS 原型
- [ ] Query
- [ ] System
- [ ] Benchmark
- [ ] 工业实现对照
- [ ] 最终选择数据组织方案

## Month 2
- [ ] Player
- [ ] Movement
- [ ] Input → Command
- [ ] Command Replay
- [ ] Projectile
- [ ] Hit
- [ ] Damage
- [ ] Command / Event / State
- [ ] 事件顺序与确定性测试
- [ ] Buff
- [ ] Death
- [ ] 简单 Loot
- [ ] Combat Sandbox
- [ ] World State 序列化

## Month 3
- [ ] Spatial Query
- [ ] Spatial Hash
- [ ] AI
- [ ] AI Tick Rate
- [ ] Simulation LOD
- [ ] Thread Pool
- [ ] Task
- [ ] Worker
- [ ] Scheduler
- [ ] 基本任务依赖
- [ ] Parallel Simulation
- [ ] Stress Scene
- [ ] Unreal RuntimeBridge
- [ ] 工业实现对照

# 当前阻塞

暂无。

# 当前 Benchmark

Task 1 基线（完整数据在 `Docs/tasks/task01.md`）：

```text
Hardware : 20 X 2918 MHz，L1d 48 KiB，L2 1280 KiB，L3 24576 KiB
Build    : MSVC 19.36.32546.0，RelWithDebInfo，Ninja

BM_EmptyLoop                0.706 ns  (CPU 0.419 ns)
BM_VersionString            0.764 ns  (CPU 0.558 ns)
BM_SequentialSum/1024         445 ns  bytes_per_second=13.67 Gi/s
BM_SequentialSum/1048576   473359 ns  bytes_per_second=10.18 Gi/s
Conclusion : 基准链路可用；顺序遍历带宽随数据超出 cache 从 13.7 降到 10.2 Gi/s，
             作为 Week 3 cache locality 实验的对照基线。
```

每次记录必须包含：

```text
Workload
Hardware
Build
Before
After
Conclusion
```

# 当前 Git

已初始化（`git init -b main`）。尚未提交首次 commit。

# 下一步：Task 2

## 目标
实现最小日志和断言。

## 工作内容
1. 分级日志（Verbose / Warning / Error）
2. 断言宏：开发期启用，Release 构建零开销
3. Sink 可替换（stdout / 内存 / 回调），便于测试捕获
4. 确认 Runtime 日志不依赖 `UE_LOG`

## 完成标准

```text
Runtime 不依赖 Unreal 也能输出诊断信息
Release 构建中断言零开销
日志内容可被测试捕获并断言
```

# 每个 Task 的记录模板

```markdown
## Task XX — 标题

### 任务目的

### 今天要解决的问题

### 实现内容

### 新增 / 修改文件

### Unit Test

### Benchmark

### 遇到的问题

### 学到的东西

### 工业实现对照

### 最终决定

### 下一步

### Git Commit
```

# 每周复盘模板

```markdown
# Week XX Review

## 本周完成

## 最重要的技术理解

## Benchmark

## 架构变化

## 错误 / 走弯路

## 工业实现对照

## 下周 Task

## 路线是否需要调整
```

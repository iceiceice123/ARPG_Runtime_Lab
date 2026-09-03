# ARPG Runtime Lab — KNOWLEDGE_GUIDE.md

**用途：区分事实、原始资料、实验结果和项目建议。**

# A 类：事实 / 原始资料

A 类回答：

> 现实世界里到底是什么？

要求尽量来自可追溯的原始或权威资料。

## A1 — 基础原理

包括：

- C++ object lifetime
- RAII
- Move / Copy
- Memory layout
- CPU Cache
- Cache Line
- Memory hierarchy
- Thread / Mutex / Atomic
- Task Scheduling
- Serialization
- GPU 基础

优先来源：

- 标准
- 官方技术资料
- 权威教材
- 厂商资料

## A2 — 行业原始资料

包括：

- GDC
- 开发团队技术分享
- 论文
- 官方工程文章
- 开发者访谈

注意：

> 一个团队使用某技术，不等于所有项目都应该使用。

## A3 — 工业实现

用于：

> 成熟商业引擎具体是怎么实现的？

包括：

- Unreal Engine Source
- MassEntity
- Tasks
- Renderer
- RHI
- Networking

阅读时明确：

> 这是 Unreal 的选择，不是唯一正确答案。

# B 类：项目设计建议

B 类回答：

> 对 ARPG Runtime Lab，这样做是否更合适？

例如：

- 前两个月 Runtime First
- UE 深度接入后置
- 91 天改 Task Unit
- Month 1 比较多种数据组织方案
- 前期单机
- PoE 内容保持最小切片
- 提前加入 allocator / pool
- 每个主题必须做工业实现对照

这些是针对当前项目的工程建议，不是行业标准。

# C 类：实验结论

C 类记录：

> 我们自己测出来什么？

例如：

```text
SoA 比 AoS 快 18%。

条件：
100K entities
Release build
特定 workload

结论：
当前 Movement workload 更适合 SoA。
```

这是项目实验事实，不应直接推广为普遍真理。

# 推荐流程

```text
问题
 ↓
A1：基础原理
 ↓
A2：行业案例
 ↓
自己做最小实现
 ↓
C：Benchmark / Experiment
 ↓
A3：工业实现
 ↓
B：针对项目做决定
 ↓
DECISIONS.md
```

# 示例：ECS

不要：

```text
某游戏用 ECS
↓
所以我们用 ECS
```

应该：

```text
A2：
某游戏公开使用 ECS。

A1：
理解 ECS 背后的数据组织思想。

自己实现：
做一个小型 ECS。

C：
Benchmark。

A3：
研究 UE Mass。

B：
决定我们的 Runtime 是否采用 ECS。
```

# 重要原则

> 不因为“大厂用了 X”而使用 X。

> 不因为“自己写过 X”就认为 X 一定正确。

> 任何重要架构都应该允许被 Benchmark 推翻。

> 学习目标不是背答案，而是获得做技术判断的能力。

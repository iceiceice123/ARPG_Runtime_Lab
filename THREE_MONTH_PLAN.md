# ARPG Runtime Lab — 前三个月任务手册 v1.3

**执行单位：Task Unit**  
**预计周期：约 3 个月，不设置硬性日历期限**

> 一个 Task 可以花一天，也可以花数天。完成标准是“理解 + 实现 + 验证”，而不是准时翻页。

# 使用方式

每个任务：

```text
理解问题
 ↓
实现
 ↓
测试
 ↓
Benchmark
 ↓
总结
```

每个大主题结束：

```text
工业实现对照
```

时间不足时：

```text
代码 / 实验
>
阅读
```

# Month 1 — 数据组织与 Runtime 基础

## 本月问题

> 如何表示、创建、销毁、查询和更新大量游戏对象？

**不提前决定 ECS。**

## Week 1 — Game Loop / Time

### Task 1 — 工程骨架
建立 Runtime、Tests、Benchmarks、Docs。

完成标准：三个 target 能 build。

### Task 2 — Log / Assert
实现最小日志和断言。

完成标准：Runtime 不依赖 Unreal 也能输出诊断信息。

### Task 3 — Clock
区分 Wall Clock、Frame Time、Simulation Time。

### Task 4 — 最小 Game Loop
实现：

```text
Input
 ↓
Simulation
 ↓
Output
```

### Task 5 — Fixed Timestep
实现 accumulator + fixed step。

实验 30 / 60 / 144 FPS 下 Simulation 的稳定性。

### Task 6 — Frame Spike
制造 100ms 级卡顿。

研究追赶策略和 MaxTicksPerFrame。

### Task 7 — Benchmark / Profiler 基础
建立 Empty Tick、基础 workload benchmark 和 PROFILE_SCOPE。

### Week 1 Review
写 `Docs/week1_review.md`，用自己的话解释 Fixed Timestep、Frame Spike 和 Simulation/Render 分离。

## Week 2 — Entity 生命周期

### Task 8 — Entity Handle
比较 Pointer、Integer ID、Handle。

### Task 9 — Index + Generation
解决删除后旧引用误操作新对象的问题。

### Task 10 — EntityManager
实现 CreateEntity、DestroyEntity、IsAlive。

### Task 11 — Free List
避免高频 new/delete。

### Task 12 — Stress Test
随机 Create / Destroy / Reuse，运行 10 分钟。

### Task 13 — Benchmark
测试 1K / 10K / 100K 的创建与销毁。

### Task 14 — 工业实现对照
研究 UE MassEntity 实体管理相关实现，写 3 点相同、3 点不同、1 个设计取舍。

## Week 3 — 数据布局与内存

### Task 15 — Component 数据
定义 Position、Velocity、Health。

### Task 16 — AoS
实现结构数组，并 benchmark。

### Task 17 — SoA
实现分离数组，并 benchmark。

### Task 18 — Cache Locality
通过 profiler 观察连续访问和随机访问的区别。

### Task 19 — Object Pool
实现 Pool<T>，用于高频对象。

### Task 20 — Arena / Linear Allocator
实现 Allocate(size) + Reset() 的最小 Arena。

### Task 21 — 接入 Runtime
让 Pool / Arena 真正服务 EntityManager 或其他高频系统。

### Task 22 — Sparse Set
实现最小 Sparse Set，理解 Entity → Component 映射。

## Week 4 — ECS 作为实验对象

### Task 23 — 轻量 ECS Prototype
最小 Entity / Component / Query / System。

### Task 24 — Query
实现 Query<Position, Velocity>。

### Task 25 — Movement System
`Position += Velocity * dt`。

### Task 26 — 多方案 Benchmark
统一 workload 比较：

- OOP / AoS
- SoA
- Sparse Set
- ECS Prototype

### Task 27 — 架构选择
决定当前 ARPG 最适合的方案。可以是混合方案，也可以暂不定型。

### Task 28 — UE 数据组织对照
研究 Mass 的 Archetype / Chunk 风格，并比较你的方案。

### Task 29 — 独立 Runtime Demo
Runtime 脱离 Unreal 也能 Spawn / Update / Destroy。

### Task 30 — Month 1 Review
必须回答：

1. Entity 在本项目里解决什么问题？
2. 当前数据怎么组织？
3. 为什么选择当前方案？
4. Memory 谁负责？
5. 最大性能瓶颈是什么？
6. UE 的实现与我的方案有什么差异？

# Month 2 — ARPG Combat Sandbox

## 本月问题

> 如何让 Runtime 支撑一个真正可玩的 ARPG 战斗循环？

## Week 5 — Player / Command

### Task 31
建立 Player Entity。

### Task 32
实现 MoveCommand。

### Task 33
建立：

```text
Input
 ↓
Command
 ↓
Runtime
```

### Task 34
Movement 与 Player 接起来。

### Task 35
做最小 Unreal Input Bridge 原型。

### Task 36
Debug Draw 玩家和移动结果。

### Task 37 — Command Replay
记录一段 Move / Attack Command 序列，重新注入，比较最终世界状态。

完成标准：相同 Command 序列得到相同世界状态。

## Week 6 — Projectile

### Task 38
Projectile 数据：Owner / Position / Velocity / Lifetime / Damage。

### Task 39
Projectile Spawn。

### Task 40
Projectile Movement。

### Task 41
Projectile Lifetime。

### Task 42
基础命中。

### Task 43
压力测试 100 / 1K / 10K / 100K Projectiles，记录 CPU、Memory、Allocation。

## Week 7 — Combat

### Task 44
AttackCommand。

### Task 45
HitEvent。

### Task 46
Damage System。

### Task 47
Resistance。

### Task 48
明确 Command / Event / State。

### Task 49 — 事件顺序与确定性测试
定义 Event 消费顺序规则，同一 Tick 内固定顺序，写 deterministic test。

完成标准：相同输入多次运行得到相同结果。

### Task 50
Death Event。

### Task 51
Monster Destroy / Recycle。

## Week 8 — Buff / Loot / Sandbox

### Task 52
Buff 数据模型。

### Task 53
Poison。

### Task 54
Burn。

### Task 55
Slow / Stun。

### Task 56
Buff Update System。

### Task 57
简单 Loot Entity。

### Task 58
串起：

```text
Move
→ Attack
→ Projectile
→ Hit
→ Damage
→ Buff
→ Death
→ Loot
```

### Task 59
Combat Stress Test。

### Task 60 — Combat Sandbox
玩家能够移动、攻击、杀怪、获得掉落。

### Task 61 — World State 序列化
把 World State 序列化成字节流，再反序列化还原，最小验证。

完成标准：序列化前后世界状态一致，为回放 / 网络打基础。

Month 2 Review：

- Command / Event / State 为什么分？
- 高频 Projectile 的内存策略是什么？
- Buff 为什么这样组织？
- 游戏功能有没有反向污染 Runtime？
- 确定性（determinism）是否成立？哪些地方破坏了它？
- 序列化的最小边界划在哪里？

# Month 3 — Spatial / AI / Job

## 本月问题

> 大量实体同时存在时，如何让 Simulation 仍然可控？

## Week 9 — Spatial

### Task 62
先做暴力查询并记录真实表现。

### Task 63
Uniform Grid。

### Task 64
Spatial Hash。

### Task 65
统一 QueryRadius / QueryAABB 接口。

### Task 66
Benchmark 1K / 5K / 10K / 50K。

### Task 67
工业实现对照：UE Mass 的空间查询相关实现。

## Week 10 — AI

### Task 68
状态：Idle / Detect / Chase / Attack / Dead。

### Task 69
利用 Spatial Query 找目标。

### Task 70
AI Tick Rate。

### Task 71
Distance-based Simulation LOD。

### Task 72
500 / 1000 / 5000 AI Stress Test。

### Task 73
记录 AI CPU 成本和质量变化。

## Week 11 — Job System

### Task 74
先回答：为什么需要 Task，而不是开很多 Thread？

### Task 75
Thread Pool。

### Task 76
Task。

### Task 77
Worker。

### Task 78
Task Queue。

### Task 79
Movement 并行。

### Task 80
部分 AI 并行。

### Task 81
Task Dependency。

### Task 82
Atomic / Fence / Event。

### Task 83
False Sharing 实验。

### Task 84
Work Stealing 实验。

### Task 85
Benchmark：

```text
Single Thread
Thread Pool
Task System
```

### Task 86
UE Tasks 对照，记录相同点、不同点和设计取舍。

## Week 12 — Stress / Bridge

### Task 87
创建固定 Stress Scene。

### Task 88
目标规模：

```text
Monster × 5000
Projectile × 10000
Buff × 20000
Combat Events × 1000 / frame
```

### Task 89
Simulation Profiler。

### Task 90
Memory Profiler。

### Task 91
RuntimeBridge：UE 调用 `Runtime.Tick()`。

### Task 92
Runtime → Render State。

### Task 93
UE Presentation：只表现需要表现的实体。

### Task 94 — 三个月毕业任务

建立一个可运行的 Stress ARPG：

- 玩家可以移动
- 玩家可以攻击
- 怪物可以行动
- Projectile 可以飞行并命中
- Damage / Buff / Death / Loot 成立
- Runtime 可独立测试
- Runtime 可独立 Benchmark
- Runtime 可由 Unreal 展示

输出：

`Docs/three_month_review.md`

必须回答：

1. 最终数据组织方案是什么？
2. 为什么？
3. 有没有使用 ECS？
4. 哪些地方没有使用 ECS？
5. Memory Strategy 是什么？
6. Job System 有什么局限？
7. Spatial Query 的瓶颈是什么？
8. Unreal Bridge 的边界是什么？
9. 下一阶段最重要的问题是什么？

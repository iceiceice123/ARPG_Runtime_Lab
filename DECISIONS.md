# ARPG Runtime Lab — DECISIONS.md

**版本：v1.3**

> 决策可以演进。推翻旧决策时不要删除，建立新 Decision 并说明替代关系。

# Decision 001 — 项目不是“PoE 复刻项目”

**状态：Accepted**

ARPG 是底层能力训练的载体。内容必须克制。

# Decision 002 — Unreal Engine 5 + C++

**状态：Accepted**

Unreal 用于 Presentation、Tooling、Platform 和工业实现参考，不替代自己的核心 Simulation。

# Decision 003 — Runtime 尽量独立于 Unreal

**状态：Accepted**

```text
Runtime ← RuntimeBridge ← UnrealClient
```

Runtime 应能独立编译、测试、Benchmark、运行。

# Decision 004 — 不预设 ECS

**状态：Accepted**

候选：

- OOP / 对象模型
- AoS
- SoA
- Sparse Set
- ECS
- Archetype / Chunk

Month 1 先实验、Benchmark，再决定。

# Decision 005 — 91 天改为 91 个 Task Unit

**状态：Accepted**

完成“理解 + 实现 + 验证”优先于日历进度。

# Decision 006 — Unreal 深度接入后置

**状态：Accepted**

```text
Task 1～34
Runtime First

Task 35～36
最小 Input Bridge 原型 + Debug Draw

Task 37～90
继续 Runtime 为主（Combat / Spatial / AI / Job / Stress）

Task 91～93
正式 RuntimeBridge + Render State + Presentation

Task 94
三个月毕业任务
```

# Decision 007 — PoE 内容保持最小切片

**状态：Accepted**

第一版只做：

```text
移动
攻击
Projectile
Hit
Damage
死亡
Loot
```

# Decision 008 — A / B 知识分离

**状态：Accepted**

A = 事实 / 原始资料。  
B = 针对本项目的建议。

# Decision 009 — A 类进一步分 A1 / A2 / A3

**状态：Accepted**

```text
A1：基础原理
A2：行业原始资料
A3：工业实现
```

学习顺序：

```text
原理
→ 自己实现
→ Benchmark
→ 工业实现
→ 比较
```

# Decision 010 — 每个主题做工业实现对照

**状态：Accepted**

至少回答：

1. 我的问题是什么？
2. 我的方案是什么？
3. Benchmark 怎么样？
4. 工业实现怎么做？
5. 三点相同
6. 三点不同
7. 为什么可能存在这些差异？
8. 我是否需要修改自己的方案？

# Decision 011 — 提前加入 Memory

**状态：Accepted**

从 Entity / Data Layout 阶段开始引入：

- Pool
- Free List
- Arena
- Frame Allocation

# Decision 012 — Benchmark 是核心流程

**状态：Accepted**

不接受“感觉更快”。

记录：

```text
Workload
Hardware
Build
Before
After
Conclusion
```

# Decision 013 — 固定 Stress Scene

**状态：Accepted**

目标规模：

```text
Monster × 5000
Projectile × 10000
Buff × 20000
Combat Events × 1000 / frame
```

# Decision 014 — 不把“自己写”变成宗教

**状态：Accepted**

自己实现核心系统是为了理解，不是为了拒绝 STL 或成熟基础设施。

# Decision 015 — Source Reading 问题驱动

**状态：Accepted**

```text
问题
→ 自己设计
→ 自己实现
→ 测量
→ 找工业实现
→ 比较
→ 记录
```

# Decision 016 — 游戏与底层必须双闭环

**状态：Accepted**

每个阶段至少同时产出：

```text
技术成果
+
游戏成果
```

# 新决策模板

```markdown
# Decision XXX — 标题

状态：Proposed / Accepted / Superseded

## 背景

## 问题

## 候选方案

### A

### B

### C

## 实验 / Benchmark

## 决策

## 原因

## 影响

## 未来如何验证
```

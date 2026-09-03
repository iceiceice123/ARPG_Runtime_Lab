# ARPG Runtime Lab — PROJECT.md

**版本：v1.3**  
**状态：正式开工前总纲**

## 1. 项目定位

这是一个长期个人项目：

> 以制作一款 3D ARPG 为载体，系统训练底层游戏开发能力。

最终同时追求：

1. 一款真正可以玩的 ARPG。
2. 一套自己设计、自己实现、自己维护的 Game Runtime。

ARPG / PoE-like 是训练载体，不是全部目标。

## 2. 核心目标

优先级：

1. 底层能力成长
2. 游戏真正可玩
3. 画面质量

目标不是重写 Unreal，而是从“主要调用引擎 API 的客户端程序员”逐渐成长为能够理解、设计、实现和验证 Runtime 核心系统的程序员。

## 3. 技术路线

### 核心
- Unreal Engine 5
- C++20
- CMake
- Git

### 工具
- Visual Studio / Rider
- GoogleTest
- Google Benchmark
- Tracy / Visual Studio Profiler
- RenderDoc
- 后期按需加入 PIX、AddressSanitizer、UndefinedBehaviorSanitizer 等

## 4. Runtime 是什么

本项目中的 Runtime 指：

> 负责维护游戏世界状态，并让世界随着时间推进而发生变化的程序部分。

例如：

```text
Entity
Position
Velocity
Health
Buff
Target
Skill
```

以及：

```text
MovementSystem
CombatSystem
BuffSystem
AISystem
SpatialSystem
```

Runtime 可以独立运行，不需要知道 Unreal 是什么。

## 5. Runtime 与 Unreal 的边界

目标架构：

```text
                   Unreal Client
                         │
                  Runtime Bridge
                         │
                         ▼
                   My Runtime
                         │
          ┌──────────────┼──────────────┐
          │              │              │
         Data        Simulation        Job
          │              │              │
        Memory        Combat            AI
                       │
                    Spatial
```

依赖方向：

```text
Runtime ← RuntimeBridge ← UnrealClient
```

Runtime 尽量不依赖：

```cpp
AActor
UObject
UWorld
UActorComponent
Niagara
UMaterial
```

Unreal 主要负责：

- Window
- Input
- Camera
- Mesh
- Animation
- VFX
- Audio
- Asset
- Editor
- 基础 Rendering
- Platform / Tooling

## 6. 第一版游戏

第一版是一个极小的 3D ARPG 切片：

- 单角色
- 单地图
- 一种怪物
- 一种攻击技能

核心循环：

```text
进入地图
 ↓
玩家移动
 ↓
发现怪物
 ↓
释放技能
 ↓
Projectile 飞行
 ↓
命中
 ↓
Damage
 ↓
怪物死亡
 ↓
掉落简单物品
```

画面可以非常简陋：Cube、Sphere、Capsule、Debug Line、Debug Text。

## 7. PoE 的定位

PoE 只是玩法灵感和问题背景。

前三个月不做：

- Passive Tree
- 大量 Skill Gem
- 大量词缀
- 复杂 Build
- 大规模内容生产

原则：

> 守住最小玩法切片，不让内容膨胀吞掉底层训练时间。

## 8. 不预设 ECS

ECS 是候选方案，不是最终答案。

Month 1 真正的问题：

> 如何组织和高效更新大量游戏对象？

候选：

- 传统 OOP / 对象模型
- AoS
- SoA
- Sparse Set
- 轻量 ECS
- Archetype / Chunk 风格存储

通过性能、内存、灵活性、复杂度、可维护性和 Benchmark 决定。

## 9. 知识分类

### A 类：事实 / 原始资料

来自：

- Unreal 官方文档
- Unreal Engine 源码
- C++ 标准与权威资料
- GDC / 开发者技术分享
- 学术论文
- CPU / GPU 厂商官方资料

要求：可追溯、可验证。

### B 类：项目设计建议

针对本项目的工程建议，例如：

- 前期单机
- Month 1 比较多种数据布局
- UE 深度接入后置
- 先自己实现简化版
- 建立固定 Stress Scene

这些不应描述成行业标准答案。

## 10. A 类的三层

### A1：基础原理
CPU、Cache、C++ 生命周期、并发、GPU 等。

### A2：行业原始资料
GDC、技术分享、论文、开发者资料等。

### A3：工业实现
Unreal Source、MassEntity、Tasks、Renderer、RHI、Networking 等。

推荐学习顺序：

```text
原理
 ↓
自己实现
 ↓
Benchmark / Experiment
 ↓
工业实现
 ↓
比较
 ↓
形成自己的结论
```

## 11. 时间管理

“91 天”解释为：

> 91 个 Task Unit，而不是 91 个必须按日历完成的天。

一个任务可以花一天，也可以花数天。

优先级：

```text
理解 + 实现 + 验证
>
按日历准时完成
```

## 12. Unreal 接入节奏

```text
Task 1～34
基本完全独立 Runtime

Task 35～36
最小 Unreal Input Bridge 原型 + Debug Draw

Task 37～90
继续纯 Runtime 为主（Combat / Spatial / AI / Job / Stress）

Task 91～93
正式 RuntimeBridge + Render State + UE Presentation

Task 94
三个月毕业任务
```

## 13. 内存提前进入

Memory 不晚到 Month 4。

在 Entity / Data Layout 阶段逐渐引入：

- Object Pool
- Free List
- Arena / Linear Allocator
- Frame Allocation

关注：

> 内存是谁的？什么时候分配？什么时候释放？为什么需要池？碎片从哪里来？

## 14. 工业实现对照

每个大主题结束都做：

```text
1. 我的问题是什么？
2. 我做了什么方案？
3. Benchmark 怎么样？
4. 工业实现怎么做？
5. 三点相同
6. 三点不同
7. 为什么存在差异？
8. 我是否需要修改自己的方案？
```

## 15. 性能原则

性能必须有数字。

至少记录：

```text
Workload
Hardware
Build Configuration
Before
After
Conclusion
```

## 16. 固定 Stress Scene

逐步达到：

```text
Monster × 5000
Projectile × 10000
Buff × 20000
Combat Events × 1000 / frame
```

观察：

```text
Simulation
Movement
AI
Spatial
Combat
Jobs
Memory
Allocations
Entity Count
```

## 17. 开发流程

```text
问题
 ↓
设计
 ↓
最小实现
 ↓
Unit Test
 ↓
集成
 ↓
Benchmark
 ↓
Profiler
 ↓
Stress Test
 ↓
工业实现对照
 ↓
Documentation
 ↓
Refactor
```

## 18. 成功标准

不是“用了 ECS”，也不是“做了多少 PoE 内容”。

真正的毕业标准：

> 面对一个游戏系统，能够解释问题、提出多个方案、实现简化版本、测量结果、阅读工业实现，并做出有理由的技术选择。

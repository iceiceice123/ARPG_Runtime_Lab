# ARPG Runtime Lab — AI 协作规则

## 项目定位

以「复刻 PoE 玩法切片」为载体的底层 Game Runtime 训练项目。
目标：从"API 使用者"成长为"系统实现者"。
ARPG / PoE-like 是训练载体，不是全部目标。

## 文档索引（遇到项目问题先读）

- `PROJECT.md`           — 总纲、目标、技术路线
- `THREE_MONTH_PLAN.md`  — 94 个 Task 的详细计划
- `DECISIONS.md`         — 架构决策记录（ADR）
- `KNOWLEDGE_GUIDE.md`   — 知识分类（A1/A2/A3/B/C）
- `PROGRESS.md`          — 进度存档点

## 技术栈

UE5（仅表现层）+ C++20 + CMake + GoogleTest + Google Benchmark + Git

## 必须遵守的规则

1. **Runtime 不依赖 Unreal**：`Runtime/` 目录禁止 include 任何 UE 头文件（`Engine/`、`CoreMinimal.h`、`UObject/` 等）。UE 可以调用 Runtime，Runtime 不反向依赖 UE。
2. **不预设 ECS**：数据方案（OOP/AoS/SoA/Sparse Set/轻量 ECS）必须用 benchmark 数据对比后决定，结论必须来自数据。
3. **性能结论必须有数字**：任何"更快/更慢"都要附 benchmark 数据（Workload / Hardware / Build / Before / After / Conclusion）。
4. **每个主题结束做工业实现对照**：读 UE 源码（Mass、Tasks、Renderer 等），写 3 点相同、3 点不同、为什么存在差异。
5. **代码/实验优先于文档和阅读**：时间不足时先写代码，文档和阅读可顺延。
6. **内容克制**：只做最小玩法切片（移动/攻击/Projectile/Hit/Damage/Buff/死亡/Loot），不做 PoE 技能树、词缀、Passive Tree。
7. **进度用 Task Unit**：完成标准是"理解 + 实现 + 验证"，不按日历赶进度。

## 协作偏好

- 用简体中文回答。
- 写代码时始终保持 Runtime 独立性，不引入 UE 依赖。
- 涉及架构选择时，要求用 benchmark 数据说话，不接受"感觉更快"。
- 不确定的技术细节，优先查 UE 源码或官方文档，而非凭空猜。

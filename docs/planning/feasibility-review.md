# Merkuro 解耦重构 - 可行性评审与执行计划

> **最后更新**: 2026-01-06  
> **评审结论**: ✅ **完全可行** (14-22 周完成)

---

## 📊 核心发现

### 项目规模
```
代码总量:        9,645 行 C++
源文件数:        50+ 个文件
QML 文件:        51 个
构建系统:        CMake (现代化)
```

### 耦合度评分
```
Akonadi (日历存储)      : 9/10 ⚠️ 极强
Kirigami (UI 框架)      : 8/10 ⚠️ 强
KDE 其他库              : 5-7/10 中等
```

### 可行性评估
| 因素 | 评分 | 说明 |
|------|------|------|
| 代码规模适中性 | ✅ | 9.6K 行，适合重构 |
| 分层清晰度 | ✅ | UI → Model → Akonadi，边界明确 |
| 指南完整性 | ✅ | IMPLEMENTATION_GUIDE.md 质量高 |
| 向后兼容可能性 | ✅ | 可渐进式迁移 |
| 实施风险 | ⚠️ | Akonadi 转换复杂，需充分测试 |

**结论**: ✅ **推荐立即启动**

---

## 🏗️ 执行路线图

### 6 个 Phase，分阶段实施

| Phase | 名称 | 难度 | 工期 | 工作量 | 价值 |
|-------|------|------|------|--------|------|
| **0** | 准备 | ⭐⭐ | 1w | 中等 | 基础 |
| **1** | 核心库 | ⭐⭐ | 3-4w | 1K 行 | 🔴 极高 |
| **2** | Akonadi 适配 | ⭐⭐⭐ | 2-3w | 700 行 | 🔴 极高 |
| **3** | DI 容器 | ⭐⭐ | 1-2w | 300 行 | 🟠 高 |
| **4** | UI 抽象 | ⭐⭐⭐ | 3-4w | 1.5K 行 | 🟡 中 |
| **5** | 可选后端 | ⭐⭐⭐⭐ | 2-3w | 800 行 | 🟢 中 |
| **6** | 测试/文档 | ⭐⭐ | 2-3w | 1K 行 | 🔴 极高 |

**总计**: 14-22 周 (3-5 个月)

---

## 💎 核心价值承诺

### 1. Plasma Widget 支持
```
现状: 小组件启动 = 整个 Akonadi + Kirigami (太重)
之后: 小组件启动 = 核心库 + 简单 QML (轻!)
```

### 2. 跨平台能力
```
现状: 依赖 KDE 生态，Windows 几乎不可用
之后: 默认用 Akonadi，可选本地 ICS 或 Google Calendar
结果: Windows / macOS / 轻量 Linux 都能用
```

### 3. 测试自动化
```
现状: 手动点击测试，无法自动化
之后: 单元测试 >80% 覆盖，CI/CD 完全自动化
```

### 4. UI 框架灵活性
```
现状: Kirigami 唯一选择，难以更新
之后: Kirigami (默认) + Material + Fluent (可选)
```

---

## 🚀 两条执行路径

### 快速路径 (推荐) - 2-3 个月
**适合**: 1-2 个专职开发者，想快速产出核心价值

```
Week 1-2   : 准备
Week 3-6   : Phase 1 - 核心库 (独立可用！)
Week 7-9   : Phase 2 - Akonadi 适配 (可插拔！)
Week 10    : Phase 3 - DI 容器 (完整解耦！)

🎯 成果:
  ✅ 1 个独立的、可测试的核心库
  ✅ Akonadi 成为可选后端
  ✅ 完整的依赖注入系统
  ✅ 为 Plasma Widget 和多后端铺路
```

### 完整路径 - 5 个月
**适合**: 有专门团队，要生产级系统

```
Week 1-10  : 快速路径所有内容
Week 11-14 : Phase 4 - UI 抽象
Week 15-17 : Phase 5 - 可选后端
Week 18-20 : Phase 6 - 完整测试 & 文档

🎯 成果:
  ✅ 完全模块化系统
  ✅ 多 UI 框架支持
  ✅ 多数据后端支持
  ✅ 200+ 单元测试
  ✅ 完整的扩展文档
```

---

## 📋 详细的周间里程碑

### Pre-Phase (Week 1-2)
**交付物**:
- [ ] 新建目录结构: `src/core/`, `src/backends/`, `tests/`
- [ ] CMake 模块化配置
- [ ] 选择测试框架 (推荐 Google Test)
- [ ] 代码审视：标记所有 Akonadi 使用点

---

### Phase 1 (Week 3-6) 核心库
**交付物**:
```cpp
✅ src/core/models/
   ├── CalendarEvent.h      (80 行，0 个 KDE 依赖)
   ├── TodoItem.h
   ├── Recurrence.h
   └── CalendarCollection.h

✅ src/core/data/
   ├── ICalendarStorage.h   (80 行，接口)
   ├── IContactProvider.h
   └── ISyncManager.h

✅ src/core/operations/
   ├── EventOperations.h    (100 行)
   ├── TodoOperations.h
   └── MockCalendarStorage  (用于测试)

✅ src/core/utils/
   ├── RecurrenceCalculator.h
   ├── DateTimeUtils.h
   └── ICalParser.h

✅ tests/unit/
   ├── CalendarEventTest.cpp
   ├── RecurrenceTest.cpp
   └── DateTimeUtilsTest.cpp
```

**验收标准**:
- [ ] 0 个 KDE 头文件 #include
- [ ] 100% 单元测试通过
- [ ] 测试覆盖率 >80%

**里程碑**: 🎉 **核心库完全独立！**

---

### Phase 2 (Week 7-9) Akonadi 适配
**交付物**:
```cpp
✅ src/backends/akonadi/
   ├── AkonadiCalendarBackend.h/cpp
   │   ├── 继承 ICalendarStorage
   │   ├── 包装 Akonadi::ETMCalendar
   │   └── 实现所有虚函数
   ├── AkonadiDataConverter.h/cpp
   │   ├── Core::CalendarEvent ↔ KCalendarCore::Incidence::Ptr
   │   ├── 递归处理
   │   ├── 时区处理
   │   └── Attendees 处理
   └── AkonadiSyncManager.h

✅ tests/integration/
   └── AkonadiBackendTest.cpp
       ├── 创建事件
       ├── 修改事件
       ├── 删除事件
       └── 递归处理验证
```

**验收标准**:
- [ ] 与现有 calendarmanager.cpp 兼容
- [ ] 集成测试通过 (需要 Akonadi)
- [ ] 100% 公开接口被测试

**里程碑**: 🎉 **Akonadi 正式成为"可选后端"！**

---

### Phase 3 (Week 10) DI 容器
**交付物**:
```cpp
✅ src/core/ServiceContainer.h/cpp
   ├── getInstance() 单例
   ├── registerCalendarStorage()
   ├── getCalendarStorage()
   └── 线程安全访问

✅ 修改 src/main.cpp
   ├── 初始化 Akonadi 适配器
   ├── 注册到 ServiceContainer
   └── 暴露给 QML

✅ 修改 src/contents/ui/
   └── 使用 ServiceContainer 而非直接调用 Akonadi
```

**验收标准**:
- [ ] UI 代码不直接引用 Akonadi
- [ ] 可以在单元测试中注册 Mock 后端
- [ ] 可编译时选择不同后端

**里程碑**: 🎉 **完整的解耦架构！**

---

### Phase 4 (Week 11-14) UI 抽象 (可选)
**交付物**:
```qml
✅ src/ui/contracts/
   ├── IMonthView.qml
   ├── IWeekView.qml
   ├── IEventEditor.qml
   └── ... (其他接口)

✅ src/ui/kirigami/
   ├── KirigamiMonthView.qml
   ├── KirigamiWeekView.qml
   └── ... (Kirigami 实现)

✅ src/ui/UIFactory.qml
   └── 根据配置加载不同 UI 框架
```

**里程碑**: 🎉 **UI 框架不再是"必需"！**

---

### Phase 5 (Week 15-17) 可选后端 (可选)
**交付物**:
```cpp
✅ src/backends/local/
   └── LocalICalendarBackend.h/cpp
       ├── 从 ~/.config/calendar/ 读取 .ics
       ├── 保存修改
       └── 支持 CRUD 操作

✅ src/backends/cloud/
   └── GoogleCalendarBackend.h/cpp
       ├── OAuth2 认证
       ├── REST API 同步
       └── 完整的日期/时间支持
```

**里程碑**: 🎉 **应用可跨平台运行！**

---

### Phase 6 (Week 18-20) 测试 & 文档
**交付物**:
```
✅ tests/
   ├── unit/        (200+ 单元测试)
   ├── integration/ (50+ 集成测试)
   ├── coverage/    (>80% 覆盖率报告)
   └── performance/ (性能基准)

✅ docs/
   ├── ARCHITECTURE.md            (整体架构)
   ├── MIGRATION_GUIDE.md         (如何迁移代码)
   ├── BACKEND_IMPLEMENTATION.md  (实现新后端)
   ├── UI_FRAMEWORK_GUIDE.md      (添加新 UI)
   ├── API_REFERENCE.md           (API 文档)
   └── TROUBLESHOOTING.md         (问题排查)
```

**里程碑**: 🎉 **完整的生产级系统！**

---

## ⚠️ 关键风险与缓解

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| Akonadi 数据转换复杂 | 中 | 高 | 充分的集成测试、提前设计 |
| 现有代码难迁移 | 中 | 中 | 渐进式迁移、向后兼容 |
| 测试覆盖不足 | 高 | 高 | 前置测试框架、严格门槛 |
| 性能下降 | 低 | 中 | 性能基准测试、及时优化 |
| KDE API 变更 | 低 | 中 | 版本检查、适配层 |

---

## ✅ 立即行动清单

### 本周 (决策阶段)
- [ ] **确认是否启动**
  - [ ] 是 → 继续
  - [ ] 否 → 标记为"未来候选"
  - [ ] 有条件 → 先做 Phase 1 验证

- [ ] **选择执行路径**
  - [ ] 快速路径 (2-3 月，推荐)
  - [ ] 完整路径 (5 个月)

- [ ] **分配资源**
  - [ ] 后端开发: __ 人
  - [ ] 前端开发: __ 人  
  - [ ] 测试工程师: __ 人
  - [ ] 架构师/主管: __ 人

### 接下来 (准备阶段)
- [ ] 创建分支: `feature/decouple-architecture`
- [ ] 建立目录: `src/core/`, `src/backends/`, `tests/`
- [ ] 选择测试框架 (推荐 Google Test)
- [ ] 制定 Code Review 标准
- [ ] 安排 6 个 Checkpoint (Week 2, 6, 9, 14, 17, 20)

---

## 📚 参考文档

### 项目文档
- ✅ `IMPLEMENTATION_GUIDE.md` - 具体代码示例
- ✅ `ARCHITECTURE_ANALYSIS.md` - 当前架构分析
- 📄 本文档 - 可行性评审

### 学习资源
- **架构设计**: "Clean Architecture" (Robert C. Martin)
- **C++ 最佳实践**: "Effective C++" (Scott Meyers)
- **测试**: "Test Driven Development" (Kent Beck)
- **Qt 开发**: Qt 官方文档 + KDE 开发指南

---

## 🏁 结论

**✅ 这个项目完全适合进行架构解耦重构。**

通过 14-22 周的分阶段实施，Merkuro 将从"一个 KDE 日历应用"升级为：

1. 🎁 支持 Plasma Widget
2. 🌍 跨越 Windows/macOS/Linux
3. 🧪 完整的自动化测试能力
4. 🎨 多 UI 框架支持
5. 🔌 多数据后端支持
6. 📖 成为 KDE 应用解耦的范例

**建议**: 选择 **快速路径**（2-3 个月），先获得核心价值，再根据需要扩展。

**下一步**: 你的决策
- ✅ 确认启动？
- ✅ 分配资源？
- ✅ 开始 Phase 0？

---

## 📞 联系与支持

如果你确认要启动重构，我可以帮你：
- ✅ 详细设计 Phase 1 的代码结构
- ✅ 生成 Phase 1 的代码框架和模板
- ✅ 建立测试框架和 CI/CD 流程
- ✅ 编写详细的实现指南和最佳实践

**你准备好了吗？**

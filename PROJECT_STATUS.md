# Merkuro 解耦重构 - 项目状态

**最后更新**: 2026-01-06  
**状态**: 🚀 Phase 0 完成，Phase 1 进行中

---

## 📊 快速概览

| 指标 | 值 |
|------|-----|
| 总工期 | 10 周 (快速路径) |
| 当前进度 | Week 1/10 |
| 完成度 | 15% |
| 核心库代码 | 776 行 |
| 单元测试 | 10 个 |
| KDE 依赖 | 0 个 ✅ |

---

## ✅ Phase 0 (Week 1-2) - 完成

### 已完成
- [x] 目录结构创建
- [x] CalendarEvent 数据模型 (2 个文件, 224 行)
- [x] ICalendarStorage 接口 (1 个文件, 107 行)
- [x] EventOperations 业务逻辑 (2 个文件, 250 行)
- [x] 单元测试框架 (1 个文件, 195 行)
- [x] 编译验证 ✅ (零 KDE 依赖)

### 成果
```
src/core/
├── models/
│   ├── CalendarEvent.h        ✅
│   └── CalendarEvent.cpp      ✅
├── data/
│   └── ICalendarStorage.h     ✅
├── operations/
│   ├── EventOperations.h      ✅
│   └── EventOperations.cpp    ✅
└── CMakeLists.txt             ✅

tests/unit/
└── CalendarEventTest.cpp      ✅ (10 个测试)
```

---

## 🔄 Phase 1 (Week 3-6) - 进行中

### Week 3-4: 数据模型补全
- [ ] TodoItem 数据模型
- [ ] Recurrence 递归规则完善
- [ ] CalendarCollection 日历集合
- [ ] 相关单元测试

### Week 5: 工具类实现
- [ ] RecurrenceCalculator - 递归事件计算
- [ ] DateTimeUtils - 日期时间工具函数
- [ ] iCalendar 序列化/反序列化改进
- [ ] 相关单元测试

### Week 6: 测试和验收
- [ ] 单元测试覆盖率达到 >85%
- [ ] 完整的 Doxygen API 文档
- [ ] Phase 1 Checkpoint 验收

### 里程碑
目标: **完全独立的核心库，零 KDE 依赖**

---

## 📅 后续 Phase 计划

### Phase 2 (Week 7-9) - Akonadi 适配器
- [ ] AkonadiCalendarBackend 实现
- [ ] AkonadiDataConverter 数据转换
- [ ] 集成测试
- **里程碑**: Akonadi 成为可插拔的后端

### Phase 3 (Week 10) - 依赖注入容器
- [ ] ServiceContainer 实现
- [ ] main.cpp 修改
- [ ] QML 集成示例
- **里程碑**: 完整的解耦架构就位

### Phase 4-6 (可选，Week 11-20)
- **Phase 4**: UI 抽象 (支持多个 UI 框架)
- **Phase 5**: 可选后端 (本地 ICS、Google Calendar)
- **Phase 6**: 完整测试和文档

---

## 📚 文档索引

### 决策和规划
- **REFACTORING_FEASIBILITY_REVIEW.md** (9.7K)
  - 可行性评估
  - 6 个 Phase 概览
  - 风险和成功指标
  - 立即行动清单

### 详细执行计划
- **DETAILED_MILESTONE_PLAN.md** (26K)
  - Week 1-20 具体计划
  - 每周交付物
  - 代码框架示例
  - 验收标准

### 实现参考
- **IMPLEMENTATION_GUIDE.md** (9.6K)
  - 完整的代码示例
  - 接口设计
  - 迁移清单

### 背景信息
- **ARCHITECTURE_ANALYSIS.md** (7.4K)
  - 当前架构分析
  - 耦合度评分
  - 问题识别

---

## 💻 开发指南

### 编译核心库
```bash
# 简单编译（g++）
g++ -std=c++17 -fPIC -I. -c src/core/models/CalendarEvent.cpp
g++ -std=c++17 -fPIC -I. -c src/core/operations/EventOperations.cpp
ar cr libpersonalcalendar-core.a *.o

# 使用 CMake
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make personalcalendar-core
```

### 运行单元测试
```bash
# 需要 Google Test
cd build
make core-unit-tests
./core-unit-tests

# 或使用 ctest
ctest --verbose
```

### 验证零 KDE 依赖
```bash
nm libpersonalcalendar-core.a | grep -i kde
# 应该没有输出
```

---

## 🎯 关键决策

- ✅ **快速路径**: 2-3 个月完成核心功能
- ✅ **核心库优先**: Phase 1 是基础
- ✅ **Akonadi 可插拔**: Phase 2 将其变为后端
- ✅ **完整解耦**: Phase 3 完成架构分离
- ✅ **测试驱动**: 每个功能都有单元测试

---

## 📊 进度跟踪

### Week 1 ✅ 完成
- [x] Phase 0 所有任务完成
- [x] 核心库编译成功
- [x] 第一批单元测试通过

### Week 2 进行中
- [ ] 完成数据模型补全
- [ ] 增加单元测试覆盖
- [ ] Checkpoint 验收

### Week 3-6 计划中
- [ ] Phase 1 其他数据模型
- [ ] 工具类实现
- [ ] 测试覆盖到 >85%

---

## 💡 成功标志

**短期 (Week 6 后)**
```
✅ 核心库独立编译，0 个 KDE 依赖
✅ 100+ 个单元测试，覆盖率 >85%
✅ 完整的 API 文档
```

**中期 (Week 10 后)**
```
✅ Akonadi 成为可选后端
✅ 应用用 Mock 后端也能运行
✅ UI 代码不直接调用 Akonadi
```

**长期 (Week 20 后)**
```
✅ 支持多个 UI 框架
✅ 支持多个数据后端
✅ 完整的跨平台能力
```

---

## 🚀 下一步行动

### 本周 (Week 1)
1. ✅ 建立基础架构 (完成)
2. ✅ 编译验证 (完成)
3. ⏳ Code Review 和反馈

### 下周 (Week 2)
1. 完成其他数据模型 (TodoItem, Recurrence, etc.)
2. 实现工具类 (RecurrenceCalculator, DateTimeUtils)
3. 单元测试到 >80% 覆盖

### Week 2 末 Checkpoint
- [ ] Phase 1 核心代码 100% 完成
- [ ] 单元测试覆盖 >85%
- [ ] Akonadi 适配器设计完成
- [ ] 准备开始 Phase 2

---

## 📞 联系和反馈

对于：
- **整体方案**: 参考 REFACTORING_FEASIBILITY_REVIEW.md
- **具体步骤**: 参考 DETAILED_MILESTONE_PLAN.md
- **代码参考**: 参考 IMPLEMENTATION_GUIDE.md
- **架构疑问**: 参考 ARCHITECTURE_ANALYSIS.md

---

## 📈 项目指标

| 指标 | 目标 | 当前 | 进度 |
|------|------|------|------|
| 核心库代码行数 | 1,500 | 776 | 52% |
| 单元测试数量 | 100+ | 10 | 10% |
| 测试覆盖率 | >85% | TBD | ⏳ |
| 编译成功 | ✅ | ✅ | 100% |
| KDE 依赖 | 0 | 0 | 100% ✅ |

---

**状态**: 🚀 正在稳步推进  
**势头**: 📈 向上  
**下次更新**: Week 2 末

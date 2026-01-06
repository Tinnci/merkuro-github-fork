# Merkuro 代码耦合和解耦分析

## 📊 当前架构

### 层级结构
```
QML UI 层 (51 files)
    ↓
C++ 模型/数据层 (26 headers + 44+ cpp files)
    ↓
KDE Frameworks (Akonadi, CalendarCore, etc.)
    ↓
系统日历存储 (Akonadi Backend)
```

## 🔗 主要耦合点

### 1. **Akonadi 深度耦合** (最强)
- **文件**: calendarmanager.cpp/h, contactsmanager.cpp/h, agentconfiguration.cpp/h
- **依赖**: 
  - `Akonadi::ETMCalendar` - 事件三元模型日历
  - `Akonadi::IncidenceChanger` - 事件修改器
  - `Akonadi::CollectionFilterProxyModel` - 集合筛选代理
  - `Akonadi::AgentManager` - 代理管理（在线日历账户）
- **强度**: ⚠️⚠️⚠️ 极强
- **特点**: 无法直接用其他后端替换

### 2. **KDE UI 框架耦合** (强)
- **文件**: 几乎所有 QML 文件都使用
- **依赖**:
  - `org.kde.kirigami` - 移动/桌面自适应UI
  - `org.kde.kitemmodels` - 代理模型库
  - `org.kde.kalendar` - 自定义QML类型注册
- **强度**: ⚠️⚠️ 强
- **特点**: Kirigami是高度定制化的，难以替换

### 3. **KDE 其他框架耦合** (中等)
- **依赖**:
  - KLocalizedString - 多语言支持
  - KConfigWidgets - 配置UI
  - KDescendantsProxyModel - 代理模型
  - CalendarSupport - 日历辅助库
  - EventViews - 事件视图组件
- **强度**: ⚠️ 中等
- **可替换**: 部分可替换

## 🏗️ 核心组件分析

### 数据模型层 (C++)
```
CalendarManager
├── ETMCalendar (Akonadi)
├── IncidenceChanger (Akonadi)
├── Collection Models (Akonadi)
└── Event/Todo Models
    ├── TodoModel
    ├── MonthModel
    ├── HourlyIncidenceModel
    ├── IncidenceOccurrenceModel
    └── InfinitecalendarViewModel

ContactsManager
├── ContactSearchJob (Akonadi)
├── ContactsTreeModel (Akonadi)
└── EmailAddressSelectionModel

TagManager
├── Tag data storage

IncidenceWrapper
├── Event/Todo wrapper
└── Recurrence handling
```

### UI 层 (QML)
```
Main Application
├── MonthView
├── WeekView  
├── DayView
├── ScheduleView
├── TodoView (with TreeView)
├── IncidenceEditor
├── IncidenceInfo
└── Settings
```

### 集成点
```
KDE 特有的集成:
├── Akonadi Agent Configuration (在线日历)
├── Contact auto-completion (从KDE Contacts)
├── System color schemes (Kirigami)
└── Breeze icon set (KDE标准)
```

## 📈 解耦方案

### 阶段 1: 提取核心业务逻辑 (易度: ⭐⭐)
目标: 创建"Pure Core"库，0个KDE依赖

**提取目标**:
- 事件数据模型 (Event, Todo, Recurrence)
- 日历基础操作 (Create, Read, Update, Delete)
- 日期/时间计算 (Month view logic, etc.)
- 数据转换层

**新增文件**:
```
src/core/
├── models/
│   ├── CalendarEvent.h
│   ├── TodoItem.h
│   ├── Recurrence.h
│   └── CalendarCollection.h
├── operations/
│   ├── EventOperations.h
│   ├── TodoOperations.h
│   └── SyncManager.h
├── data/
│   ├── ICalendarStorage.h  (抽象接口)
│   └── IContactProvider.h  (抽象接口)
└── utils/
    ├── DateTimeUtils.h
    ├── RecurrenceCalculator.h
    └── ICalParser.h
```

**隔离Akonadi**:
```
src/backends/
├── akonadi/
│   ├── AkonadiCalendarBackend.h  (实现 ICalendarStorage)
│   ├── AkonadiContactProvider.h  (实现 IContactProvider)
│   └── AkonadiSyncManager.h
└── (未来: local/, cloud/, etc.)
```

### 阶段 2: 抽象Kirigami UI (易度: ⭐⭐⭐)
目标: 创建UI抽象层，支持多个UI框架

**方案**:
```
src/ui/
├── contracts/
│   ├── IMonthView.qml (接口)
│   ├── IEventEditor.qml (接口)
│   └── IMainWindow.qml (接口)
├── kirigami/
│   ├── KirigamiMonthView.qml (KDE实现)
│   ├── KirigamiEventEditor.qml
│   └── KirigamiMainWindow.qml
└── (未来: material/, fluent/, etc.)
```

### 阶段 3: 解耦KDE集成点 (易度: ⭐⭐⭐⭐)

**需要处理的KDE特有功能**:
1. 在线日历账户管理 (Akonadi Agents)
2. 联系人自动补全 (KDE Contacts)
3. 多语言支持 (KLocalizedString)
4. 配置存储 (KDE ConfigWidgets)
5. 主题/颜色 (Breeze icons, color schemes)

**解耦方式**:
```
对于每个KDE功能，创建接口:

IAccountManager
├── KDE实现: AkonadiAccountManager
├── 本地实现: LocalAccountManager
└── Cloud实现: CloudAccountManager

IContactProvider
├── KDE实现: KDEContactProvider
├── Fallback: EmptyContactProvider
└── Cloud实现: CloudContactProvider

ITranslationProvider
├── KDE实现: KDETranslationProvider
├── Fallback: EnglishOnlyProvider
└── (支持热插拔翻译引擎)

IThemeProvider
├── KDE实现: KDEThemeProvider
├── Fallback: DefaultThemeProvider
└── Custom实现: CustomThemeProvider
```

## 🎯 分流程设计

### 核心业务流程 (100% 可移植)
```
1. Create Event Flow
   Input: EventData
   → Core.EventOperations.create()
   → Backend.EventBackend.save()
   → Core.EventCreated signal
   → UI 刷新

2. Sync Calendar Flow
   Core.SyncManager.sync()
   → Backend.sync()
   → Detect changes
   → Core.EventsUpdated signal
   → UI 刷新

3. Delete Event Flow
   Input: EventID
   → Core.EventOperations.delete()
   → Backend.EventBackend.delete()
   → Core.EventDeleted signal
   → UI 刷新
```

### KDE 特定流程 (可选，可替代)
```
1. Add Online Account Flow
   UI 交互
   → AkonadiAccountManager.addAccount()
   → Akonadi Agent Setup
   → Account added

2. Auto-complete Contact Flow
   User输入
   → KDEContactProvider.search()
   → Akonadi Query
   → Display suggestions

3. Sync with Online Calendars
   Timer trigger
   → AkonadiSyncManager.syncAll()
   → Akonadi Agent sync
   → Core.EventsUpdated
```

## 🚀 迁移路线图

### Phase 1: 基础设施 (2-3周)
- [ ] 创建 core/ 目录和抽象接口
- [ ] 提取核心数据模型
- [ ] 创建 ICalendarBackend 接口
- [ ] 为 Akonadi 创建适配器

### Phase 2: UI 抽象 (3-4周)
- [ ] 定义 QML 接口契约
- [ ] 创建 kirigami/ 具体实现
- [ ] 修改现有 QML 使用接口
- [ ] 创建 UI 工厂/注册机制

### Phase 3: 功能解耦 (4-6周)
- [ ] 为每个 KDE 集成点创建接口
- [ ] 实现可选的 fallback
- [ ] 添加依赖注入
- [ ] 测试非KDE环境运行

### Phase 4: 验证和优化 (2-3周)
- [ ] 单元测试
- [ ] 集成测试
- [ ] 性能测试
- [ ] 文档

## 📊 耦合度评分

| 组件 | 耦合度 | 优先级 | 难度 |
|------|--------|--------|------|
| Akonadi Calendar | 9/10 | P0 | ⭐⭐⭐⭐ |
| Kirigami UI | 8/10 | P1 | ⭐⭐⭐ |
| Akonadi Contacts | 7/10 | P2 | ⭐⭐⭐ |
| KDE Config | 6/10 | P3 | ⭐⭐ |
| KDE Translation | 5/10 | P4 | ⭐ |
| KDE Icons/Theme | 4/10 | P5 | ⭐ |

## 💡 建议行动

### 如果想保持 KDE 兼容：
✅ 实施分层架构（上述方案）
✅ 保持 Akonadi 作为推荐后端
✅ 支持其他后端作为可选

### 如果想完全解耦：
⚠️ 放弃 Akonadi（失去在线日历功能简单性）
⚠️ 放弃 Kirigami（重写所有 UI）
⚠️ 工作量约 3-6 个月

### 最平衡方案（推荐）：
✅ 实施接口层（6-8 周工作）
✅ 保持 Akonadi 作为默认实现
✅ 未来可选择更轻量级后端
✅ 支持桌面和移动

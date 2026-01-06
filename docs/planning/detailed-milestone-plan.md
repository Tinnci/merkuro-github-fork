# Merkuro 详细执行计划：周间里程碑

> 这是 REFACTORING_FEASIBILITY_REVIEW.md 的详细版本
> 提供每周的具体交付物、验收标准、和检查清单

---

## 📅 完整时间表

```
Week 1-2  : Pre-Phase (准备)
  ├─ 创建目录结构
  ├─ CMake 模块化
  └─ 测试框架选择

Week 3-6  : Phase 1 (核心库) - 🎯 核心库完全独立
Week 7-9  : Phase 2 (Akonadi 适配) - 🎯 可插拔后端
Week 10   : Phase 3 (DI 容器) - 🎯 完整解耦

Week 11-14: Phase 4 (UI 抽象) [可选]
Week 15-17: Phase 5 (可选后端) [可选]
Week 18-20: Phase 6 (测试/文档) [可选]

总计: 20 周 (快速路径 10 周 + 完整路径 10 周)
```

---

## 🏁 Pre-Phase: 准备阶段 (Week 1-2)

### Week 1: 基础设施

#### Day 1-2: 目录结构
```bash
# 创建新的目录结构
mkdir -p src/core/{models,data,operations,utils}
mkdir -p src/backends/{akonadi,local,cloud}
mkdir -p tests/{unit,integration,fixtures}
mkdir -p docs

# 创建 CMakeLists.txt 模板
touch src/core/CMakeLists.txt
touch src/backends/CMakeLists.txt
touch tests/CMakeLists.txt
```

**验收标准**:
- [ ] 目录结构创建完成
- [ ] 现有代码不动
- [ ] 可以 `git status` 查看变化

#### Day 3-4: CMake 模块化
```cmake
# src/core/CMakeLists.txt
add_library(personalcalendar-core STATIC
    models/CalendarEvent.cpp
    data/ICalendarStorage.cpp
    operations/EventOperations.cpp
    utils/RecurrenceCalculator.cpp
)

target_include_directories(personalcalendar-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(personalcalendar-core
    Qt5::Core
    # NOT 任何 KDE 库
)

# tests/CMakeLists.txt
enable_testing()
add_subdirectory(unit)
add_subdirectory(integration)
```

**验收标准**:
- [ ] `cmake . && make` 能编译（即使文件为空）
- [ ] 编译不涉及 Akonadi
- [ ] 现有应用仍能正常编译

#### Day 5: 代码审视
```markdown
# REFACTORING_LOG.md

## Akonadi 使用点
- [ ] calendarmanager.cpp:15 - Akonadi::ETMCalendar
- [ ] calendarmanager.cpp:42 - Akonadi::IncidenceChanger
- [ ] incidencewrapper.cpp:20 - KCalendarCore::Incidence::Ptr
- ... (继续标记所有)

## 文件分类
- [x] 可保留 (不修改): about.cpp, about.h
- [ ] 需解耦 (修改): calendarmanager.cpp, incidencewrapper.cpp
- [ ] 需适配 (后续): contactsmanager.cpp, tagmanager.cpp
```

**验收标准**:
- [ ] 标记了所有 Akonadi 依赖
- [ ] 标记了所有 KCalendarCore 依赖
- [ ] 创建了迁移检查清单

#### Day 6-7: 测试框架选择

**推荐**: Google Test + Qt Test 混合

```cmake
# 添加到 CMakeLists.txt
find_package(GTest REQUIRED)
find_package(Qt5Test REQUIRED)

# tests/unit/CMakeLists.txt
add_executable(calendar-core-tests
    CalendarEventTest.cpp
    RecurrenceTest.cpp
)
target_link_libraries(calendar-core-tests
    personalcalendar-core
    GTest::GTest GTest::Main
)
add_test(NAME CoreTests COMMAND calendar-core-tests)
```

**验收标准**:
- [ ] 测试框架能编译
- [ ] 能 `make test` 运行
- [ ] 有测试结果报告

---

### Week 2: 代码审视与规划

#### Day 1-3: 深度分析
```
创建 REFACTORING_DETAILED_PLAN.md
└── 记录每个类的迁移计划
    ├── CalendarManager
    │   ├── 哪些方法可移到核心库
    │   ├── 哪些方法保留
    │   └── 如何过渡
    ├── IncidenceWrapper
    │   ├── 拆分为 Core::CalendarEvent
    │   └── 保留 UI 相关逻辑
    └── TodoModel
        ├── 数据部分 → Core::TodoOperations
        └── 视图部分 → QML Model
```

**产出**:
- [ ] 每个主要类都有迁移计划
- [ ] 标记了"易迁移"和"难迁移"部分
- [ ] 估算了每部分的代码行数

#### Day 4-5: 团队同步
- [ ] 分享代码审视结果
- [ ] 讨论迁移策略
- [ ] 确认测试标准
- [ ] 分配 Phase 1 负责人

#### Day 6-7: 最终检查
```bash
# 确保现有代码仍能编译
cmake . && make

# 检查新目录不影响既有构建
git status | grep -E "^?? src/(core|backends)"
```

**成果**:
- [ ] 新建的目录结构不影响现有编译
- [ ] 团队对迁移计划达成共识
- [ ] 测试框架已经就位
- [ ] 都已做好 Phase 1 的准备

---

## 🎯 Phase 1: 核心库 (Week 3-6)

### Week 3: 数据模型

**目标**: 创建 0 个 KDE 依赖的数据模型

#### src/core/models/CalendarEvent.h

```cpp
#pragma once

#include <QString>
#include <QDateTime>
#include <QList>
#include <memory>

namespace PersonalCalendar::Core {

enum class EventType {
    Event,      // 普通事件
    Todo,       // 待办事项
    Journal     // 日记
};

enum class EventStatus {
    None,       // 未设定
    Tentative,  // 待定
    Confirmed,  // 确定
    Cancelled   // 已取消
};

struct Attendee {
    QString uid;
    QString name;
    QString email;
    QString role;      // CHAIR, REQ-PARTICIPANT, OPT-PARTICIPANT, NON-PARTICIPANT
    QString status;    // NEEDS-ACTION, ACCEPTED, DECLINED, TENTATIVE, DELEGATED
};

struct Alarm {
    int minutesBefore = -1;
    QString action;     // DISPLAY, EMAIL, PROCEDURE, AUDIO
    QString description;
};

class Recurrence {
public:
    enum class Pattern {
        None,
        Daily,
        Weekly,
        Monthly,
        Yearly
    };

    Pattern pattern = Pattern::None;
    int interval = 1;
    QDate endDate;          // 递归截止日期，为空表示无限
    QList<int> byDayOfWeek; // 1-7 (Monday-Sunday)
    QList<int> byDayOfMonth;
    QStringList byMonth;
    
    bool isValid() const;
};

class CalendarEvent {
public:
    // 基本属性
    QString uid;                // 唯一标识
    QString title;
    QString description;
    QString location;
    
    // 日期时间
    QDateTime startDateTime;
    QDateTime endDateTime;
    bool isAllDay = false;
    
    // 递归
    Recurrence recurrence;
    QList<QDate> recurrenceExceptions;  // 排除的日期
    
    // 状态
    EventType type = EventType::Event;
    EventStatus status = EventStatus::Confirmed;
    int priority = 0;           // 0-9，0 表示未定
    
    // 参与者
    QList<Attendee> attendees;
    QString organizer;          // 组织者 email/uid
    
    // 提醒
    QList<Alarm> alarms;
    
    // 附加信息
    QStringList categories;
    QString url;
    QByteArray attachment;      // 附件 (可选)
    
    // 元数据
    QString calendarId;         // 所属日历
    QDateTime created;
    QDateTime lastModified;
    
public:
    bool isValid() const;
    
    // 序列化
    QString toICalString() const;
    static CalendarEvent fromICalString(const QString& ical);
    
    // 时间检查
    bool hasStarted(const QDateTime& now) const;
    bool hasEnded(const QDateTime& now) const;
    bool occurredOn(const QDate& date) const;
    
    // 比较
    bool operator==(const CalendarEvent& other) const;
};

using CalendarEventPtr = std::shared_ptr<CalendarEvent>;
using CalendarEventConstPtr = std::shared_ptr<const CalendarEvent>;

} // namespace PersonalCalendar::Core
```

**工作量**: 150 行代码

**验收标准**:
- [ ] 编译无错误
- [ ] 0 个 #include KDE 头
- [ ] 包含所有日历事件字段

#### src/core/models/TodoItem.h
```cpp
// 类似 CalendarEvent，但针对 Todo 优化
// 字段:
// - uid, title, description
// - startDateTime, dueDateTime, completedDateTime
// - priority (0-9)
// - percentComplete (0-100)
// - status (NEEDS-ACTION, IN-PROCESS, COMPLETED, CANCELLED)
// - categories, location
// - recurrence
```

**工作量**: 100 行代码

#### src/core/models/Recurrence.h
```cpp
// 递归规则和计算
class RecurrenceRule {
public:
    // iCalendar RRULE 的 C++ 表示
    Pattern pattern;
    int frequency;
    QDate startDate;
    QDate until;
    int count;
    QStringList byDay;   // MO, TU, WE, TH, FR, SA, SU
    QList<int> byMonthDay;
    QStringList byMonth;
    
    bool isValid() const;
    QString toICalString() const;
    static RecurrenceRule fromICalString(const QString& rrule);
};
```

**工作量**: 80 行代码

#### src/core/models/CalendarCollection.h
```cpp
// 日历集合信息
class CalendarCollection {
public:
    QString id;
    QString name;
    QString description;
    QString color;          // #RRGGBB
    bool isReadonly = false;
    bool isVisible = true;
    QString owner;          // 拥有者邮箱（如果是共享日历）
    
    bool isValid() const;
};

using CalendarCollectionPtr = std::shared_ptr<CalendarCollection>;
```

**工作量**: 60 行代码

#### 单元测试: tests/unit/CalendarEventTest.cpp

```cpp
#include <gtest/gtest.h>
#include "core/models/CalendarEvent.h"

using namespace PersonalCalendar::Core;

class CalendarEventTest : public ::testing::Test {
protected:
    CalendarEvent event;
};

TEST_F(CalendarEventTest, BasicCreation) {
    event.title = "Test Event";
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    EXPECT_EQ(event.title, "Test Event");
    EXPECT_TRUE(event.isValid());
}

TEST_F(CalendarEventTest, AllDayEvent) {
    event.title = "All Day Event";
    event.isAllDay = true;
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(0, 0));
    
    EXPECT_TRUE(event.isAllDay);
}

TEST_F(CalendarEventTest, WithAttendees) {
    Attendee attendee;
    attendee.name = "John Doe";
    attendee.email = "john@example.com";
    
    event.attendees.append(attendee);
    EXPECT_EQ(event.attendees.size(), 1);
}

TEST_F(CalendarEventTest, ICalSerialization) {
    event.title = "Meeting";
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(14, 0));
    
    QString ical = event.toICalString();
    EXPECT_FALSE(ical.isEmpty());
    EXPECT_TRUE(ical.contains("SUMMARY:Meeting"));
}

TEST_F(CalendarEventTest, ICalDeserialization) {
    QString ical = "BEGIN:VEVENT\n"
                   "SUMMARY:Test\n"
                   "DTSTART:20260106T100000Z\n"
                   "DTEND:20260106T110000Z\n"
                   "END:VEVENT";
    
    auto event2 = CalendarEvent::fromICalString(ical);
    EXPECT_EQ(event2.title, "Test");
}
```

**工作量**: 150+ 行测试代码

#### Day 7 验收
```bash
cd src/core
cmake . && make
cd ../../tests/unit
cmake . && make && ./calendar-core-tests

# 期望结果:
# [==========] 10 tests from CalendarEventTest
# [       OK ] CalendarEventTest.BasicCreation
# ...
```

**Week 3 成果**:
- ✅ 4 个数据模型完成 (CalendarEvent, TodoItem, Recurrence, Collection)
- ✅ 20+ 单元测试通过
- ✅ 测试覆盖率 >85%

---

### Week 4: 存储接口与业务逻辑

#### src/core/data/ICalendarStorage.h

```cpp
#pragma once

#include "CalendarEvent.h"
#include <QString>
#include <QList>
#include <QDate>
#include <memory>

namespace PersonalCalendar::Core {

class ICalendarStorage {
public:
    virtual ~ICalendarStorage() = default;
    
    // ===== 事件操作 =====
    virtual bool createEvent(const CalendarEventPtr& event) = 0;
    virtual CalendarEventPtr getEvent(const QString& uid) = 0;
    virtual bool updateEvent(const CalendarEventPtr& event) = 0;
    virtual bool deleteEvent(const QString& uid) = 0;
    
    // ===== 查询 =====
    virtual QList<CalendarEventPtr> getEventsByDate(const QDate& date) = 0;
    virtual QList<CalendarEventPtr> getEventsByDateRange(
        const QDate& start, const QDate& end) = 0;
    virtual QList<CalendarEventPtr> getEventsByCollection(
        const QString& collectionId) = 0;
    
    // ===== 日历管理 =====
    virtual QList<QString> getCalendarIds() = 0;
    virtual QString getCalendarName(const QString& id) = 0;
    virtual bool createCalendar(const QString& id, const QString& name) = 0;
    virtual bool deleteCalendar(const QString& id) = 0;
    
    // ===== 同步 =====
    virtual bool sync() = 0;
    virtual bool isOnline() const = 0;
    virtual QString getLastSyncTime(const QString& collectionId) = 0;
};

using ICalendarStoragePtr = std::shared_ptr<ICalendarStorage>;

} // namespace PersonalCalendar::Core
```

**工作量**: 60 行代码

#### src/core/operations/EventOperations.h

```cpp
#pragma once

#include "core/data/ICalendarStorage.h"
#include <functional>
#include <QString>

namespace PersonalCalendar::Core {

class EventOperations {
public:
    using SuccessCallback = std::function<void(const CalendarEventPtr&)>;
    using ErrorCallback = std::function<void(const QString& error)>;
    using EventListCallback = std::function<void(const QList<CalendarEventPtr>&)>;
    
    explicit EventOperations(ICalendarStoragePtr storage);
    virtual ~EventOperations() = default;
    
    // ===== 高级操作 =====
    
    // 创建新事件
    void createEvent(const CalendarEventPtr& event,
                     SuccessCallback onSuccess,
                     ErrorCallback onError);
    
    // 更新现有事件
    void updateEvent(const CalendarEventPtr& event,
                     SuccessCallback onSuccess,
                     ErrorCallback onError);
    
    // 删除事件
    void deleteEvent(const QString& uid,
                     SuccessCallback onSuccess,
                     ErrorCallback onError);
    
    // 获取单个事件
    void getEvent(const QString& uid,
                  SuccessCallback onSuccess,
                  ErrorCallback onError);
    
    // 获取某日期的事件
    void getEventsForDate(const QDate& date,
                          EventListCallback onSuccess,
                          ErrorCallback onError);
    
    // 获取日期范围内的事件
    void getEventsForDateRange(const QDate& start, const QDate& end,
                               EventListCallback onSuccess,
                               ErrorCallback onError);
    
    // ===== 辅助操作 =====
    
    // 生成递归事件的实例
    QList<CalendarEventPtr> expandRecurrenceInstances(
        const CalendarEventPtr& event,
        const QDate& start,
        const QDate& end) const;
    
protected:
    ICalendarStoragePtr m_storage;
    
private:
    void handleError(const QString& error, ErrorCallback callback);
};

} // namespace PersonalCalendar::Core
```

**工作量**: 80 行代码

#### src/core/utils/RecurrenceCalculator.h

```cpp
#pragma once

#include "CalendarEvent.h"
#include <QDate>
#include <QList>

namespace PersonalCalendar::Core {

class RecurrenceCalculator {
public:
    // 计算所有递归实例
    static QList<QDateTime> calculateInstances(
        const CalendarEvent& event,
        const QDate& rangeStart,
        const QDate& rangeEnd);
    
    // 检查事件是否在指定日期发生
    static bool occurredOn(const CalendarEvent& event, const QDate& date);
    
    // 获取下一个发生日期
    static QDate getNextOccurrence(
        const CalendarEvent& event,
        const QDate& afterDate);
    
private:
    static QList<QDateTime> expandDaily(
        const CalendarEvent& event,
        const QDate& rangeStart,
        const QDate& rangeEnd);
    
    static QList<QDateTime> expandWeekly(
        const CalendarEvent& event,
        const QDate& rangeStart,
        const QDate& rangeEnd);
    
    // ... (expandMonthly, expandYearly)
};

} // namespace PersonalCalendar::Core
```

**工作量**: 100 行头文件

#### src/core/utils/DateTimeUtils.h

```cpp
#pragma once

#include <QDateTime>
#include <QDate>
#include <QString>

namespace PersonalCalendar::Core {

class DateTimeUtils {
public:
    // 日期操作
    static int daysInMonth(int year, int month);
    static int weekOfMonth(const QDate& date);
    static int weekOfYear(const QDate& date);
    
    // 时间操作
    static bool isSameDay(const QDateTime& dt1, const QDateTime& dt2);
    static bool isSameMonth(const QDateTime& dt1, const QDateTime& dt2);
    static bool isSameYear(const QDateTime& dt1, const QDateTime& dt2);
    
    // 格式化
    static QString formatDate(const QDate& date, const QString& format);
    static QString formatTime(const QDateTime& dt, const QString& format);
    
    // 时区
    static QDateTime toUTC(const QDateTime& dt, const QString& timezone);
    static QDateTime fromUTC(const QDateTime& utc, const QString& timezone);
};

} // namespace PersonalCalendar::Core
```

**工作量**: 40 行代码

#### 单元测试：Week 4

```cpp
// tests/unit/EventOperationsTest.cpp
#include <gtest/gtest.h>
#include "core/operations/EventOperations.h"
#include "MockCalendarStorage.h"

using namespace PersonalCalendar::Core;

class EventOperationsTest : public ::testing::Test {
protected:
    std::shared_ptr<MockCalendarStorage> mockStorage;
    std::shared_ptr<EventOperations> operations;
    
    void SetUp() override {
        mockStorage = std::make_shared<MockCalendarStorage>();
        operations = std::make_shared<EventOperations>(mockStorage);
    }
};

TEST_F(EventOperationsTest, CreateEvent) {
    auto event = std::make_shared<CalendarEvent>();
    event->title = "Test";
    
    bool success = false;
    operations->createEvent(event,
        [&](const CalendarEventPtr&) { success = true; },
        [](const QString&) { });
    
    // 实际实现需要处理异步
}

// tests/unit/RecurrenceCalculatorTest.cpp
TEST(RecurrenceCalculatorTest, DailyRecurrence) {
    CalendarEvent event;
    event.title = "Daily";
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    Recurrence& rec = event.recurrence;
    rec.pattern = Recurrence::Pattern::Daily;
    rec.interval = 1;
    rec.endDate = QDate(2026, 1, 10);
    
    auto instances = RecurrenceCalculator::calculateInstances(
        event,
        QDate(2026, 1, 6),
        QDate(2026, 1, 10)
    );
    
    EXPECT_EQ(instances.size(), 5);  // Jan 6-10
}
```

**工作量**: 200+ 行测试代码

#### Week 4 成果
- ✅ ICalendarStorage 接口定义
- ✅ EventOperations 实现
- ✅ RecurrenceCalculator 实现
- ✅ DateTimeUtils 实现
- ✅ 30+ 单元测试通过
- ✅ 测试覆盖率 >85%

---

### Week 5-6: 完整测试与文档

#### Week 5: 补充测试与 Mock 实现

```cpp
// tests/unit/MockCalendarStorage.h
class MockCalendarStorage : public ICalendarStorage {
public:
    // 所有虚函数的 Mock 实现
    bool createEvent(const CalendarEventPtr& event) override {
        m_events[event->uid] = event;
        return true;
    }
    
    // ... 其他实现
    
private:
    QMap<QString, CalendarEventPtr> m_events;
    QMap<QString, CalendarCollectionPtr> m_collections;
};
```

**单元测试覆盖**:
- [ ] CalendarEvent: 15 个测试
- [ ] TodoItem: 10 个测试
- [ ] Recurrence: 20 个测试
- [ ] RecurrenceCalculator: 30 个测试
- [ ] DateTimeUtils: 20 个测试
- [ ] EventOperations: 15 个测试

**目标**: >90% 代码覆盖率

#### Week 6: 文档与集成验证

```markdown
# docs/CORE_API.md

## 核心库文档

### 快速开始

```cpp
#include "core/models/CalendarEvent.h"
#include "core/operations/EventOperations.h"

using namespace PersonalCalendar::Core;

// 创建事件
auto event = std::make_shared<CalendarEvent>();
event->title = "Meeting";
event->startDateTime = QDateTime::currentDateTime();

// 使用 Mock 存储进行测试
auto mockStorage = std::make_shared<MockCalendarStorage>();
EventOperations ops(mockStorage);

ops.createEvent(event,
    [](const CalendarEventPtr& e) { qDebug() << "Success"; },
    [](const QString& err) { qDebug() << "Error:" << err; });
```

### API 参考
...
```

**验收标准**:
- [ ] 所有公开类都有文档
- [ ] 至少 3 个使用示例
- [ ] API 文档可自动生成 (doxygen)

#### Phase 1 最终验收

```bash
# 编译核心库
cd src/core && cmake . && make

# 运行所有单元测试
make test
# 期望: 100+ 个测试全部通过

# 生成覆盖率报告
cmake -DCMAKE_BUILD_TYPE=Coverage . && make coverage
# 期望: >85% 代码覆盖率

# 检查无 KDE 依赖
ldd ./lib/libpersonalcalendar-core.a | grep kde
# 期望: 无输出

# 生成文档
doxygen Doxyfile
# 期望: docs/html/index.html 生成成功
```

**🎉 Phase 1 里程碑: 核心库完全独立！**

---

## 🎯 Phase 2: Akonadi 适配 (Week 7-9)

### Week 7-8: 适配器实现

#### src/backends/akonadi/AkonadiCalendarBackend.h

```cpp
#pragma once

#include "core/data/ICalendarStorage.h"
#include <Akonadi/Calendar/ETMCalendar>
#include <Akonadi/Calendar/IncidenceChanger>
#include <memory>

namespace PersonalCalendar::Akonadi {

class AkonadiCalendarBackend : public Core::ICalendarStorage {
public:
    AkonadiCalendarBackend();
    ~AkonadiCalendarBackend() override;
    
    // 实现 ICalendarStorage 的所有虚函数
    bool createEvent(const Core::CalendarEventPtr& event) override;
    Core::CalendarEventPtr getEvent(const QString& uid) override;
    // ... 其他接口方法

private:
    ::Akonadi::ETMCalendar::Ptr m_akonadiCalendar;
    ::Akonadi::IncidenceChanger::Ptr m_incidenceChanger;
    
    // 数据转换
    KCalendarCore::Incidence::Ptr toAkonadiIncidence(
        const Core::CalendarEventPtr& event);
    Core::CalendarEventPtr fromAkonadiIncidence(
        const KCalendarCore::Incidence::Ptr& incidence);
};

} // namespace PersonalCalendar::Akonadi
```

#### src/backends/akonadi/AkonadiDataConverter.h

```cpp
// 处理 Core::CalendarEvent ↔ KCalendarCore::Incidence 转换
// 关键转换点:
// - 递归规则转换 (RRULE)
// - 时区处理 (TZID)
// - Attendees 转换
// - Alarms 转换
// - 附加信息转换

class AkonadiDataConverter {
public:
    static KCalendarCore::Incidence::Ptr 
    toAkonadiIncidence(const Core::CalendarEventPtr& event);
    
    static Core::CalendarEventPtr
    fromAkonadiIncidence(const KCalendarCore::Incidence::Ptr& incidence);
    
private:
    static void convertRecurrence(...);
    static void convertAttendees(...);
    static void convertAlarms(...);
};
```

**工作量**: 400+ 行代码（这是最复杂的部分）

### Week 9: 集成测试

```cpp
// tests/integration/AkonadiBackendTest.cpp
class AkonadiBackendTest : public ::testing::Test {
protected:
    std::shared_ptr<AkonadiCalendarBackend> backend;
    
    void SetUp() override {
        // 初始化 Akonadi (需要 Akonadi 服务运行)
        backend = std::make_shared<AkonadiCalendarBackend>();
    }
};

TEST_F(AkonadiBackendTest, CreateAndRetrieveEvent) {
    auto event = std::make_shared<Core::CalendarEvent>();
    event->title = "Integration Test Event";
    event->startDateTime = QDateTime::currentDateTime();
    
    // 创建事件
    EXPECT_TRUE(backend->createEvent(event));
    
    // 检索事件
    auto retrieved = backend->getEvent(event->uid);
    EXPECT_EQ(retrieved->title, event->title);
}

TEST_F(AkonadiBackendTest, RecurrenceHandling) {
    // 测试递归事件的准确转换
}
```

**🎉 Phase 2 里程碑: Akonadi 正式成为"可选后端"！**

---

## 🎯 Phase 3: DI 容器 (Week 10)

### src/core/ServiceContainer.h

```cpp
namespace PersonalCalendar::Core {

class ServiceContainer {
public:
    static ServiceContainer& instance();
    
    void registerCalendarStorage(ICalendarStoragePtr storage);
    void registerEventOperations(std::shared_ptr<EventOperations> ops);
    
    ICalendarStoragePtr getCalendarStorage() const;
    std::shared_ptr<EventOperations> getEventOperations() const;
    
private:
    ServiceContainer();
    
    ICalendarStoragePtr m_calendarStorage;
    std::shared_ptr<EventOperations> m_eventOperations;
};

} // namespace PersonalCalendar::Core
```

### src/main.cpp (修改)

```cpp
#include "core/ServiceContainer.h"
#include "backends/akonadi/AkonadiCalendarBackend.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 初始化服务容器
    auto& container = PersonalCalendar::Core::ServiceContainer::instance();
    
    // 注册 Akonadi 后端（或其他）
    auto backend = std::make_shared<PersonalCalendar::Akonadi::AkonadiCalendarBackend>();
    container.registerCalendarStorage(backend);
    
    auto eventOps = std::make_shared<PersonalCalendar::Core::EventOperations>(backend);
    container.registerEventOperations(eventOps);
    
    // 注册给 QML
    qmlRegisterSingletonType<PersonalCalendar::Core::ServiceContainer>(
        "org.personalcalendar.core", 1, 0, "CalendarService",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return &PersonalCalendar::Core::ServiceContainer::instance();
        }
    );
    
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/ui/main.qml")));
    
    return app.exec();
}
```

**🎉 Phase 3 里程碑: 完整解耦架构就位！**

---

## ✅ 检查清单

### 每周检查 (周末)

- [ ] 完成了计划的 commit？
- [ ] 所有测试都通过了？
- [ ] 代码覆盖率达标了？
- [ ] Code Review 通过了？
- [ ] 文档更新了？

### 里程碑检查 (6, 9, 10 周)

- [ ] Phase 完整吗？
- [ ] 性能满足要求吗？
- [ ] 向后兼容吗？
- [ ] 文档完整吗？

---

## 🏁 成功标志

### Week 6 之后
```bash
$ cd src/core && cmake . && make && make test
# ✅ 所有测试通过
# ✅ 覆盖率 >85%
# ✅ 0 个 KDE 依赖
```

### Week 9 之后
```bash
$ ./bin/kalendar --backend=akonadi
# ✅ 应用正常运行
$ ./bin/kalendar --backend=mock
# ✅ 应用用 Mock 后端正常运行
```

### Week 10 之后
```cpp
// QML 代码
import org.personalcalendar.core 1.0
CalendarService.eventOperations.createEvent(...)
// ✅ 不再直接调用 Akonadi
```

---

## 📞 获得帮助

如有问题，查看：
- `IMPLEMENTATION_GUIDE.md` - 具体代码实现
- `ARCHITECTURE_ANALYSIS.md` - 架构分析
- 相关的代码注释和单元测试

**下一步**: 确认启动，开始 Week 1!

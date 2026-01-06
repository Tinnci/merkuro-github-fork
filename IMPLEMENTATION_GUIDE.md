# 解耦实现指南

## 第一步：创建核心数据模型（零KDE依赖）

### 1. src/core/models/CalendarEvent.h

```cpp
#pragma once

#include <QString>
#include <QDateTime>
#include <QList>
#include <memory>

namespace PersonalCalendar::Core {

enum class EventType {
    Event,
    Todo,
    Journal
};

enum class EventStatus {
    None,
    Tentative,
    Confirmed,
    Cancelled
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
    QDate endDate;
    QList<int> byDayOfWeek;
    QList<int> byDayOfMonth;
};

class CalendarEvent {
public:
    // Basic properties
    QString uid;
    QString title;
    QString description;
    QString location;
    
    // DateTime
    QDateTime startDateTime;
    QDateTime endDateTime;
    bool isAllDay = false;
    
    // Recurrence
    Recurrence recurrence;
    
    // Status
    EventType type = EventType::Event;
    EventStatus status = EventStatus::Confirmed;
    int priority = 0;
    
    // Attendees
    struct Attendee {
        QString name;
        QString email;
        QString role;
    };
    QList<Attendee> attendees;
    
    // Metadata
    QString calendarId;
    QDateTime created;
    QDateTime lastModified;
    
public:
    bool isValid() const;
    QString toICalString() const;
    static CalendarEvent fromICalString(const QString& ical);
};

using CalendarEventPtr = std::shared_ptr<CalendarEvent>;

} // namespace PersonalCalendar::Core
```

### 2. src/core/data/ICalendarStorage.h

```cpp
#pragma once

#include "CalendarEvent.h"
#include <QString>
#include <QList>
#include <memory>

namespace PersonalCalendar::Core {

class ICalendarStorage {
public:
    virtual ~ICalendarStorage() = default;
    
    // CRUD operations
    virtual bool createEvent(const CalendarEventPtr& event) = 0;
    virtual CalendarEventPtr getEvent(const QString& uid) = 0;
    virtual QList<CalendarEventPtr> getEventsByDate(const QDate& date) = 0;
    virtual QList<CalendarEventPtr> getEventsByDateRange(const QDate& start, const QDate& end) = 0;
    virtual bool updateEvent(const CalendarEventPtr& event) = 0;
    virtual bool deleteEvent(const QString& uid) = 0;
    
    // Calendar management
    virtual QList<QString> getCalendarIds() = 0;
    virtual QString getCalendarName(const QString& id) = 0;
    virtual bool createCalendar(const QString& id, const QString& name) = 0;
    
    // Sync
    virtual bool sync() = 0;
    virtual bool isOnline() const = 0;
};

using ICalendarStoragePtr = std::shared_ptr<ICalendarStorage>;

} // namespace PersonalCalendar::Core
```

### 3. src/core/operations/EventOperations.h

```cpp
#pragma once

#include "ICalendarStorage.h"
#include <functional>

namespace PersonalCalendar::Core {

class EventOperations {
public:
    using EventCallback = std::function<void(const CalendarEventPtr&)>;
    using ErrorCallback = std::function<void(const QString& error)>;
    
    explicit EventOperations(ICalendarStoragePtr storage);
    
    // High-level operations
    void createEvent(const CalendarEventPtr& event,
                    EventCallback onSuccess,
                    ErrorCallback onError);
    
    void updateEvent(const CalendarEventPtr& event,
                    EventCallback onSuccess,
                    ErrorCallback onError);
    
    void deleteEvent(const QString& uid,
                    EventCallback onSuccess,
                    ErrorCallback onError);
    
    void getEvent(const QString& uid,
                 EventCallback onSuccess,
                 ErrorCallback onError);
    
    void getEventsForDate(const QDate& date,
                         std::function<void(const QList<CalendarEventPtr>&)> onSuccess,
                         ErrorCallback onError);
    
    // Signals/Events (implementation-dependent)
    // Emit these when operations complete
    
private:
    ICalendarStoragePtr m_storage;
};

} // namespace PersonalCalendar::Core
```

## 第二步：创建 Akonadi 适配器

### 4. src/backends/akonadi/AkonadiCalendarBackend.h

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
    
    bool createEvent(const Core::CalendarEventPtr& event) override;
    Core::CalendarEventPtr getEvent(const QString& uid) override;
    QList<Core::CalendarEventPtr> getEventsByDate(const QDate& date) override;
    QList<Core::CalendarEventPtr> getEventsByDateRange(const QDate& start, const QDate& end) override;
    bool updateEvent(const Core::CalendarEventPtr& event) override;
    bool deleteEvent(const QString& uid) override;
    
    QList<QString> getCalendarIds() override;
    QString getCalendarName(const QString& id) override;
    bool createCalendar(const QString& id, const QString& name) override;
    
    bool sync() override;
    bool isOnline() const override;
    
private:
    ::Akonadi::ETMCalendar::Ptr m_akonadiCalendar;
    ::Akonadi::IncidenceChanger::Ptr m_incidenceChanger;
    
    // Helper methods
    KCalendarCore::Incidence::Ptr toAkonadiIncidence(const Core::CalendarEventPtr& event);
    Core::CalendarEventPtr fromAkonadiIncidence(const KCalendarCore::Incidence::Ptr& incidence);
};

} // namespace PersonalCalendar::Akonadi
```

## 第三步：依赖注入

### 5. src/core/ServiceContainer.h

```cpp
#pragma once

#include "data/ICalendarStorage.h"
#include "operations/EventOperations.h"
#include <memory>
#include <map>

namespace PersonalCalendar::Core {

class ServiceContainer {
public:
    static ServiceContainer& instance();
    
    // Register services
    void registerCalendarStorage(ICalendarStoragePtr storage);
    void registerEventOperations(std::shared_ptr<EventOperations> operations);
    
    // Get services
    ICalendarStoragePtr getCalendarStorage() const;
    std::shared_ptr<EventOperations> getEventOperations() const;
    
private:
    ServiceContainer();
    
    ICalendarStoragePtr m_calendarStorage;
    std::shared_ptr<EventOperations> m_eventOperations;
};

} // namespace PersonalCalendar::Core
```

## 第四步：初始化应用

### 6. src/main.cpp (修改)

```cpp
#include "core/ServiceContainer.h"
#include "backends/akonadi/AkonadiCalendarBackend.h"
#include "core/operations/EventOperations.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Initialize services
    auto& container = PersonalCalendar::Core::ServiceContainer::instance();
    
    // Use Akonadi as default backend
    auto akonadiBackend = std::make_shared<PersonalCalendar::Akonadi::AkonadiCalendarBackend>();
    container.registerCalendarStorage(akonadiBackend);
    
    auto eventOps = std::make_shared<PersonalCalendar::Core::EventOperations>(akonadiBackend);
    container.registerEventOperations(eventOps);
    
    // Register with QML
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

## 第五步：QML 集成

### 7. src/contents/ui/EventForm.qml (示例)

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import org.personalcalendar.core 1.0

Controls.Dialog {
    id: eventForm
    
    standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel
    
    Controls.TextField {
        id: titleField
        placeholderText: "Event title"
    }
    
    Controls.TextField {
        id: descriptionField
        placeholderText: "Description"
    }
    
    onAccepted: {
        // Use injected service instead of Akonadi directly
        var event = {
            title: titleField.text,
            description: descriptionField.text,
            startDateTime: new Date(),
            endDateTime: new Date()
        };
        
        CalendarService.eventOperations.createEvent(event,
            function(success) {
                console.log("Event created");
            },
            function(error) {
                console.error("Failed to create event:", error);
            }
        );
    }
}
```

## 迁移检查清单

- [ ] Phase 1: 核心库
  - [ ] CalendarEvent 和相关数据类
  - [ ] ICalendarStorage 接口
  - [ ] EventOperations 类
  - [ ] 单元测试

- [ ] Phase 2: Akonadi 适配器
  - [ ] AkonadiCalendarBackend 实现
  - [ ] 数据转换层（Akonadi ↔ Core）
  - [ ] 集成测试

- [ ] Phase 3: 依赖注入
  - [ ] ServiceContainer
  - [ ] QML 注册
  - [ ] 更新 main.cpp

- [ ] Phase 4: 逐步迁移现有代码
  - [ ] CalendarManager 重构
  - [ ] 迁移 TodoModel
  - [ ] 迁移 MonthModel 等

- [ ] Phase 5: 测试和验证
  - [ ] 单元测试覆盖
  - [ ] 集成测试
  - [ ] 无 KDE 环境测试（可选实现）

## 优势

✅ **完全独立的核心库** - 可在任何应用中重用
✅ **易于替换后端** - 实现新的 ICalendarStorage 接口即可
✅ **便于测试** - 可使用 Mock 实现进行单元测试
✅ **渐进式迁移** - 无需一次性重写所有代码
✅ **向后兼容** - 可保持 Akonadi 作为默认实现

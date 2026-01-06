// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core/ServiceContainer.h"
#include "core/models/CalendarEvent.h"
#include "core/models/TodoItem.h"
#include <QMap>
#include <QSet>
#include <gtest/gtest.h>

using namespace PersonalCalendar::Core;

/**
 * @brief Mock Calendar Storage for testing
 *
 * 一个简单的内存实现，用于测试而不需要真实的后端
 */
class MockCalendarStorage : public ICalendarStorage
{
public:
    bool createEvent(const CalendarEventPtr &event) override
    {
        if (!event || !event->isValid()) {
            return false;
        }
        m_events[event->uid] = event;
        return true;
    }

    CalendarEventPtr getEvent(const QString &uid) override
    {
        auto it = m_events.find(uid);
        if (it != m_events.end()) {
            return it.value();
        }
        return nullptr;
    }

    bool updateEvent(const CalendarEventPtr &event) override
    {
        if (!event || !event->isValid()) {
            return false;
        }
        m_events[event->uid] = event;
        return true;
    }

    bool deleteEvent(const QString &uid) override { return m_events.remove(uid) > 0; }

    QList<CalendarEventPtr> getEventsByDate(const QDate &date) override
    {
        QList<CalendarEventPtr> result;
        for (const auto &event : m_events) {
            if (event->occurredOn(date)) {
                result.append(event);
            }
        }
        return result;
    }

    QList<CalendarEventPtr> getEventsByDateRange(const QDate &start, const QDate &end) override
    {
        QList<CalendarEventPtr> result;
        for (const auto &event : m_events) {
            if (event->startDateTime.date() >= start && event->endDateTime.date() <= end) {
                result.append(event);
            }
        }
        return result;
    }

    QList<CalendarEventPtr> getEventsByCollection(const QString &collectionId) override
    {
        QList<CalendarEventPtr> result;
        for (const auto &event : m_events) {
            if (event->calendarId == collectionId) {
                result.append(event);
            }
        }
        return result;
    }

    QList<QString> getCalendarIds() override
    {
        QSet<QString> ids;
        for (const auto &event : m_events) {
            if (!event->calendarId.isEmpty()) {
                ids.insert(event->calendarId);
            }
        }
        return ids.values();
    }

    QString getCalendarName(const QString &id) override { return m_calendarNames.value(id, QLatin1String("")); }

    bool createCalendar(const QString &id, const QString &name) override
    {
        m_calendarNames[id] = name;
        return true;
    }

    bool deleteCalendar(const QString &id) override
    {
        m_calendarNames.remove(id);
        // 删除该日历中的所有事件
        for (auto it = m_events.begin(); it != m_events.end(); ++it) {
            if (it.value()->calendarId == id) {
                it = m_events.erase(it);
            }
        }
        return true;
    }

    bool sync() override { return true; }
    bool isOnline() const override { return true; }
    QString getLastSyncTime(const QString &collectionId) override
    {
        return QDateTime::currentDateTime().toString(Qt::ISODate);
    }

private:
    QMap<QString, CalendarEventPtr> m_events;
    QMap<QString, QString> m_calendarNames;
};

class ServiceContainerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 重置服务容器
        ServiceContainer::instance().reset();
    }

    void TearDown() override { ServiceContainer::instance().reset(); }
};

TEST_F(ServiceContainerTest, SingletonInstance)
{
    auto &container1 = ServiceContainer::instance();
    auto &container2 = ServiceContainer::instance();

    EXPECT_EQ(&container1, &container2);
}

TEST_F(ServiceContainerTest, RegisterAndRetrieveStorage)
{
    auto &container = ServiceContainer::instance();
    auto storage = std::make_shared<MockCalendarStorage>();

    container.registerCalendarStorage(storage);

    EXPECT_EQ(container.getCalendarStorage(), storage);
}

TEST_F(ServiceContainerTest, RegisterAndRetrieveOperations)
{
    auto &container = ServiceContainer::instance();
    auto storage = std::make_shared<MockCalendarStorage>();
    auto operations = std::make_shared<EventOperations>(storage);

    container.registerCalendarStorage(storage);
    container.registerEventOperations(operations);

    EXPECT_EQ(container.getEventOperations(), operations);
}

TEST_F(ServiceContainerTest, CreateEventThroughService)
{
    auto &container = ServiceContainer::instance();
    auto storage = std::make_shared<MockCalendarStorage>();
    auto operations = std::make_shared<EventOperations>(storage);

    container.registerCalendarStorage(storage);
    container.registerEventOperations(operations);

    auto event = std::make_shared<CalendarEvent>();
    event->uid = QLatin1String("test-1");
    event->title = QLatin1String("Test Event");
    event->startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event->endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));

    bool created = false;
    auto ops = container.getEventOperations();
    ops->createEvent(event, [&created](const CalendarEventPtr &) { created = true; }, [](const QString &) {});

    EXPECT_TRUE(created);
}

TEST_F(ServiceContainerTest, IsInitialized)
{
    auto &container = ServiceContainer::instance();

    EXPECT_FALSE(container.isInitialized());

    auto storage = std::make_shared<MockCalendarStorage>();
    container.registerCalendarStorage(storage);

    EXPECT_FALSE(container.isInitialized()); // 还需要注册 operations

    auto operations = std::make_shared<EventOperations>(storage);
    container.registerEventOperations(operations);

    EXPECT_TRUE(container.isInitialized());
}

TEST_F(ServiceContainerTest, Reset)
{
    auto &container = ServiceContainer::instance();
    auto storage = std::make_shared<MockCalendarStorage>();

    container.registerCalendarStorage(storage);
    EXPECT_TRUE(container.getCalendarStorage() != nullptr);

    container.reset();
    EXPECT_FALSE(container.getCalendarStorage());
}

TEST_F(ServiceContainerTest, MockStorageCRUD)
{
    auto storage = std::make_shared<MockCalendarStorage>();

    // 创建
    auto event = std::make_shared<CalendarEvent>();
    event->uid = QLatin1String("test-1");
    event->title = QLatin1String("Test Event");
    event->startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event->endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));

    EXPECT_TRUE(storage->createEvent(event));

    // 读取
    auto retrieved = storage->getEvent(QLatin1String("test-1"));
    EXPECT_TRUE(retrieved != nullptr);
    EXPECT_EQ(retrieved->title, QLatin1String("Test Event"));

    // 更新
    retrieved->title = QLatin1String("Updated Event");
    EXPECT_TRUE(storage->updateEvent(retrieved));

    auto updated = storage->getEvent(QLatin1String("test-1"));
    EXPECT_EQ(updated->title, QLatin1String("Updated Event"));

    // 删除
    EXPECT_TRUE(storage->deleteEvent(QLatin1String("test-1")));
    EXPECT_FALSE(storage->getEvent(QLatin1String("test-1")));
}

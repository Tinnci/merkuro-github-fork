// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "backends/local/DirectoryBackend.h"
#include <QFile>
#include <QTemporaryDir>
#include <gtest/gtest.h>

using namespace PersonalCalendar;

class DirectoryBackendTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        tempDir = std::make_shared<QTemporaryDir>();
        ASSERT_TRUE(tempDir->isValid());
        dirPath = tempDir->path();
    }

    void TearDown() override { tempDir.reset(); }

    std::shared_ptr<QTemporaryDir> tempDir;
    QString dirPath;

    Core::CalendarEventPtr createTestEvent(const QString &uid, const QString &title)
    {
        auto event = std::make_shared<Core::CalendarEvent>();
        event->uid = uid;
        event->title = title;
        event->startDateTime = QDateTime(QDate(2026, 1, 10), QTime(10, 0));
        event->endDateTime = QDateTime(QDate(2026, 1, 10), QTime(11, 0));
        return event;
    }
};

TEST_F(DirectoryBackendTest, InitializeEmptyDirectory)
{
    Local::DirectoryBackend backend(dirPath);

    auto ids = backend.getCalendarIds();
    EXPECT_EQ(ids.size(), 0);
}

TEST_F(DirectoryBackendTest, CreateCalendar)
{
    Local::DirectoryBackend backend(dirPath);

    EXPECT_TRUE(backend.createCalendar(QLatin1String("work"), QLatin1String("Work Calendar")));

    auto ids = backend.getCalendarIds();
    EXPECT_EQ(ids.size(), 1);
    EXPECT_EQ(ids[0], QLatin1String("work"));
}

TEST_F(DirectoryBackendTest, GetCalendarName)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("personal"), QLatin1String("Personal Calendar"));

    QString name = backend.getCalendarName(QLatin1String("personal"));
    EXPECT_EQ(name, QLatin1String("Personal Calendar"));
}

TEST_F(DirectoryBackendTest, CreateEventInFirstCalendar)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("personal"), QLatin1String("Personal"));
    auto event = createTestEvent(QLatin1String("event-1"), QLatin1String("Meeting"));

    EXPECT_TRUE(backend.createEvent(event));

    auto retrieved = backend.getEvent(QLatin1String("event-1"));
    EXPECT_TRUE(retrieved != nullptr);
    EXPECT_EQ(retrieved->title, QLatin1String("Meeting"));
}

TEST_F(DirectoryBackendTest, CreateEventInDefaultCalendar)
{
    Local::DirectoryBackend backend(dirPath);

    // No calendar created yet, should create default
    auto event = createTestEvent(QLatin1String("event-2"), QLatin1String("Task"));
    EXPECT_TRUE(backend.createEvent(event));

    auto ids = backend.getCalendarIds();
    EXPECT_EQ(ids.size(), 1);
    EXPECT_EQ(ids[0], QLatin1String("personal"));
}

TEST_F(DirectoryBackendTest, MultipleCalendars)
{
    Local::DirectoryBackend backend(dirPath);

    // Create multiple calendars
    EXPECT_TRUE(backend.createCalendar(QLatin1String("work"), QLatin1String("Work")));
    EXPECT_TRUE(backend.createCalendar(QLatin1String("personal"), QLatin1String("Personal")));
    EXPECT_TRUE(backend.createCalendar(QLatin1String("family"), QLatin1String("Family")));

    auto ids = backend.getCalendarIds();
    EXPECT_EQ(ids.size(), 3);
}

TEST_F(DirectoryBackendTest, EventsAcrossCalendars)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("work"), QLatin1String("Work"));
    backend.createCalendar(QLatin1String("personal"), QLatin1String("Personal"));

    auto event1 = createTestEvent(QLatin1String("work-1"), QLatin1String("Meeting"));
    auto event2 = createTestEvent(QLatin1String("personal-1"), QLatin1String("Vacation"));

    EXPECT_TRUE(backend.createEvent(event1));
    EXPECT_TRUE(backend.createEvent(event2));

    auto e1 = backend.getEvent(QLatin1String("work-1"));
    auto e2 = backend.getEvent(QLatin1String("personal-1"));

    EXPECT_EQ(e1->title, QLatin1String("Meeting"));
    EXPECT_EQ(e2->title, QLatin1String("Vacation"));
}

TEST_F(DirectoryBackendTest, DeleteCalendar)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("temp"), QLatin1String("Temporary"));
    auto event = createTestEvent(QLatin1String("temp-1"), QLatin1String("Temp Event"));
    backend.createEvent(event);

    // Delete calendar should also remove events
    EXPECT_TRUE(backend.deleteCalendar(QLatin1String("temp")));

    auto ids = backend.getCalendarIds();
    EXPECT_EQ(ids.size(), 0);
    EXPECT_FALSE(backend.getEvent(QLatin1String("temp-1")));
}

TEST_F(DirectoryBackendTest, GetEventsByDate)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("work"), QLatin1String("Work"));
    backend.createCalendar(QLatin1String("personal"), QLatin1String("Personal"));

    // Create events on same date in different calendars
    auto event1 = createTestEvent(QLatin1String("work-1"), QLatin1String("Meeting"));
    auto event2 = createTestEvent(QLatin1String("personal-1"), QLatin1String("Lunch"));

    backend.createEvent(event1);
    backend.createEvent(event2);

    auto events = backend.getEventsByDate(QDate(2026, 1, 10));
    EXPECT_EQ(events.size(), 2);
}

TEST_F(DirectoryBackendTest, GetEventsByDateRange)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("main"), QLatin1String("Main"));

    auto event1 = createTestEvent(QLatin1String("event-1"), QLatin1String("Event 1"));
    backend.createEvent(event1);

    auto event2 = createTestEvent(QLatin1String("event-2"), QLatin1String("Event 2"));
    event2->startDateTime = QDateTime(QDate(2026, 1, 15), QTime(10, 0));
    event2->endDateTime = QDateTime(QDate(2026, 1, 15), QTime(11, 0));
    backend.createEvent(event2);

    auto events = backend.getEventsByDateRange(QDate(2026, 1, 10), QDate(2026, 1, 15));
    EXPECT_EQ(events.size(), 2);
}

TEST_F(DirectoryBackendTest, GetEventsByCollection)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("work"), QLatin1String("Work"));

    auto event1 = createTestEvent(QLatin1String("work-1"), QLatin1String("Meeting"));
    auto event2 = createTestEvent(QLatin1String("work-2"), QLatin1String("Review"));

    backend.createEvent(event1);
    backend.createEvent(event2);

    auto events = backend.getEventsByCollection(QLatin1String("work"));
    EXPECT_EQ(events.size(), 2);
}

TEST_F(DirectoryBackendTest, PersistenceAcrossInstances)
{
    // Create backend and add calendar/event
    {
        Local::DirectoryBackend backend(dirPath);
        backend.createCalendar(QLatin1String("persistent"), QLatin1String("Persistent"));
        auto event = createTestEvent(QLatin1String("persist-1"), QLatin1String("Important"));
        backend.createEvent(event);
    }

    // Create new backend and verify calendar/event exist
    {
        Local::DirectoryBackend backend(dirPath);
        auto ids = backend.getCalendarIds();
        EXPECT_EQ(ids.size(), 1);
        EXPECT_EQ(ids[0], QLatin1String("persistent"));

        auto event = backend.getEvent(QLatin1String("persist-1"));
        EXPECT_TRUE(event != nullptr);
        EXPECT_EQ(event->title, QLatin1String("Important"));
    }
}

TEST_F(DirectoryBackendTest, Sync)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("sync-test"), QLatin1String("Sync Test"));
    auto event = createTestEvent(QLatin1String("sync-1"), QLatin1String("Sync Event"));
    backend.createEvent(event);

    EXPECT_TRUE(backend.sync());

    // After sync, event should still be retrievable
    auto retrieved = backend.getEvent(QLatin1String("sync-1"));
    EXPECT_TRUE(retrieved != nullptr);
}

TEST_F(DirectoryBackendTest, IsOnline)
{
    Local::DirectoryBackend backend(dirPath);
    EXPECT_TRUE(backend.isOnline());
}

TEST_F(DirectoryBackendTest, InvalidOperations)
{
    Local::DirectoryBackend backend(dirPath);

    // Try to create duplicate calendar
    backend.createCalendar(QLatin1String("dup"), QLatin1String("Duplicate"));
    EXPECT_FALSE(backend.createCalendar(QLatin1String("dup"), QLatin1String("Duplicate")));

    // Try to delete non-existent calendar
    EXPECT_FALSE(backend.deleteCalendar(QLatin1String("nonexistent")));

    // Try to get non-existent event
    EXPECT_FALSE(backend.getEvent(QLatin1String("nonexistent")));
}

TEST_F(DirectoryBackendTest, LastSyncTime)
{
    Local::DirectoryBackend backend(dirPath);

    backend.createCalendar(QLatin1String("sync"), QLatin1String("Sync"));
    auto event = createTestEvent(QLatin1String("sync-1"), QLatin1String("Event"));
    backend.createEvent(event);

    QString syncTime = backend.getLastSyncTime(QLatin1String("sync"));
    EXPECT_FALSE(syncTime.isEmpty());
}

TEST_F(DirectoryBackendTest, ManyCalendars)
{
    Local::DirectoryBackend backend(dirPath);

    // Create 10 calendars
    for (int i = 0; i < 10; ++i) {
        QString id = QLatin1String("cal-") + QString::number(i);
        QString name = QLatin1String("Calendar ") + QString::number(i);
        EXPECT_TRUE(backend.createCalendar(id, name));
    }

    auto ids = backend.getCalendarIds();
    EXPECT_EQ(ids.size(), 10);
}

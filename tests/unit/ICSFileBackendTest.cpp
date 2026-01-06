// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <gtest/gtest.h>
#include <QTemporaryFile>
#include <QFile>
#include "backends/local/ICSFileBackend.h"

using namespace PersonalCalendar;

class ICSFileBackendTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary file for testing
        tempFile.setFileTemplate(QLatin1String("test_calendar_XXXXXX.ics"));
        if (tempFile.open()) {
            filePath = tempFile.fileName();
            tempFile.close();
        }
    }
    
    void TearDown() override {
        tempFile.remove();
    }
    
    QTemporaryFile tempFile;
    QString filePath;
    
    Core::CalendarEventPtr createTestEvent(const QString& uid, const QString& title)
    {
        auto event = std::make_shared<Core::CalendarEvent>();
        event->uid = uid;
        event->title = title;
        event->startDateTime = QDateTime(QDate(2026, 1, 10), QTime(10, 0));
        event->endDateTime = QDateTime(QDate(2026, 1, 10), QTime(11, 0));
        return event;
    }
};

TEST_F(ICSFileBackendTest, CreateEvent)
{
    Local::ICSFileBackend backend(filePath);
    auto event = createTestEvent(QLatin1String("event-1"), QLatin1String("Test Event"));
    
    EXPECT_TRUE(backend.createEvent(event));
    EXPECT_EQ(backend.getEvent(QLatin1String("event-1"))->title, QLatin1String("Test Event"));
}

TEST_F(ICSFileBackendTest, GetEvent)
{
    Local::ICSFileBackend backend(filePath);
    auto event = createTestEvent(QLatin1String("event-2"), QLatin1String("Meeting"));
    
    backend.createEvent(event);
    auto retrieved = backend.getEvent(QLatin1String("event-2"));
    
    EXPECT_TRUE(retrieved != nullptr);
    EXPECT_EQ(retrieved->uid, QLatin1String("event-2"));
    EXPECT_EQ(retrieved->title, QLatin1String("Meeting"));
}

TEST_F(ICSFileBackendTest, UpdateEvent)
{
    Local::ICSFileBackend backend(filePath);
    auto event = createTestEvent(QLatin1String("event-3"), QLatin1String("Original"));
    
    backend.createEvent(event);
    
    // Update the event
    event->title = QLatin1String("Updated");
    EXPECT_TRUE(backend.updateEvent(event));
    
    auto updated = backend.getEvent(QLatin1String("event-3"));
    EXPECT_EQ(updated->title, QLatin1String("Updated"));
}

TEST_F(ICSFileBackendTest, DeleteEvent)
{
    Local::ICSFileBackend backend(filePath);
    auto event = createTestEvent(QLatin1String("event-4"), QLatin1String("ToDelete"));
    
    backend.createEvent(event);
    EXPECT_TRUE(backend.deleteEvent(QLatin1String("event-4")));
    EXPECT_FALSE(backend.getEvent(QLatin1String("event-4")));
}

TEST_F(ICSFileBackendTest, GetEventsByDate)
{
    Local::ICSFileBackend backend(filePath);
    
    auto event1 = createTestEvent(QLatin1String("event-5"), QLatin1String("Event on 10th"));
    backend.createEvent(event1);
    
    auto event2 = createTestEvent(QLatin1String("event-6"), QLatin1String("Event on 11th"));
    event2->startDateTime = QDateTime(QDate(2026, 1, 11), QTime(10, 0));
    event2->endDateTime = QDateTime(QDate(2026, 1, 11), QTime(11, 0));
    backend.createEvent(event2);
    
    auto eventsOnTenth = backend.getEventsByDate(QDate(2026, 1, 10));
    EXPECT_EQ(eventsOnTenth.size(), 1);
    EXPECT_EQ(eventsOnTenth[0]->uid, QLatin1String("event-5"));
}

TEST_F(ICSFileBackendTest, GetEventsByDateRange)
{
    Local::ICSFileBackend backend(filePath);
    
    auto event1 = createTestEvent(QLatin1String("event-7"), QLatin1String("Event 1"));
    backend.createEvent(event1);
    
    auto event2 = createTestEvent(QLatin1String("event-8"), QLatin1String("Event 2"));
    event2->startDateTime = QDateTime(QDate(2026, 1, 12), QTime(10, 0));
    event2->endDateTime = QDateTime(QDate(2026, 1, 12), QTime(11, 0));
    backend.createEvent(event2);
    
    auto events = backend.getEventsByDateRange(QDate(2026, 1, 10), QDate(2026, 1, 12));
    EXPECT_EQ(events.size(), 2);
}

TEST_F(ICSFileBackendTest, CalendarIds)
{
    Local::ICSFileBackend backend(filePath);
    
    auto ids = backend.getCalendarIds();
    EXPECT_EQ(ids.size(), 1);
    EXPECT_EQ(ids[0], QLatin1String("local"));
}

TEST_F(ICSFileBackendTest, CalendarName)
{
    Local::ICSFileBackend backend(filePath);
    
    QString name = backend.getCalendarName(QLatin1String("local"));
    EXPECT_EQ(name, QLatin1String("Local Calendar"));
}

TEST_F(ICSFileBackendTest, Sync)
{
    Local::ICSFileBackend backend(filePath);
    auto event = createTestEvent(QLatin1String("event-9"), QLatin1String("Before Sync"));
    
    backend.createEvent(event);
    EXPECT_TRUE(backend.sync());
    
    auto retrieved = backend.getEvent(QLatin1String("event-9"));
    EXPECT_TRUE(retrieved != nullptr);
}

TEST_F(ICSFileBackendTest, IsOnline)
{
    Local::ICSFileBackend backend(filePath);
    EXPECT_TRUE(backend.isOnline());
}

TEST_F(ICSFileBackendTest, PersistenceAcrossInstances)
{
    // Create first instance and add event
    {
        Local::ICSFileBackend backend(filePath);
        auto event = createTestEvent(QLatin1String("event-10"), QLatin1String("Persistent"));
        backend.createEvent(event);
    } // backend goes out of scope, file is saved
    
    // Create second instance and verify event exists
    {
        Local::ICSFileBackend backend(filePath);
        auto retrieved = backend.getEvent(QLatin1String("event-10"));
        EXPECT_TRUE(retrieved != nullptr);
        EXPECT_EQ(retrieved->title, QLatin1String("Persistent"));
    }
}

TEST_F(ICSFileBackendTest, InvalidOperations)
{
    Local::ICSFileBackend backend(filePath);
    
    // Try to get non-existent event
    EXPECT_FALSE(backend.getEvent(QLatin1String("non-existent")));
    
    // Try to delete non-existent event
    EXPECT_FALSE(backend.deleteEvent(QLatin1String("non-existent")));
    
    // Try to create calendar (not supported)
    EXPECT_FALSE(backend.createCalendar(QLatin1String("test"), QLatin1String("Test")));
    
    // Try to delete calendar (not supported)
    EXPECT_FALSE(backend.deleteCalendar(QLatin1String("local")));
}

TEST_F(ICSFileBackendTest, MultipleEvents)
{
    Local::ICSFileBackend backend(filePath);
    
    // Create multiple events
    for (int i = 0; i < 10; ++i) {
        auto event = createTestEvent(
            QLatin1String("event-") + QString::number(i),
            QLatin1String("Event ") + QString::number(i)
        );
        EXPECT_TRUE(backend.createEvent(event));
    }
    
    // Retrieve and verify
    for (int i = 0; i < 10; ++i) {
        auto event = backend.getEvent(
            QLatin1String("event-") + QString::number(i)
        );
        EXPECT_TRUE(event != nullptr);
    }
}

TEST_F(ICSFileBackendTest, LastSyncTime)
{
    Local::ICSFileBackend backend(filePath);
    auto event = createTestEvent(QLatin1String("event-11"), QLatin1String("Test"));
    backend.createEvent(event);
    
    QString syncTime = backend.getLastSyncTime(QLatin1String("local"));
    EXPECT_FALSE(syncTime.isEmpty());
}

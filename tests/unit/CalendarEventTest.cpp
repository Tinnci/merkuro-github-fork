// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <gtest/gtest.h>
#include "core/models/CalendarEvent.h"
#include <QDateTime>

using namespace PersonalCalendar::Core;

class CalendarEventTest : public ::testing::Test {
protected:
    CalendarEvent event;
};

TEST_F(CalendarEventTest, BasicCreation)
{
    event.uid = QLatin1String("test-1");
    event.title = QLatin1String("Test Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    EXPECT_EQ(event.title, QLatin1String("Test Event"));
    EXPECT_TRUE(event.isValid());
}

TEST_F(CalendarEventTest, InvalidWithoutTitle)
{
    event.uid = QLatin1String("test-2");
    // title 为空
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    EXPECT_FALSE(event.isValid());
}

TEST_F(CalendarEventTest, AllDayEvent)
{
    event.uid = QLatin1String("test-3");
    event.title = QLatin1String("All Day Event");
    event.isAllDay = true;
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(0, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(23, 59));
    
    EXPECT_TRUE(event.isAllDay);
    EXPECT_TRUE(event.isValid());
}

TEST_F(CalendarEventTest, WithAttendees)
{
    event.uid = QLatin1String("test-4");
    event.title = QLatin1String("Meeting");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(14, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(15, 0));
    
    Attendee attendee;
    attendee.uid = QLatin1String("john@example.com");
    attendee.name = QLatin1String("John Doe");
    attendee.email = QLatin1String("john@example.com");
    
    event.attendees.append(attendee);
    
    EXPECT_EQ(event.attendees.size(), 1);
    EXPECT_EQ(event.attendees[0].name, QLatin1String("John Doe"));
}

TEST_F(CalendarEventTest, Recurrence)
{
    event.uid = QLatin1String("test-5");
    event.title = QLatin1String("Daily Meeting");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    event.recurrence.pattern = Recurrence::Pattern::Daily;
    event.recurrence.interval = 1;
    event.recurrence.endDate = QDate(2026, 1, 10);
    
    EXPECT_TRUE(event.recurrence.isValid());
    EXPECT_EQ(event.recurrence.pattern, Recurrence::Pattern::Daily);
}

TEST_F(CalendarEventTest, HasStarted)
{
    event.uid = QLatin1String("test-6");
    event.title = QLatin1String("Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    QDateTime before(QDate(2026, 1, 6), QTime(9, 0));
    QDateTime after(QDate(2026, 1, 6), QTime(12, 0));
    
    EXPECT_FALSE(event.hasStarted(before));
    EXPECT_TRUE(event.hasStarted(after));
}

TEST_F(CalendarEventTest, HasEnded)
{
    event.uid = QLatin1String("test-7");
    event.title = QLatin1String("Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    QDateTime before(QDate(2026, 1, 6), QTime(9, 0));
    QDateTime after(QDate(2026, 1, 6), QTime(12, 0));
    
    EXPECT_FALSE(event.hasEnded(before));
    EXPECT_TRUE(event.hasEnded(after));
}

TEST_F(CalendarEventTest, OccurredOn)
{
    event.uid = QLatin1String("test-8");
    event.title = QLatin1String("Multi-day Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 5), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 7), QTime(11, 0));
    
    EXPECT_FALSE(event.occurredOn(QDate(2026, 1, 4)));
    EXPECT_TRUE(event.occurredOn(QDate(2026, 1, 6)));
    EXPECT_FALSE(event.occurredOn(QDate(2026, 1, 8)));
}

TEST_F(CalendarEventTest, ICalSerialization)
{
    event.uid = QLatin1String("test-9");
    event.title = QLatin1String("Meeting");
    event.location = QLatin1String("Conference Room");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(14, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(15, 0));
    
    QString ical = event.toICalString();
    
    EXPECT_FALSE(ical.isEmpty());
    EXPECT_TRUE(ical.contains(QLatin1String("SUMMARY:Meeting")));
    EXPECT_TRUE(ical.contains(QLatin1String("LOCATION:Conference Room")));
    EXPECT_TRUE(ical.contains(QLatin1String("BEGIN:VEVENT")));
    EXPECT_TRUE(ical.contains(QLatin1String("END:VEVENT")));
}

TEST_F(CalendarEventTest, Equality)
{
    CalendarEvent event1;
    event1.uid = QLatin1String("test-10");
    event1.title = QLatin1String("Event A");
    event1.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event1.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    CalendarEvent event2;
    event2.uid = QLatin1String("test-10");
    event2.title = QLatin1String("Event A");
    event2.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event2.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    
    EXPECT_TRUE(event1 == event2);
}

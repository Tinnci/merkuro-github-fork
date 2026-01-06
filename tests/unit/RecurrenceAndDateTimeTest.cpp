// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "core/utils/DateTimeUtils.h"
#include "core/utils/RecurrenceCalculator.h"
#include <gtest/gtest.h>

using namespace PersonalCalendar::Core;

class RecurrenceCalculatorTest : public ::testing::Test
{
protected:
    CalendarEvent event;
};

TEST_F(RecurrenceCalculatorTest, NonRecurringEvent)
{
    event.uid = QLatin1String("event-1");
    event.title = QLatin1String("Single Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));
    event.recurrence.pattern = Recurrence::Pattern::None;

    auto instances = RecurrenceCalculator::calculateInstances(event, QDate(2026, 1, 1), QDate(2026, 1, 31));

    EXPECT_EQ(instances.size(), 1);
    EXPECT_EQ(instances[0].date(), QDate(2026, 1, 6));
}

TEST_F(RecurrenceCalculatorTest, DailyRecurrence)
{
    event.uid = QLatin1String("event-2");
    event.title = QLatin1String("Daily Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));

    event.recurrence.pattern = Recurrence::Pattern::Daily;
    event.recurrence.interval = 1;
    event.recurrence.endDate = QDate(2026, 1, 10);

    auto instances = RecurrenceCalculator::calculateInstances(event, QDate(2026, 1, 6), QDate(2026, 1, 10));

    EXPECT_EQ(instances.size(), 5); // Jan 6, 7, 8, 9, 10
    EXPECT_EQ(instances[0].date(), QDate(2026, 1, 6));
    EXPECT_EQ(instances[4].date(), QDate(2026, 1, 10));
}

TEST_F(RecurrenceCalculatorTest, OccurredOn)
{
    event.uid = QLatin1String("event-3");
    event.title = QLatin1String("Recurring Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));

    event.recurrence.pattern = Recurrence::Pattern::Daily;
    event.recurrence.interval = 1;
    event.recurrence.endDate = QDate(2026, 1, 10);

    EXPECT_TRUE(RecurrenceCalculator::occurredOn(event, QDate(2026, 1, 6)));
    EXPECT_TRUE(RecurrenceCalculator::occurredOn(event, QDate(2026, 1, 8)));
    EXPECT_TRUE(RecurrenceCalculator::occurredOn(event, QDate(2026, 1, 10)));
    EXPECT_FALSE(RecurrenceCalculator::occurredOn(event, QDate(2026, 1, 11)));
}

TEST_F(RecurrenceCalculatorTest, NextOccurrence)
{
    event.uid = QLatin1String("event-4");
    event.title = QLatin1String("Recurring Event");
    event.startDateTime = QDateTime(QDate(2026, 1, 6), QTime(10, 0));
    event.endDateTime = QDateTime(QDate(2026, 1, 6), QTime(11, 0));

    event.recurrence.pattern = Recurrence::Pattern::Daily;
    event.recurrence.interval = 2; // Every 2 days
    event.recurrence.endDate = QDate(2026, 2, 28);

    QDate nextAfter = RecurrenceCalculator::getNextOccurrence(event, QDate(2026, 1, 6));
    EXPECT_EQ(nextAfter, QDate(2026, 1, 8));
}

// DateTime Utils Tests
class DateTimeUtilsTest : public ::testing::Test
{
};

TEST_F(DateTimeUtilsTest, DaysInMonth)
{
    EXPECT_EQ(DateTimeUtils::daysInMonth(2026, 1), 31);
    EXPECT_EQ(DateTimeUtils::daysInMonth(2026, 2), 28);
    EXPECT_EQ(DateTimeUtils::daysInMonth(2026, 4), 30);
    EXPECT_EQ(DateTimeUtils::daysInMonth(2024, 2), 29); // Leap year
}

TEST_F(DateTimeUtilsTest, LeapYear)
{
    EXPECT_FALSE(DateTimeUtils::isLeapYear(2026));
    EXPECT_FALSE(DateTimeUtils::isLeapYear(2100)); // Divisible by 100 but not 400
    EXPECT_TRUE(DateTimeUtils::isLeapYear(2024));
    EXPECT_TRUE(DateTimeUtils::isLeapYear(2000)); // Divisible by 400
}

TEST_F(DateTimeUtilsTest, IsSameDay)
{
    QDateTime dt1(QDate(2026, 1, 6), QTime(10, 0));
    QDateTime dt2(QDate(2026, 1, 6), QTime(15, 30));
    QDateTime dt3(QDate(2026, 1, 7), QTime(10, 0));

    EXPECT_TRUE(DateTimeUtils::isSameDay(dt1, dt2));
    EXPECT_FALSE(DateTimeUtils::isSameDay(dt1, dt3));
}

TEST_F(DateTimeUtilsTest, IsSameMonth)
{
    QDateTime dt1(QDate(2026, 1, 6), QTime(10, 0));
    QDateTime dt2(QDate(2026, 1, 15), QTime(10, 0));
    QDateTime dt3(QDate(2026, 2, 6), QTime(10, 0));

    EXPECT_TRUE(DateTimeUtils::isSameMonth(dt1, dt2));
    EXPECT_FALSE(DateTimeUtils::isSameMonth(dt1, dt3));
}

TEST_F(DateTimeUtilsTest, IsSameYear)
{
    QDateTime dt1(QDate(2026, 1, 6), QTime(10, 0));
    QDateTime dt2(QDate(2026, 12, 31), QTime(10, 0));
    QDateTime dt3(QDate(2027, 1, 6), QTime(10, 0));

    EXPECT_TRUE(DateTimeUtils::isSameYear(dt1, dt2));
    EXPECT_FALSE(DateTimeUtils::isSameYear(dt1, dt3));
}

TEST_F(DateTimeUtilsTest, GetFirstAndLastDayOfMonth)
{
    QDate firstDay = DateTimeUtils::getFirstDayOfMonth(2026, 1);
    QDate lastDay = DateTimeUtils::getLastDayOfMonth(2026, 1);

    EXPECT_EQ(firstDay, QDate(2026, 1, 1));
    EXPECT_EQ(lastDay, QDate(2026, 1, 31));
}

TEST_F(DateTimeUtilsTest, GetMondayAndSundayOfWeek)
{
    // 2026-01-06 is Tuesday
    QDate date(2026, 1, 6);

    QDate monday = DateTimeUtils::getMondayOfWeek(date);
    QDate sunday = DateTimeUtils::getSundayOfWeek(date);

    EXPECT_EQ(monday.dayOfWeek(), 1); // Monday
    EXPECT_EQ(sunday.dayOfWeek(), 7); // Sunday

    // Monday should be before or equal to date
    EXPECT_LE(monday, date);
    // Sunday should be after or equal to date
    EXPECT_GE(sunday, date);
}

TEST_F(DateTimeUtilsTest, WorkingDaysBetween)
{
    QDate from(2026, 1, 5); // Monday
    QDate to(2026, 1, 9);   // Friday

    int workingDays = DateTimeUtils::workingDaysBetween(from, to);
    EXPECT_EQ(workingDays, 5); // All 5 days are weekdays
}

TEST_F(DateTimeUtilsTest, FormatDate)
{
    QDate date(2026, 1, 6);
    QString formatted = DateTimeUtils::formatDate(date, QLatin1String("yyyy-MM-dd"));

    EXPECT_EQ(formatted, QLatin1String("2026-01-06"));
}

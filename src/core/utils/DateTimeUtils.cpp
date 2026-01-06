// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "DateTimeUtils.h"

namespace PersonalCalendar::Core
{

int DateTimeUtils::daysInMonth(int year, int month)
{
    if (month < 1 || month > 12) {
        return 0;
    }

    static const int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int days = daysPerMonth[month - 1];

    // 闰年 2 月有 29 天
    if (month == 2 && isLeapYear(year)) {
        days = 29;
    }

    return days;
}

int DateTimeUtils::weekOfMonth(const QDate &date)
{
    if (!date.isValid()) {
        return 0;
    }

    int firstDayWeek = getFirstDayOfMonth(date.year(), date.month()).dayOfWeek();
    int dayOfMonth = date.day();

    // 计算周数 (1-6)
    return (dayOfMonth + firstDayWeek - 2) / 7 + 1;
}

int DateTimeUtils::weekOfYear(const QDate &date)
{
    if (!date.isValid()) {
        return 0;
    }

    return date.weekNumber();
}

bool DateTimeUtils::isSameDay(const QDateTime &dt1, const QDateTime &dt2)
{
    return dt1.date() == dt2.date();
}

bool DateTimeUtils::isSameMonth(const QDateTime &dt1, const QDateTime &dt2)
{
    QDate d1 = dt1.date();
    QDate d2 = dt2.date();
    return d1.year() == d2.year() && d1.month() == d2.month();
}

bool DateTimeUtils::isSameYear(const QDateTime &dt1, const QDateTime &dt2)
{
    return dt1.date().year() == dt2.date().year();
}

QDate DateTimeUtils::getFirstDayOfMonth(int year, int month)
{
    return QDate(year, month, 1);
}

QDate DateTimeUtils::getLastDayOfMonth(int year, int month)
{
    int days = daysInMonth(year, month);
    return QDate(year, month, days);
}

QDate DateTimeUtils::getMondayOfWeek(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }

    // Qt 中 Monday = 1, Sunday = 7
    int dayOfWeek = date.dayOfWeek();
    return date.addDays(1 - dayOfWeek);
}

QDate DateTimeUtils::getSundayOfWeek(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }

    int dayOfWeek = date.dayOfWeek();
    return date.addDays(7 - dayOfWeek);
}

bool DateTimeUtils::isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DateTimeUtils::workingDaysBetween(const QDate &from, const QDate &to)
{
    if (!from.isValid() || !to.isValid() || from > to) {
        return 0;
    }

    int workingDays = 0;
    QDate current = from;

    while (current <= to) {
        // 不计周末 (Saturday=6, Sunday=7)
        if (current.dayOfWeek() < 6) {
            workingDays++;
        }
        current = current.addDays(1);
    }

    return workingDays;
}

QString DateTimeUtils::formatDate(const QDate &date)
{
    return formatDate(date, QLatin1String("yyyy-MM-dd"));
}

QString DateTimeUtils::formatDate(const QDate &date, const QString &format)
{
    if (!date.isValid()) {
        return QString();
    }

    return date.toString(format);
}

QString DateTimeUtils::formatDateTime(const QDateTime &dt)
{
    return formatDateTime(dt, QLatin1String("yyyy-MM-dd hh:mm:ss"));
}

QString DateTimeUtils::formatDateTime(const QDateTime &dt, const QString &format)
{
    if (!dt.isValid()) {
        return QString();
    }

    return dt.toString(format);
}

QString DateTimeUtils::formatTime(const QDateTime &dt)
{
    return formatTime(dt, QLatin1String("hh:mm"));
}

QString DateTimeUtils::formatTime(const QDateTime &dt, const QString &format)
{
    if (!dt.isValid()) {
        return QString();
    }

    return dt.time().toString(format);
}

} // namespace PersonalCalendar::Core

// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "RecurrenceCalculator.h"

namespace PersonalCalendar::Core
{

QList<QDateTime> RecurrenceCalculator::calculateInstances(const CalendarEvent &event, const QDate &rangeStart,
                                                          const QDate &rangeEnd)
{
    QList<QDateTime> instances;

    // 非递归事件，只返回原始时间（如果在范围内）
    if (event.recurrence.pattern == Recurrence::Pattern::None) {
        if (event.startDateTime.date() >= rangeStart && event.startDateTime.date() <= rangeEnd) {
            instances.append(event.startDateTime);
        }
        return instances;
    }

    // 根据递归模式计算
    switch (event.recurrence.pattern) {
        case Recurrence::Pattern::Daily:
            instances = expandDaily(event, rangeStart, rangeEnd);
            break;
        case Recurrence::Pattern::Weekly:
            instances = expandWeekly(event, rangeStart, rangeEnd);
            break;
        case Recurrence::Pattern::Monthly:
            instances = expandMonthly(event, rangeStart, rangeEnd);
            break;
        case Recurrence::Pattern::Yearly:
            instances = expandYearly(event, rangeStart, rangeEnd);
            break;
        case Recurrence::Pattern::None:
            break;
    }

    return instances;
}

bool RecurrenceCalculator::occurredOn(const CalendarEvent &event, const QDate &date)
{
    if (!date.isValid()) {
        return false;
    }

    // 单一事件，检查是否在该日期
    if (event.recurrence.pattern == Recurrence::Pattern::None) {
        return event.startDateTime.date() <= date && event.endDateTime.date() >= date;
    }

    // 递归事件，计算该日期范围内的实例
    auto instances = calculateInstances(event, date, date);
    return !instances.isEmpty();
}

QDate RecurrenceCalculator::getNextOccurrence(const CalendarEvent &event, const QDate &afterDate)
{
    if (!afterDate.isValid()) {
        return QDate();
    }

    // 查询之后 365 天内的实例
    auto instances = calculateInstances(event, afterDate.addDays(1), afterDate.addDays(365));

    if (!instances.isEmpty()) {
        return instances.first().date();
    }

    return QDate();
}

QDate RecurrenceCalculator::getPreviousOccurrence(const CalendarEvent &event, const QDate &beforeDate)
{
    if (!beforeDate.isValid()) {
        return QDate();
    }

    // 查询之前 365 天内的实例
    auto instances = calculateInstances(event, beforeDate.addDays(-365), beforeDate.addDays(-1));

    if (!instances.isEmpty()) {
        return instances.last().date();
    }

    return QDate();
}

QList<QDateTime> RecurrenceCalculator::expandDaily(const CalendarEvent &event, const QDate &rangeStart,
                                                   const QDate &rangeEnd)
{
    QList<QDateTime> instances;

    QDate current = event.startDateTime.date();
    int daysInterval = event.recurrence.interval;

    // 如果开始日期在范围之前，调整到范围内
    if (current < rangeStart) {
        int daysUntilRange = current.daysTo(rangeStart);
        int steps = daysUntilRange / daysInterval;
        if (daysUntilRange % daysInterval != 0) {
            steps++; // 向上取整
        }
        current = current.addDays(steps * daysInterval);
    }

    // 生成所有实例
    while (current <= rangeEnd) {
        // 检查递归截止日期
        if (event.recurrence.endDate.isValid() && current > event.recurrence.endDate) {
            break;
        }

        // 检查排除日期
        if (!isException(event, current)) {
            QDateTime instance = QDateTime(current, event.startDateTime.time());
            instances.append(instance);
        }

        current = current.addDays(daysInterval);
    }

    return instances;
}

QList<QDateTime> RecurrenceCalculator::expandWeekly(const CalendarEvent &event, const QDate &rangeStart,
                                                    const QDate &rangeEnd)
{
    QList<QDateTime> instances;

    // 简化版本：每周按 byDayOfWeek 规则生成
    // 完整实现需要处理更多边界情况

    QDate current = event.startDateTime.date();
    int weeksInterval = event.recurrence.interval;

    while (current <= rangeEnd) {
        if (event.recurrence.endDate.isValid() && current > event.recurrence.endDate) {
            break;
        }

        if (current >= rangeStart && !isException(event, current)) {
            QDateTime instance = QDateTime(current, event.startDateTime.time());
            instances.append(instance);
        }

        current = current.addDays(7 * weeksInterval);
    }

    return instances;
}

QList<QDateTime> RecurrenceCalculator::expandMonthly(const CalendarEvent &event, const QDate &rangeStart,
                                                     const QDate &rangeEnd)
{
    QList<QDateTime> instances;

    // 简化版本：每月同一天
    // 完整实现需要处理 byDayOfMonth 等规则

    QDate current = event.startDateTime.date();
    int monthsInterval = event.recurrence.interval;

    while (current <= rangeEnd) {
        if (event.recurrence.endDate.isValid() && current > event.recurrence.endDate) {
            break;
        }

        if (current >= rangeStart && !isException(event, current)) {
            QDateTime instance = QDateTime(current, event.startDateTime.time());
            instances.append(instance);
        }

        current = current.addMonths(monthsInterval);
    }

    return instances;
}

QList<QDateTime> RecurrenceCalculator::expandYearly(const CalendarEvent &event, const QDate &rangeStart,
                                                    const QDate &rangeEnd)
{
    QList<QDateTime> instances;

    QDate current = event.startDateTime.date();
    int yearsInterval = event.recurrence.interval;

    while (current <= rangeEnd) {
        if (event.recurrence.endDate.isValid() && current > event.recurrence.endDate) {
            break;
        }

        if (current >= rangeStart && !isException(event, current)) {
            QDateTime instance = QDateTime(current, event.startDateTime.time());
            instances.append(instance);
        }

        current = current.addYears(yearsInterval);
    }

    return instances;
}

bool RecurrenceCalculator::isException(const CalendarEvent &event, const QDate &date)
{
    return event.recurrenceExceptions.contains(date);
}

} // namespace PersonalCalendar::Core

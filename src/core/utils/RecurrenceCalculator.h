// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "../models/CalendarEvent.h"
#include <QDate>
#include <QDateTime>
#include <QList>

namespace PersonalCalendar::Core
{

/**
 * @brief 递归事件计算器
 *
 * 计算递归规则生成的所有事件实例。
 * 支持日递归、周递归、月递归、年递归。
 */
class RecurrenceCalculator
{
public:
    /**
     * @brief 计算给定日期范围内的所有递归实例
     * @param event 包含递归规则的事件
     * @param rangeStart 范围开始日期
     * @param rangeEnd 范围结束日期
     * @return 所有实例的开始时间列表
     */
    static QList<QDateTime> calculateInstances(const CalendarEvent &event, const QDate &rangeStart,
                                               const QDate &rangeEnd);

    /**
     * @brief 检查事件是否在指定日期发生
     * @param event 事件
     * @param date 检查的日期
     * @return 在该日期发生返回 true
     */
    static bool occurredOn(const CalendarEvent &event, const QDate &date);

    /**
     * @brief 获取下一个发生时间
     * @param event 事件
     * @param afterDate 查询此日期之后
     * @return 下一个发生的日期
     */
    static QDate getNextOccurrence(const CalendarEvent &event, const QDate &afterDate);

    /**
     * @brief 获取上一个发生时间
     * @param event 事件
     * @param beforeDate 查询此日期之前
     * @return 上一个发生的日期
     */
    static QDate getPreviousOccurrence(const CalendarEvent &event, const QDate &beforeDate);

private:
    // 各种递归模式的计算函数
    static QList<QDateTime> expandDaily(const CalendarEvent &event, const QDate &rangeStart, const QDate &rangeEnd);

    static QList<QDateTime> expandWeekly(const CalendarEvent &event, const QDate &rangeStart, const QDate &rangeEnd);

    static QList<QDateTime> expandMonthly(const CalendarEvent &event, const QDate &rangeStart, const QDate &rangeEnd);

    static QList<QDateTime> expandYearly(const CalendarEvent &event, const QDate &rangeStart, const QDate &rangeEnd);

    /**
     * @brief 检查日期是否在排除列表中
     */
    static bool isException(const CalendarEvent &event, const QDate &date);
};

} // namespace PersonalCalendar::Core

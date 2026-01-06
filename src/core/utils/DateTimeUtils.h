// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QDateTime>
#include <QDate>
#include <QString>

namespace PersonalCalendar::Core {

/**
 * @brief 日期时间工具类
 * 
 * 提供各种日期时间计算和格式化的辅助函数。
 */
class DateTimeUtils {
public:
    /**
     * @brief 获取给定月份的天数
     * @param year 年份
     * @param month 月份 (1-12)
     * @return 天数
     */
    static int daysInMonth(int year, int month);
    
    /**
     * @brief 获取日期在月份中的周数
     * @param date 日期
     * @return 周数 (1-6)
     */
    static int weekOfMonth(const QDate& date);
    
    /**
     * @brief 获取日期在年份中的周数
     * @param date 日期
     * @return 周数 (1-53)
     */
    static int weekOfYear(const QDate& date);
    
    /**
     * @brief 检查两个日期是否是同一天
     */
    static bool isSameDay(const QDateTime& dt1, const QDateTime& dt2);
    
    /**
     * @brief 检查两个日期是否是同一月
     */
    static bool isSameMonth(const QDateTime& dt1, const QDateTime& dt2);
    
    /**
     * @brief 检查两个日期是否是同一年
     */
    static bool isSameYear(const QDateTime& dt1, const QDateTime& dt2);
    
    /**
     * @brief 获取月份的第一天
     */
    static QDate getFirstDayOfMonth(int year, int month);
    
    /**
     * @brief 获取月份的最后一天
     */
    static QDate getLastDayOfMonth(int year, int month);
    
    /**
     * @brief 获取周一的日期（给定日期所在的周）
     */
    static QDate getMondayOfWeek(const QDate& date);
    
    /**
     * @brief 获取周日的日期（给定日期所在的周）
     */
    static QDate getSundayOfWeek(const QDate& date);
    
    /**
     * @brief 检查是否是闰年
     */
    static bool isLeapYear(int year);
    
    /**
     * @brief 计算两个日期之间的工作天数（不含周末）
     */
    static int workingDaysBetween(const QDate& from, const QDate& to);
    
    /**
     * @brief 格式化日期
     * @param date 日期
     * @param format 格式字符串 (Qt 的 QDate::toString 格式)
     * @return 格式化的字符串
     */
    static QString formatDate(const QDate& date);
    static QString formatDate(const QDate& date, const QString& format);
    
    /**
     * @brief 格式化时间
     * @param dt 日期时间
     * @param format 格式字符串
     * @return 格式化的字符串
     */
    static QString formatDateTime(const QDateTime& dt);
    static QString formatDateTime(const QDateTime& dt, const QString& format);
    
    /**
     * @brief 格式化时间（仅时间部分）
     * @param dt 日期时间
     * @param format 格式字符串
     * @return 格式化的字符串
     */
    static QString formatTime(const QDateTime& dt);
    static QString formatTime(const QDateTime& dt, const QString& format);
};

} // namespace PersonalCalendar::Core

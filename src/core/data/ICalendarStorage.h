// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "../models/CalendarEvent.h"
#include <QDate>
#include <QList>
#include <QString>
#include <memory>

namespace PersonalCalendar::Core
{

/**
 * @brief 日历存储的抽象接口
 *
 * 任何日历存储实现都需要继承这个接口。
 * 这允许我们支持多个后端：Akonadi、本地 ICS、Google Calendar 等
 */
class ICalendarStorage
{
public:
    virtual ~ICalendarStorage() = default;

    // ===== 事件 CRUD 操作 =====

    /**
     * @brief 创建新事件
     * @param event 要创建的事件
     * @return 成功返回 true
     */
    virtual bool createEvent(const CalendarEventPtr &event) = 0;

    /**
     * @brief 获取单个事件
     * @param uid 事件的唯一标识
     * @return 事件指针，如果不存在返回 nullptr
     */
    virtual CalendarEventPtr getEvent(const QString &uid) = 0;

    /**
     * @brief 更新现有事件
     * @param event 更新的事件
     * @return 成功返回 true
     */
    virtual bool updateEvent(const CalendarEventPtr &event) = 0;

    /**
     * @brief 删除事件
     * @param uid 事件的唯一标识
     * @return 成功返回 true
     */
    virtual bool deleteEvent(const QString &uid) = 0;

    // ===== 查询 =====

    /**
     * @brief 获取特定日期的所有事件
     * @param date 要查询的日期
     * @return 事件列表
     */
    virtual QList<CalendarEventPtr> getEventsByDate(const QDate &date) = 0;

    /**
     * @brief 获取日期范围内的所有事件
     * @param start 开始日期
     * @param end 结束日期
     * @return 事件列表
     */
    virtual QList<CalendarEventPtr> getEventsByDateRange(const QDate &start, const QDate &end) = 0;

    /**
     * @brief 获取特定日历集合的事件
     * @param collectionId 日历集合 ID
     * @return 事件列表
     */
    virtual QList<CalendarEventPtr> getEventsByCollection(const QString &collectionId) = 0;

    // ===== 日历管理 =====

    /**
     * @brief 获取所有日历 ID
     * @return 日历 ID 列表
     */
    virtual QList<QString> getCalendarIds() = 0;

    /**
     * @brief 获取日历名称
     * @param id 日历 ID
     * @return 日历的显示名称
     */
    virtual QString getCalendarName(const QString &id) = 0;

    /**
     * @brief 创建新日历
     * @param id 日历 ID
     * @param name 日历名称
     * @return 成功返回 true
     */
    virtual bool createCalendar(const QString &id, const QString &name) = 0;

    /**
     * @brief 删除日历
     * @param id 日历 ID
     * @return 成功返回 true
     */
    virtual bool deleteCalendar(const QString &id) = 0;

    // ===== 同步 =====

    /**
     * @brief 同步日历
     * @return 成功返回 true
     */
    virtual bool sync() = 0;

    /**
     * @brief 检查是否在线
     * @return 在线返回 true
     */
    virtual bool isOnline() const = 0;

    /**
     * @brief 获取最后同步时间
     * @param collectionId 日历 ID
     * @return 最后同步的时间戳
     */
    virtual QString getLastSyncTime(const QString &collectionId) = 0;
};

using ICalendarStoragePtr = std::shared_ptr<ICalendarStorage>;

} // namespace PersonalCalendar::Core

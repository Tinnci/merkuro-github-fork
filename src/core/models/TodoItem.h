// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "CalendarEvent.h"
#include <QDateTime>
#include <QList>
#include <QString>
#include <memory>

namespace PersonalCalendar::Core
{

/**
 * @brief 待办事项模型
 *
 * 代表一个待办任务。相比 CalendarEvent，TodoItem 更关注：
 * - 完成状态 (NEEDS-ACTION, IN-PROCESS, COMPLETED, CANCELLED)
 * - 完成百分比 (0-100)
 * - 完成时间
 */
class TodoItem
{
public:
    enum class Status {
        NeedsAction, // 需要处理
        InProcess,   // 进行中
        Completed,   // 已完成
        Cancelled    // 已取消
    };

    // 基本属性
    QString uid;
    QString title;
    QString description;
    QString location;

    // 日期时间
    QDateTime startDateTime;
    QDateTime dueDateTime;
    QDateTime completedDateTime;
    bool isAllDay = false;

    // 完成状态
    Status status = Status::NeedsAction;
    int percentComplete = 0; // 0-100

    // 优先级 (0-9, 0 表示未定义)
    int priority = 0;

    // 分类
    QStringList categories;

    // 递归
    Recurrence recurrence;
    QList<QDate> recurrenceExceptions;

    // 元数据
    QString calendarId;
    QDateTime created;
    QDateTime lastModified;

public:
    /**
     * @brief 验证 TodoItem 是否有效
     * @return 有效返回 true
     */
    bool isValid() const;

    /**
     * @brief 检查是否已完成
     * @return 已完成返回 true
     */
    bool isCompleted() const { return status == Status::Completed; }

    /**
     * @brief 检查是否逾期
     * @param now 当前时间
     * @return 逾期返回 true
     */
    bool isOverdue(const QDateTime &now) const;

    /**
     * @brief 标记为完成
     */
    void markCompleted();

    /**
     * @brief 标记为取消
     */
    void markCancelled();

    /**
     * @brief 序列化为 iCalendar 格式
     * @return iCalendar 字符串
     */
    QString toICalString() const;

    /**
     * @brief 从 iCalendar 格式反序列化
     * @param ical iCalendar 字符串
     * @return 反序列化的 TodoItem
     */
    static TodoItem fromICalString(const QString &ical);

    /**
     * @brief 比较两个 TodoItem
     */
    bool operator==(const TodoItem &other) const;
};

using TodoItemPtr = std::shared_ptr<TodoItem>;
using TodoItemConstPtr = std::shared_ptr<const TodoItem>;

} // namespace PersonalCalendar::Core

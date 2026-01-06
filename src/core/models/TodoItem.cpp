// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TodoItem.h"

namespace PersonalCalendar::Core {

bool TodoItem::isValid() const
{
    return !uid.isEmpty() && !title.isEmpty();
}

bool TodoItem::isOverdue(const QDateTime& now) const
{
    if (!dueDateTime.isValid()) {
        return false;
    }
    
    // 已完成的任务不算逾期
    if (isCompleted()) {
        return false;
    }
    
    return dueDateTime < now;
}

void TodoItem::markCompleted()
{
    status = Status::Completed;
    percentComplete = 100;
    completedDateTime = QDateTime::currentDateTime();
    lastModified = QDateTime::currentDateTime();
}

void TodoItem::markCancelled()
{
    status = Status::Cancelled;
    lastModified = QDateTime::currentDateTime();
}

QString TodoItem::toICalString() const
{
    if (!isValid()) {
        return QString();
    }
    
    QString ical = QLatin1String("BEGIN:VTODO\n");
    ical += QLatin1String("UID:") + uid + QLatin1String("\n");
    ical += QLatin1String("SUMMARY:") + title + QLatin1String("\n");
    
    if (!description.isEmpty()) {
        ical += QLatin1String("DESCRIPTION:") + description + QLatin1String("\n");
    }
    
    if (!location.isEmpty()) {
        ical += QLatin1String("LOCATION:") + location + QLatin1String("\n");
    }
    
    if (startDateTime.isValid()) {
        ical += QLatin1String("DTSTART:") + startDateTime.toString(Qt::ISODate) + QLatin1String("\n");
    }
    
    if (dueDateTime.isValid()) {
        ical += QLatin1String("DUE:") + dueDateTime.toString(Qt::ISODate) + QLatin1String("\n");
    }
    
    if (completedDateTime.isValid()) {
        ical += QLatin1String("COMPLETED:") + completedDateTime.toString(Qt::ISODate) + QLatin1String("\n");
    }
    
    // 状态映射
    QString statusStr;
    switch (status) {
        case Status::NeedsAction: statusStr = QLatin1String("NEEDS-ACTION"); break;
        case Status::InProcess: statusStr = QLatin1String("IN-PROCESS"); break;
        case Status::Completed: statusStr = QLatin1String("COMPLETED"); break;
        case Status::Cancelled: statusStr = QLatin1String("CANCELLED"); break;
    }
    ical += QLatin1String("STATUS:") + statusStr + QLatin1String("\n");
    
    if (percentComplete >= 0 && percentComplete <= 100) {
        ical += QLatin1String("PERCENT-COMPLETE:") + QString::number(percentComplete) + QLatin1String("\n");
    }
    
    if (priority > 0 && priority <= 9) {
        ical += QLatin1String("PRIORITY:") + QString::number(priority) + QLatin1String("\n");
    }
    
    ical += QLatin1String("END:VTODO\n");
    
    return ical;
}

TodoItem TodoItem::fromICalString(const QString& ical)
{
    TodoItem todo;
    
    if (ical.contains(QLatin1String("SUMMARY:"))) {
        int start = ical.indexOf(QLatin1String("SUMMARY:")) + 8;
        int end = ical.indexOf(QLatin1Char('\n'), start);
        todo.title = ical.mid(start, end - start).trimmed();
    }
    
    if (ical.contains(QLatin1String("DESCRIPTION:"))) {
        int start = ical.indexOf(QLatin1String("DESCRIPTION:")) + 12;
        int end = ical.indexOf(QLatin1Char('\n'), start);
        todo.description = ical.mid(start, end - start).trimmed();
    }
    
    // 完整的实现会处理所有字段
    // 这里是简化版本
    
    return todo;
}

bool TodoItem::operator==(const TodoItem& other) const
{
    return uid == other.uid && title == other.title &&
           status == other.status && percentComplete == other.percentComplete;
}

} // namespace PersonalCalendar::Core

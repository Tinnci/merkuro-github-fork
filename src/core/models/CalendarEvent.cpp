// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "CalendarEvent.h"
#include <QUuid>

namespace PersonalCalendar::Core
{

bool CalendarEvent::isValid() const
{
    return !uid.isEmpty() && !title.isEmpty() && startDateTime.isValid();
}

QString CalendarEvent::toICalString() const
{
    // 基础 iCalendar 格式导出
    // TODO: 完整实现时需要处理递归、参与者等
    if (!isValid()) {
        return QString();
    }

    QString ical = QLatin1String("BEGIN:VEVENT\n");
    ical += QLatin1String("UID:") + uid + QLatin1String("\n");
    ical += QLatin1String("SUMMARY:") + title + QLatin1String("\n");
    ical += QLatin1String("DTSTART:") + startDateTime.toString(Qt::ISODate) + QLatin1String("\n");
    ical += QLatin1String("DTEND:") + endDateTime.toString(Qt::ISODate) + QLatin1String("\n");
    if (!location.isEmpty()) {
        ical += QLatin1String("LOCATION:") + location + QLatin1String("\n");
    }
    ical += QLatin1String("END:VEVENT\n");

    return ical;
}

CalendarEvent CalendarEvent::fromICalString(const QString &ical)
{
    // 基础 iCalendar 格式导入
    // TODO: 完整实现时需要使用 RFC 5545 解析
    CalendarEvent event;

    if (ical.contains(QLatin1String("SUMMARY:"))) {
        int start = ical.indexOf(QLatin1String("SUMMARY:")) + 8;
        int end = ical.indexOf(QLatin1Char('\n'), start);
        event.title = ical.mid(start, end - start).trimmed();
    }

    return event;
}

bool CalendarEvent::hasStarted(const QDateTime &now) const
{
    return startDateTime <= now;
}

bool CalendarEvent::hasEnded(const QDateTime &now) const
{
    return endDateTime <= now;
}

bool CalendarEvent::occurredOn(const QDate &date) const
{
    return startDateTime.date() <= date && endDateTime.date() >= date;
}

bool CalendarEvent::operator==(const CalendarEvent &other) const
{
    return uid == other.uid && title == other.title && startDateTime == other.startDateTime &&
           endDateTime == other.endDateTime;
}

} // namespace PersonalCalendar::Core

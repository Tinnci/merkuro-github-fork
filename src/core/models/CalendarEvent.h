// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QString>
#include <QDateTime>
#include <QList>
#include <QByteArray>
#include <memory>

namespace PersonalCalendar::Core {

enum class EventType {
    Event,
    Todo,
    Journal
};

enum class EventStatus {
    None,
    Tentative,
    Confirmed,
    Cancelled
};

struct Attendee {
    QString uid;
    QString name;
    QString email;
    QString role;      // CHAIR, REQ-PARTICIPANT, OPT-PARTICIPANT, NON-PARTICIPANT
    QString status;    // NEEDS-ACTION, ACCEPTED, DECLINED, TENTATIVE, DELEGATED
};

struct Alarm {
    int minutesBefore = -1;
    QString action;     // DISPLAY, EMAIL, PROCEDURE, AUDIO
    QString description;
};

class Recurrence {
public:
    enum class Pattern {
        None,
        Daily,
        Weekly,
        Monthly,
        Yearly
    };

    Pattern pattern = Pattern::None;
    int interval = 1;
    QDate endDate;          // 递归截止日期，为空表示无限
    QList<int> byDayOfWeek; // 1-7 (Monday-Sunday)
    QList<int> byDayOfMonth;
    QStringList byMonth;
    
    bool isValid() const { return pattern != Pattern::None; }
};

class CalendarEvent {
public:
    // 基本属性
    QString uid;
    QString title;
    QString description;
    QString location;
    
    // 日期时间
    QDateTime startDateTime;
    QDateTime endDateTime;
    bool isAllDay = false;
    
    // 递归
    Recurrence recurrence;
    QList<QDate> recurrenceExceptions;
    
    // 状态
    EventType type = EventType::Event;
    EventStatus status = EventStatus::Confirmed;
    int priority = 0;
    
    // 参与者
    QList<Attendee> attendees;
    QString organizer;
    
    // 提醒
    QList<Alarm> alarms;
    
    // 附加信息
    QStringList categories;
    QString url;
    QByteArray attachment;
    
    // 元数据
    QString calendarId;
    QDateTime created;
    QDateTime lastModified;
    
public:
    bool isValid() const;
    
    // 序列化
    QString toICalString() const;
    static CalendarEvent fromICalString(const QString& ical);
    
    // 时间检查
    bool hasStarted(const QDateTime& now) const;
    bool hasEnded(const QDateTime& now) const;
    bool occurredOn(const QDate& date) const;
    
    // 比较
    bool operator==(const CalendarEvent& other) const;
};

using CalendarEventPtr = std::shared_ptr<CalendarEvent>;
using CalendarEventConstPtr = std::shared_ptr<const CalendarEvent>;

} // namespace PersonalCalendar::Core

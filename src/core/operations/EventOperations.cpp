// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "EventOperations.h"
#include <QDebug>
#include <QUuid>

namespace PersonalCalendar::Core {

EventOperations::EventOperations(ICalendarStoragePtr storage)
    : m_storage(storage)
{
    if (!m_storage) {
        qWarning() << QLatin1String("EventOperations: storage is nullptr!");
    }
}

void EventOperations::createEvent(const CalendarEventPtr& event,
                                  SuccessCallback onSuccess,
                                  ErrorCallback onError)
{
    if (!event) {
        handleError(QLatin1String("Event is nullptr"), onError);
        return;
    }
    
    if (!event->isValid()) {
        handleError(QLatin1String("Event is not valid"), onError);
        return;
    }
    
    if (!m_storage) {
        handleError(QLatin1String("Storage not initialized"), onError);
        return;
    }
    
    // 生成 UID（如果没有）
    if (event->uid.isEmpty()) {
        event->uid = QUuid::createUuid().toString();
    }
    
    // 设置创建和修改时间
    event->created = QDateTime::currentDateTime();
    event->lastModified = QDateTime::currentDateTime();
    
    // 调用存储实现
    bool success = m_storage->createEvent(event);
    if (success) {
        if (onSuccess) {
            onSuccess(event);
        }
    } else {
        handleError(QLatin1String("Failed to create event in storage"), onError);
    }
}

void EventOperations::updateEvent(const CalendarEventPtr& event,
                                  SuccessCallback onSuccess,
                                  ErrorCallback onError)
{
    if (!event) {
        handleError(QLatin1String("Event is nullptr"), onError);
        return;
    }
    
    if (!event->isValid()) {
        handleError(QLatin1String("Event is not valid"), onError);
        return;
    }
    
    if (!m_storage) {
        handleError(QLatin1String("Storage not initialized"), onError);
        return;
    }
    
    // 更新修改时间
    event->lastModified = QDateTime::currentDateTime();
    
    // 调用存储实现
    bool success = m_storage->updateEvent(event);
    if (success) {
        if (onSuccess) {
            onSuccess(event);
        }
    } else {
        handleError(QLatin1String("Failed to update event in storage"), onError);
    }
}

void EventOperations::deleteEvent(const QString& uid,
                                  SuccessCallback onSuccess,
                                  ErrorCallback onError)
{
    if (uid.isEmpty()) {
        handleError(QLatin1String("Event UID is empty"), onError);
        return;
    }
    
    if (!m_storage) {
        handleError(QLatin1String("Storage not initialized"), onError);
        return;
    }
    
    // 获取事件（用于回调）
    auto event = m_storage->getEvent(uid);
    
    // 调用存储实现
    bool success = m_storage->deleteEvent(uid);
    if (success) {
        if (onSuccess && event) {
            onSuccess(event);
        }
    } else {
        handleError(QLatin1String("Failed to delete event from storage"), onError);
    }
}

void EventOperations::getEvent(const QString& uid,
                               SuccessCallback onSuccess,
                               ErrorCallback onError)
{
    if (uid.isEmpty()) {
        handleError(QLatin1String("Event UID is empty"), onError);
        return;
    }
    
    if (!m_storage) {
        handleError(QLatin1String("Storage not initialized"), onError);
        return;
    }
    
    // 调用存储实现
    auto event = m_storage->getEvent(uid);
    if (event) {
        if (onSuccess) {
            onSuccess(event);
        }
    } else {
        handleError(QLatin1String("Event not found"), onError);
    }
}

void EventOperations::getEventsForDate(const QDate& date,
                                       EventListCallback onSuccess,
                                       ErrorCallback onError)
{
    if (!date.isValid()) {
        handleError(QLatin1String("Date is not valid"), onError);
        return;
    }
    
    if (!m_storage) {
        handleError(QLatin1String("Storage not initialized"), onError);
        return;
    }
    
    // 调用存储实现
    auto events = m_storage->getEventsByDate(date);
    if (onSuccess) {
        onSuccess(events);
    }
}

void EventOperations::getEventsForDateRange(const QDate& start,
                                            const QDate& end,
                                            EventListCallback onSuccess,
                                            ErrorCallback onError)
{
    if (!start.isValid() || !end.isValid()) {
        handleError(QLatin1String("Start or end date is not valid"), onError);
        return;
    }
    
    if (start > end) {
        handleError(QLatin1String("Start date is after end date"), onError);
        return;
    }
    
    if (!m_storage) {
        handleError(QLatin1String("Storage not initialized"), onError);
        return;
    }
    
    // 调用存储实现
    auto events = m_storage->getEventsByDateRange(start, end);
    if (onSuccess) {
        onSuccess(events);
    }
}

void EventOperations::handleError(const QString& error, ErrorCallback callback)
{
    qWarning() << QLatin1String("EventOperations error:") << error;
    if (callback) {
        callback(error);
    }
}

} // namespace PersonalCalendar::Core

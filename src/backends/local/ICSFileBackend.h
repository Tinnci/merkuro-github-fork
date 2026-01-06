// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "core/data/ICalendarStorage.h"
#include <QString>
#include <QMap>

namespace PersonalCalendar::Local {

/**
 * @brief Local ICS File Backend
 * 
 * Implements ICalendarStorage using local .ics files
 * Supports reading/writing standard iCalendar format
 */
class ICSFileBackend : public Core::ICalendarStorage {
public:
    /**
     * @brief Constructor
     * @param filePath Path to the .ics file
     */
    explicit ICSFileBackend(const QString& filePath);
    
    ~ICSFileBackend() override;
    
    // ICalendarStorage interface implementation
    bool createEvent(const Core::CalendarEventPtr& event) override;
    Core::CalendarEventPtr getEvent(const QString& uid) override;
    bool updateEvent(const Core::CalendarEventPtr& event) override;
    bool deleteEvent(const QString& uid) override;
    
    QList<Core::CalendarEventPtr> getEventsByDate(const QDate& date) override;
    QList<Core::CalendarEventPtr> getEventsByDateRange(
        const QDate& start, const QDate& end) override;
    QList<Core::CalendarEventPtr> getEventsByCollection(
        const QString& collectionId) override;
    
    QList<QString> getCalendarIds() override;
    QString getCalendarName(const QString& id) override;
    bool createCalendar(const QString& id, const QString& name) override;
    bool deleteCalendar(const QString& id) override;
    
    bool sync() override;
    bool isOnline() const override;
    QString getLastSyncTime(const QString& collectionId) override;
    
private:
    QString m_filePath;
    QMap<QString, Core::CalendarEventPtr> m_events;
    QString m_lastError;
    
    // File I/O
    bool loadFromFile();
    bool saveToFile();
    
    // iCalendar format
    QString generateICalendarContent();
    bool parseICalendarContent(const QString& content);
};

} // namespace PersonalCalendar::Local

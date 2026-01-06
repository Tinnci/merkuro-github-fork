// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "ICSFileBackend.h"
#include <QDir>
#include <QMap>

namespace PersonalCalendar::Local
{

/**
 * @brief Directory-based Multi-Calendar Backend
 *
 * Implements ICalendarStorage using a directory of .ics files
 * One .ics file per calendar, supports auto-discovery and management
 */
class DirectoryBackend : public Core::ICalendarStorage
{
public:
    /**
     * @brief Constructor
     * @param directoryPath Path to directory containing .ics files
     * @param createIfMissing Create directory if it doesn't exist
     */
    explicit DirectoryBackend(const QString &directoryPath, bool createIfMissing = true);

    ~DirectoryBackend() override;

    // ICalendarStorage interface implementation
    bool createEvent(const Core::CalendarEventPtr &event) override;
    Core::CalendarEventPtr getEvent(const QString &uid) override;
    bool updateEvent(const Core::CalendarEventPtr &event) override;
    bool deleteEvent(const QString &uid) override;

    QList<Core::CalendarEventPtr> getEventsByDate(const QDate &date) override;
    QList<Core::CalendarEventPtr> getEventsByDateRange(const QDate &start, const QDate &end) override;
    QList<Core::CalendarEventPtr> getEventsByCollection(const QString &collectionId) override;

    QList<QString> getCalendarIds() override;
    QString getCalendarName(const QString &id) override;
    bool createCalendar(const QString &id, const QString &name) override;
    bool deleteCalendar(const QString &id) override;

    // Extended Metadata Support
    void setCalendarColor(const QString &id, const QString &color);
    QString getCalendarColor(const QString &id);
    void setCalendarVisibility(const QString &id, bool visible);
    bool getCalendarVisibility(const QString &id);

    bool sync() override;
    bool isOnline() const override;
    QString getLastSyncTime(const QString &collectionId) override;

private:
    struct CalendarMetadata {
        QString name;
        QString color;
        bool visible = true;
    };

    QString m_directoryPath;
    QDir m_directory;

    // Map calendar ID to backend
    QMap<QString, std::shared_ptr<ICSFileBackend>> m_calendars;

    // Map calendar ID to metadata
    QMap<QString, CalendarMetadata> m_metadata;

    // Map UID to calendar ID
    QMap<QString, QString> m_eventToCalendar;

    QString m_lastError;

    // Initialization
    bool initialize();

    // Directory scanning
    bool discoverCalendars();

    // Metadata management
    bool loadCalendarMetadata();
    bool saveCalendarMetadata();
};

} // namespace PersonalCalendar::Local

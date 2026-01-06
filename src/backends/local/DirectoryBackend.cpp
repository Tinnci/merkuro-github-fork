// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "DirectoryBackend.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

namespace PersonalCalendar::Local
{

DirectoryBackend::DirectoryBackend(const QString &directoryPath, bool createIfMissing)
    : m_directoryPath(directoryPath), m_directory(directoryPath)
{
    if (!m_directory.exists()) {
        if (createIfMissing) {
            if (m_directory.mkpath(QLatin1String("."))) {
                qDebug() << "DirectoryBackend: Created directory:" << directoryPath;
            } else {
                m_lastError = QLatin1String("Failed to create directory");
                qWarning() << "DirectoryBackend: Cannot create directory:" << directoryPath;
                return;
            }
        } else {
            m_lastError = QLatin1String("Directory does not exist");
            qWarning() << "DirectoryBackend: Directory not found:" << directoryPath;
            return;
        }
    }

    initialize();
}

DirectoryBackend::~DirectoryBackend()
{
    // Clear calendars
    m_calendars.clear();
    qDebug() << "DirectoryBackend: Destroyed";
}

bool DirectoryBackend::initialize()
{
    // Discover existing calendars
    if (!discoverCalendars()) {
        return false;
    }

    // Load metadata
    if (!loadCalendarMetadata()) {
        qWarning() << "DirectoryBackend: Failed to load calendar metadata";
        // Continue anyway, will create default metadata
    }

    qDebug() << "DirectoryBackend: Initialized with" << m_calendars.size() << "calendars";
    return true;
}

bool DirectoryBackend::discoverCalendars()
{
    m_calendars.clear();
    m_eventToCalendar.clear();

    // Scan directory for .ics files
    m_directory.setFilter(QDir::Files);
    m_directory.setNameFilters({QLatin1String("*.ics")});

    const auto files = m_directory.entryList();
    for (const auto &filename : files) {
        QString filepath = m_directory.filePath(filename);
        QString calendarId = filename.left(filename.length() - 4); // Remove .ics

        auto backend = std::make_shared<ICSFileBackend>(filepath);
        m_calendars[calendarId] = backend;

        // Set default calendar name if not set
        if (!m_calendarNames.contains(calendarId)) {
            m_calendarNames[calendarId] = calendarId;
        }

        qDebug() << "DirectoryBackend: Discovered calendar:" << calendarId;
    }

    return true;
}

bool DirectoryBackend::createEvent(const Core::CalendarEventPtr &event)
{
    if (!event || !event->isValid()) {
        m_lastError = QLatin1String("Event is invalid");
        return false;
    }

    // Find which calendar this event belongs to
    // If not specified, use first calendar or create default
    QString calendarId = m_eventToCalendar.value(event->uid);
    if (calendarId.isEmpty()) {
        if (m_calendars.isEmpty()) {
            // Create default calendar
            if (!createCalendar(QLatin1String("default"), QLatin1String("Default"))) {
                return false;
            }
            calendarId = QLatin1String("default");
        } else {
            calendarId = m_calendars.keys().first();
        }
    }

    auto backend = m_calendars.value(calendarId);
    if (!backend) {
        m_lastError = QLatin1String("Calendar not found");
        return false;
    }

    bool success = backend->createEvent(event);
    if (success) {
        m_eventToCalendar[event->uid] = calendarId;
    }
    return success;
}

Core::CalendarEventPtr DirectoryBackend::getEvent(const QString &uid)
{
    if (uid.isEmpty()) {
        m_lastError = QLatin1String("UID is empty");
        return nullptr;
    }

    // Find which calendar contains this event
    QString calendarId = m_eventToCalendar.value(uid);
    if (calendarId.isEmpty()) {
        // Search all calendars
        for (auto it = m_calendars.begin(); it != m_calendars.end(); ++it) {
            auto event = it.value()->getEvent(uid);
            if (event) {
                m_eventToCalendar[uid] = it.key();
                return event;
            }
        }
        m_lastError = QLatin1String("Event not found");
        return nullptr;
    }

    auto backend = m_calendars.value(calendarId);
    if (backend) {
        return backend->getEvent(uid);
    }

    m_lastError = QLatin1String("Calendar not found");
    return nullptr;
}

bool DirectoryBackend::updateEvent(const Core::CalendarEventPtr &event)
{
    if (!event || !event->isValid()) {
        m_lastError = QLatin1String("Event is invalid");
        return false;
    }

    QString calendarId = m_eventToCalendar.value(event->uid);
    if (calendarId.isEmpty()) {
        m_lastError = QLatin1String("Event not found");
        return false;
    }

    auto backend = m_calendars.value(calendarId);
    if (!backend) {
        m_lastError = QLatin1String("Calendar not found");
        return false;
    }

    return backend->updateEvent(event);
}

bool DirectoryBackend::deleteEvent(const QString &uid)
{
    if (uid.isEmpty()) {
        m_lastError = QLatin1String("UID is empty");
        return false;
    }

    QString calendarId = m_eventToCalendar.value(uid);
    if (calendarId.isEmpty()) {
        m_lastError = QLatin1String("Event not found");
        return false;
    }

    auto backend = m_calendars.value(calendarId);
    if (!backend) {
        m_lastError = QLatin1String("Calendar not found");
        return false;
    }

    bool success = backend->deleteEvent(uid);
    if (success) {
        m_eventToCalendar.remove(uid);
    }
    return success;
}

QList<Core::CalendarEventPtr> DirectoryBackend::getEventsByDate(const QDate &date)
{
    QList<Core::CalendarEventPtr> result;

    if (!date.isValid()) {
        m_lastError = QLatin1String("Date is invalid");
        return result;
    }

    // Query all calendars
    for (auto it = m_calendars.begin(); it != m_calendars.end(); ++it) {
        result.append(it.value()->getEventsByDate(date));
    }

    return result;
}

QList<Core::CalendarEventPtr> DirectoryBackend::getEventsByDateRange(const QDate &start, const QDate &end)
{
    QList<Core::CalendarEventPtr> result;

    if (!start.isValid() || !end.isValid()) {
        m_lastError = QLatin1String("Date range is invalid");
        return result;
    }

    if (start > end) {
        m_lastError = QLatin1String("Start date is after end date");
        return result;
    }

    // Query all calendars
    for (auto it = m_calendars.begin(); it != m_calendars.end(); ++it) {
        result.append(it.value()->getEventsByDateRange(start, end));
    }

    return result;
}

QList<Core::CalendarEventPtr> DirectoryBackend::getEventsByCollection(const QString &collectionId)
{
    QList<Core::CalendarEventPtr> result;

    auto backend = m_calendars.value(collectionId);
    if (backend) {
        result = backend->getEventsByCollection(QLatin1String("local"));
    }

    return result;
}

QList<QString> DirectoryBackend::getCalendarIds()
{
    return m_calendars.keys();
}

QString DirectoryBackend::getCalendarName(const QString &id)
{
    return m_calendarNames.value(id, id);
}

bool DirectoryBackend::createCalendar(const QString &id, const QString &name)
{
    if (id.isEmpty()) {
        m_lastError = QLatin1String("Calendar ID is empty");
        return false;
    }

    if (m_calendars.contains(id)) {
        m_lastError = QLatin1String("Calendar already exists");
        return false;
    }

    // Create .ics file for new calendar
    QString filepath = m_directory.filePath(id + QLatin1String(".ics"));
    auto backend = std::make_shared<ICSFileBackend>(filepath);

    m_calendars[id] = backend;
    m_calendarNames[id] = name;

    // Save metadata
    saveCalendarMetadata();

    qDebug() << "DirectoryBackend: Created calendar:" << id;
    return true;
}

bool DirectoryBackend::deleteCalendar(const QString &id)
{
    if (id.isEmpty()) {
        m_lastError = QLatin1String("Calendar ID is empty");
        return false;
    }

    if (!m_calendars.contains(id)) {
        m_lastError = QLatin1String("Calendar not found");
        return false;
    }

    // Remove events from mapping
    for (auto it = m_eventToCalendar.begin(); it != m_eventToCalendar.end();) {
        if (it.value() == id) {
            it = m_eventToCalendar.erase(it);
        } else {
            ++it;
        }
    }

    // Delete .ics file
    QString filepath = m_directory.filePath(id + QLatin1String(".ics"));
    QFile file(filepath);
    if (file.exists()) {
        if (!file.remove()) {
            m_lastError = QLatin1String("Failed to delete calendar file");
            return false;
        }
    }

    // Remove calendar
    m_calendars.remove(id);
    m_calendarNames.remove(id);

    // Save metadata
    saveCalendarMetadata();

    qDebug() << "DirectoryBackend: Deleted calendar:" << id;
    return true;
}

bool DirectoryBackend::sync()
{
    // Rediscover calendars and reload
    if (!discoverCalendars()) {
        return false;
    }

    // Sync all calendars
    for (auto it = m_calendars.begin(); it != m_calendars.end(); ++it) {
        if (!it.value()->sync()) {
            return false;
        }
    }

    return true;
}

bool DirectoryBackend::isOnline() const
{
    return true;
}

QString DirectoryBackend::getLastSyncTime(const QString &collectionId)
{
    auto backend = m_calendars.value(collectionId);
    if (backend) {
        return backend->getLastSyncTime(QLatin1String("local"));
    }
    return QString();
}

bool DirectoryBackend::loadCalendarMetadata()
{
    // Load calendar names from metadata file
    QString metadataPath = m_directory.filePath(QLatin1String(".calendars.json"));
    QFile file(metadataPath);

    if (!file.exists()) {
        return true; // No metadata yet, that's OK
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    // Simple metadata format: calendarId=calendarName pairs
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) {
            continue;
        }

        int eqPos = line.indexOf(QLatin1Char('='));
        if (eqPos > 0) {
            QString id = line.left(eqPos);
            QString name = line.mid(eqPos + 1);
            if (m_calendars.contains(id)) {
                m_calendarNames[id] = name;
            }
        }
    }

    file.close();
    return true;
}

bool DirectoryBackend::saveCalendarMetadata()
{
    QString metadataPath = m_directory.filePath(QLatin1String(".calendars.json"));
    QFile file(metadataPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    stream << QLatin1String("# Calendar metadata\n");

    for (auto it = m_calendarNames.begin(); it != m_calendarNames.end(); ++it) {
        stream << it.key() << QLatin1Char('=') << it.value() << QLatin1Char('\n');
    }

    file.close();
    return true;
}

} // namespace PersonalCalendar::Local

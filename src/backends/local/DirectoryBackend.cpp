// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "DirectoryBackend.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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
    m_calendars.clear();
    qDebug() << "DirectoryBackend: Destroyed";
}

bool DirectoryBackend::initialize()
{
    if (!discoverCalendars()) {
        return false;
    }

    if (!loadCalendarMetadata()) {
        qWarning() << "DirectoryBackend: Failed to load calendar metadata";
    }

    qDebug() << "DirectoryBackend: Initialized with" << m_calendars.size() << "calendars";
    return true;
}

bool DirectoryBackend::discoverCalendars()
{
    m_calendars.clear();
    m_eventToCalendar.clear();

    m_directory.setFilter(QDir::Files);
    m_directory.setNameFilters({QLatin1String("*.ics")});

    const auto files = m_directory.entryList();
    for (const auto &filename : files) {
        QString filepath = m_directory.filePath(filename);
        QString calendarId = filename.left(filename.length() - 4); // Remove .ics

        auto backend = std::make_shared<ICSFileBackend>(filepath);
        m_calendars[calendarId] = backend;

        // Ensure metadata entry exists
        if (!m_metadata.contains(calendarId)) {
            m_metadata[calendarId] = CalendarMetadata{calendarId, QString(), true};
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

    QString calendarId = event->calendarId;
    if (calendarId.isEmpty() || !m_calendars.contains(calendarId)) {
        calendarId = m_eventToCalendar.value(event->uid);
    }

    if (calendarId.isEmpty()) {
        if (m_calendars.isEmpty()) {
            if (!createCalendar(QLatin1String("personal"), QLatin1String("Personal"))) {
                return false;
            }
            calendarId = QLatin1String("personal");
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

    QString calendarId = m_eventToCalendar.value(uid);
    if (calendarId.isEmpty()) {
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
    if (!date.isValid())
        return result;

    for (auto it = m_calendars.begin(); it != m_calendars.end(); ++it) {
        // Check visibility
        if (getCalendarVisibility(it.key())) {
            result.append(it.value()->getEventsByDate(date));
        }
    }
    return result;
}

QList<Core::CalendarEventPtr> DirectoryBackend::getEventsByDateRange(const QDate &start, const QDate &end)
{
    QList<Core::CalendarEventPtr> result;
    if (!start.isValid() || !end.isValid())
        return result;

    for (auto it = m_calendars.begin(); it != m_calendars.end(); ++it) {
        if (getCalendarVisibility(it.key())) {
            result.append(it.value()->getEventsByDateRange(start, end));
        }
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
    return m_metadata.value(id).name;
}

bool DirectoryBackend::createCalendar(const QString &id, const QString &name)
{
    if (id.isEmpty() || m_calendars.contains(id))
        return false;

    QString filepath = m_directory.filePath(id + QLatin1String(".ics"));
    auto backend = std::make_shared<ICSFileBackend>(filepath);

    m_calendars[id] = backend;
    m_metadata[id] = CalendarMetadata{name, QString(), true};

    saveCalendarMetadata();
    qDebug() << "DirectoryBackend: Created calendar:" << id;
    return true;
}

bool DirectoryBackend::deleteCalendar(const QString &id)
{
    if (id.isEmpty() || !m_calendars.contains(id))
        return false;

    // Remove events mapping
    for (auto it = m_eventToCalendar.begin(); it != m_eventToCalendar.end();) {
        if (it.value() == id) {
            it = m_eventToCalendar.erase(it);
        } else {
            ++it;
        }
    }

    QString filepath = m_directory.filePath(id + QLatin1String(".ics"));
    QFile::remove(filepath);

    m_calendars.remove(id);
    m_metadata.remove(id);

    saveCalendarMetadata();
    return true;
}

void DirectoryBackend::setCalendarColor(const QString &id, const QString &color)
{
    if (m_metadata.contains(id)) {
        m_metadata[id].color = color;
        saveCalendarMetadata();
    }
}

QString DirectoryBackend::getCalendarColor(const QString &id)
{
    return m_metadata.value(id).color;
}

void DirectoryBackend::setCalendarVisibility(const QString &id, bool visible)
{
    if (m_metadata.contains(id)) {
        m_metadata[id].visible = visible;
        saveCalendarMetadata();
    }
}

bool DirectoryBackend::getCalendarVisibility(const QString &id)
{
    if (m_metadata.contains(id)) {
        return m_metadata[id].visible;
    }
    return true; // Default to visible
}

bool DirectoryBackend::sync()
{
    if (!discoverCalendars())
        return false;
    for (auto it = m_calendars.begin(); it != m_calendars.end(); ++it) {
        if (!it.value()->sync())
            return false;
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
    return backend ? backend->getLastSyncTime(QLatin1String("local")) : QString();
}

bool DirectoryBackend::loadCalendarMetadata()
{
    QString path = m_directory.filePath(QLatin1String(".calendars.json"));
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull())
        return false;

    QJsonObject root = doc.object();
    QJsonObject calendars = root.value(QLatin1String("calendars")).toObject();

    for (auto it = calendars.begin(); it != calendars.end(); ++it) {
        QString id = it.key();
        QJsonObject meta = it.value().toObject();

        if (m_calendars.contains(id)) {
            m_metadata[id].name = meta.value(QLatin1String("name")).toString();
            m_metadata[id].color = meta.value(QLatin1String("color")).toString();
            m_metadata[id].visible = meta.value(QLatin1String("visible")).toBool(true);
        }
    }
    return true;
}

bool DirectoryBackend::saveCalendarMetadata()
{
    QString path = m_directory.filePath(QLatin1String(".calendars.json"));
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QJsonObject calendars;
    for (auto it = m_metadata.begin(); it != m_metadata.end(); ++it) {
        QJsonObject meta;
        meta.insert(QLatin1String("name"), it.value().name);
        meta.insert(QLatin1String("color"), it.value().color);
        meta.insert(QLatin1String("visible"), it.value().visible);
        calendars.insert(it.key(), meta);
    }

    QJsonObject root;
    root.insert(QLatin1String("calendars"), calendars);

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    return true;
}

} // namespace PersonalCalendar::Local
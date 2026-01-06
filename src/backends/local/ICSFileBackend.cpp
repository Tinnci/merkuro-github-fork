// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ICSFileBackend.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace PersonalCalendar::Local
{

ICSFileBackend::ICSFileBackend(const QString &filePath) : m_filePath(filePath)
{
    qDebug() << "ICSFileBackend: Loading from" << filePath;
    loadFromFile();
}

ICSFileBackend::~ICSFileBackend()
{
    // Save before destruction
    saveToFile();
    qDebug() << "ICSFileBackend: Destroyed and saved";
}

bool ICSFileBackend::createEvent(const Core::CalendarEventPtr &event)
{
    if (!event || !event->isValid()) {
        m_lastError = QLatin1String("Event is invalid");
        return false;
    }

    if (m_events.contains(event->uid)) {
        m_lastError = QLatin1String("Event with same UID already exists");
        return false;
    }

    m_events[event->uid] = event;
    return saveToFile();
}

Core::CalendarEventPtr ICSFileBackend::getEvent(const QString &uid)
{
    if (uid.isEmpty()) {
        m_lastError = QLatin1String("UID is empty");
        return nullptr;
    }

    auto it = m_events.find(uid);
    if (it != m_events.end()) {
        return it.value();
    }

    m_lastError = QLatin1String("Event not found");
    return nullptr;
}

bool ICSFileBackend::updateEvent(const Core::CalendarEventPtr &event)
{
    if (!event || !event->isValid()) {
        m_lastError = QLatin1String("Event is invalid");
        return false;
    }

    if (!m_events.contains(event->uid)) {
        m_lastError = QLatin1String("Event not found");
        return false;
    }

    m_events[event->uid] = event;
    return saveToFile();
}

bool ICSFileBackend::deleteEvent(const QString &uid)
{
    if (uid.isEmpty()) {
        m_lastError = QLatin1String("UID is empty");
        return false;
    }

    auto count = m_events.remove(uid);
    if (count == 0) {
        m_lastError = QLatin1String("Event not found");
        return false;
    }

    return saveToFile();
}

QList<Core::CalendarEventPtr> ICSFileBackend::getEventsByDate(const QDate &date)
{
    QList<Core::CalendarEventPtr> result;

    if (!date.isValid()) {
        m_lastError = QLatin1String("Date is invalid");
        return result;
    }

    for (const auto &event : m_events) {
        if (event->startDateTime.date() <= date && event->endDateTime.date() >= date) {
            result.append(event);
        }
    }

    return result;
}

QList<Core::CalendarEventPtr> ICSFileBackend::getEventsByDateRange(const QDate &start, const QDate &end)
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

    for (const auto &event : m_events) {
        if (event->endDateTime.date() >= start && event->startDateTime.date() <= end) {
            result.append(event);
        }
    }

    return result;
}

QList<Core::CalendarEventPtr> ICSFileBackend::getEventsByCollection(const QString &collectionId)
{
    // Local file backend has single collection (the file itself)
    if (collectionId == QLatin1String("local")) {
        return m_events.values();
    }
    return QList<Core::CalendarEventPtr>();
}

QList<QString> ICSFileBackend::getCalendarIds()
{
    return {QLatin1String("local")};
}

QString ICSFileBackend::getCalendarName(const QString &id)
{
    if (id == QLatin1String("local")) {
        return QLatin1String("Local Calendar");
    }
    return QString();
}

bool ICSFileBackend::createCalendar(const QString &id, const QString &name)
{
    // Local backend doesn't support multiple calendars
    m_lastError = QLatin1String("Cannot create calendars in local ICS backend");
    return false;
}

bool ICSFileBackend::deleteCalendar(const QString &id)
{
    // Local backend doesn't support deleting calendars
    m_lastError = QLatin1String("Cannot delete calendars in local ICS backend");
    return false;
}

bool ICSFileBackend::sync()
{
    // Reload from file and save
    if (!loadFromFile()) {
        return false;
    }
    return saveToFile();
}

bool ICSFileBackend::isOnline() const
{
    return true; // Local file is always available
}

QString ICSFileBackend::getLastSyncTime(const QString &collectionId)
{
    QFile file(m_filePath);
    if (file.exists()) {
        return file.fileTime(QFileDevice::FileModificationTime).toString(Qt::ISODate);
    }
    return QString();
}

bool ICSFileBackend::loadFromFile()
{
    QFile file(m_filePath);
    if (!file.exists()) {
        qWarning() << "ICSFileBackend: File does not exist:" << m_filePath;
        m_lastError = QLatin1String("File not found");
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QLatin1String("Cannot open file for reading");
        qWarning() << "ICSFileBackend: Cannot open file:" << m_filePath;
        return false;
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    // Parse iCalendar content
    if (!parseICalendarContent(content)) {
        m_lastError = QLatin1String("Failed to parse iCalendar content");
        return false;
    }

    qDebug() << "ICSFileBackend: Loaded" << m_events.size() << "events from" << m_filePath;
    return true;
}

bool ICSFileBackend::saveToFile()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QLatin1String("Cannot open file for writing");
        qWarning() << "ICSFileBackend: Cannot write to" << m_filePath;
        return false;
    }

    QTextStream stream(&file);
    QString content = generateICalendarContent();
    stream << content;
    file.close();

    qDebug() << "ICSFileBackend: Saved" << m_events.size() << "events to" << m_filePath;
    return true;
}

QString ICSFileBackend::generateICalendarContent()
{
    QString content;
    content += QLatin1String("BEGIN:VCALENDAR\n");
    content += QLatin1String("VERSION:2.0\n");
    content += QLatin1String("PRODID:-//Personal Calendar//EN\n");
    content += QLatin1String("CALSCALE:GREGORIAN\n");

    // Add all events
    for (const auto &event : m_events) {
        if (event && event->isValid()) {
            content += event->toICalString();
        }
    }

    content += QLatin1String("END:VCALENDAR\n");
    return content;
}

bool ICSFileBackend::parseICalendarContent(const QString &content)
{
    m_events.clear();

    // Simple iCalendar parser
    // Look for VEVENT blocks
    int pos = 0;
    while (true) {
        int start = content.indexOf(QLatin1String("BEGIN:VEVENT"), pos);
        if (start == -1) {
            break;
        }

        int end = content.indexOf(QLatin1String("END:VEVENT"), start);
        if (end == -1) {
            break;
        }

        QString eventBlock = content.mid(start, end - start + 10); // +10 for "END:VEVENT"

        // Parse event and create CalendarEvent
        auto event = std::make_shared<Core::CalendarEvent>();

        // Extract UID
        int uidPos = eventBlock.indexOf(QLatin1String("UID:"));
        if (uidPos != -1) {
            int uidEnd = eventBlock.indexOf(QLatin1Char('\n'), uidPos);
            event->uid = eventBlock.mid(uidPos + 4, uidEnd - uidPos - 4).trimmed();
        }

        // Extract SUMMARY (title)
        int summaryPos = eventBlock.indexOf(QLatin1String("SUMMARY:"));
        if (summaryPos != -1) {
            int summaryEnd = eventBlock.indexOf(QLatin1Char('\n'), summaryPos);
            event->title = eventBlock.mid(summaryPos + 8, summaryEnd - summaryPos - 8).trimmed();
        }

        // Add to map if valid
        if (!event->uid.isEmpty() && !event->title.isEmpty()) {
            m_events[event->uid] = event;
        }

        pos = end + 10;
    }

    return true;
}

} // namespace PersonalCalendar::Local

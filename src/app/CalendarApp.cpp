#include "CalendarApp.h"
#include "backends/local/DirectoryBackend.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

using namespace PersonalCalendar::Core;
using namespace PersonalCalendar::Local;

CalendarApp::CalendarApp(QObject *parent)
    : QObject(parent)
    , m_eventsModel(new EventsModel(this))
{
}

void CalendarApp::initialize(const QString &storagePath)
{
    QString path = storagePath;
    if (path.isEmpty()) {
        // Default to ~/.local/share/personal-calendar/calendars
        path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/calendars");
    }

    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }

    qDebug() << "Initializing DirectoryBackend at:" << path;
    m_storage = std::make_shared<DirectoryBackend>(path);
    
    // Ensure at least one calendar exists
    if (m_storage->getCalendarIds().isEmpty()) {
        qDebug() << "Creating default 'Personal' calendar";
        m_storage->createCalendar(QStringLiteral("personal"), QStringLiteral("Personal"));
    }

    m_eventsModel->setStorage(m_storage);
}

EventsModel* CalendarApp::eventsModel() const
{
    return m_eventsModel;
}

void CalendarApp::createEvent(const QString &title, const QDateTime &start, const QDateTime &end, bool allDay)
{
    if (!m_storage) return;

    auto event = std::make_shared<CalendarEvent>();
    event->title = title;
    event->startDateTime = start;
    event->endDateTime = end;
    event->isAllDay = allDay;
    
    // Set UUID (simple random for now)
    event->uid = QString::number(QDateTime::currentMSecsSinceEpoch()); 

    if (m_storage->createEvent(event)) {
        qDebug() << "Event created:" << title;
        m_eventsModel->refresh();
    } else {
        qWarning() << "Failed to create event";
    }
}

void CalendarApp::updateEvent(const QString &uid, const QString &title, const QDateTime &start, const QDateTime &end, bool allDay)
{
    if (!m_storage) return;

    auto event = m_storage->getEvent(uid);
    if (!event) {
        qWarning() << "Event not found for update:" << uid;
        return;
    }

    event->title = title;
    event->startDateTime = start;
    event->endDateTime = end;
    event->isAllDay = allDay;

    if (m_storage->updateEvent(event)) {
        qDebug() << "Event updated:" << uid;
        m_eventsModel->refresh();
    } else {
        qWarning() << "Failed to update event";
    }
}

void CalendarApp::deleteEvent(const QString &uid)
{
    if (!m_storage) return;

    if (m_storage->deleteEvent(uid)) {
        qDebug() << "Event deleted:" << uid;
        m_eventsModel->refresh();
    }
}

void CalendarApp::createCalendar(const QString &name)
{
    if (!m_storage) return;
    
    // Simple ID generation
    QString id = name.toLower().replace(QStringLiteral(" "), QStringLiteral("-"));
    m_storage->createCalendar(id, name);
}

QStringList CalendarApp::getCalendarNames()
{
    QStringList names;
    if (!m_storage) return names;

    for (const auto &id : m_storage->getCalendarIds()) {
        names << m_storage->getCalendarName(id);
    }
    return names;
}

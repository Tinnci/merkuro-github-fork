#include "CalendarApp.h"
#include "backends/local/DirectoryBackend.h"
#ifdef AKONADI_BACKEND_AVAILABLE
#include "backends/akonadi/AkonadiCalendarBackend.h"
#endif
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

using namespace PersonalCalendar::Core;
using namespace PersonalCalendar::Local;

CalendarApp::CalendarApp(QObject *parent)
    : QObject(parent), m_eventsModel(new EventsModel(this)), m_monthModel(new MonthModel(this))
{
    // Sync models
    connect(m_monthModel, &MonthModel::selectedChanged, this,
            [this]() { m_eventsModel->setSelectedDate(m_monthModel->selected()); });

    // Initial sync
    m_eventsModel->setSelectedDate(m_monthModel->selected());
}

void CalendarApp::initialize(const QString &storagePath)
{
#ifdef AKONADI_BACKEND_AVAILABLE
    if (qEnvironmentVariable("PERSONAL_CALENDAR_BACKEND") == QLatin1String("akonadi")) {
        qDebug() << "Initializing AkonadiBackend";
        m_storage = std::make_shared<PersonalCalendar::Akonadi::AkonadiCalendarBackend>(this);
        m_eventsModel->setStorage(m_storage);
        m_monthModel->setStorage(m_storage);
        return;
    }
#endif

    QString path = storagePath;
    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/calendars");
    }

    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }

    qDebug() << "Initializing DirectoryBackend at:" << path;
    auto backend = std::make_shared<DirectoryBackend>(path);
    m_storage = backend;

    if (m_storage->getCalendarIds().isEmpty()) {
        qDebug() << "Creating default 'Personal' calendar";
        // Default color Blue
        backend->createCalendar(QStringLiteral("personal"), QStringLiteral("Personal"));
        backend->setCalendarColor(QStringLiteral("personal"), QStringLiteral("#2196F3"));
    }

    m_eventsModel->setStorage(m_storage);
    m_monthModel->setStorage(m_storage);
}

EventsModel *CalendarApp::eventsModel() const
{
    return m_eventsModel;
}

MonthModel *CalendarApp::monthModel() const
{
    return m_monthModel;
}

void CalendarApp::createEvent(const QString &title, const QDateTime &start, const QDateTime &end, bool allDay,
                              const QString &description, const QString &location, const QString &calendarId)
{
    if (!m_storage) return;

    auto event = std::make_shared<CalendarEvent>();
    event->title = title;
    event->description = description;
    event->location = location;
    event->startDateTime = start;
    event->endDateTime = end;
    event->isAllDay = allDay;
    event->calendarId = calendarId;
    event->uid = QString::number(QDateTime::currentMSecsSinceEpoch());

    if (m_storage->createEvent(event)) {
        qDebug() << "Event created:" << title;
        m_eventsModel->refresh();
    } else {
        qWarning() << "Failed to create event";
    }
}

void CalendarApp::updateEvent(const QString &uid, const QString &title, const QDateTime &start, const QDateTime &end,
                              bool allDay, const QString &description, const QString &location)
{
    if (!m_storage) return;

    auto event = m_storage->getEvent(uid);
    if (!event) return;

    event->title = title;
    event->description = description;
    event->location = location;
    event->startDateTime = start;
    event->endDateTime = end;
    event->isAllDay = allDay;

    if (m_storage->updateEvent(event)) {
        qDebug() << "Event updated:" << uid;
        m_eventsModel->refresh();
    }
}

void CalendarApp::deleteEvent(const QString &uid)
{
    if (m_storage && m_storage->deleteEvent(uid)) {
        m_eventsModel->refresh();
    }
}

void CalendarApp::createCalendar(const QString &name, const QString &color)
{
    if (!m_storage) return;

    auto backend = std::dynamic_pointer_cast<DirectoryBackend>(m_storage);
    if (!backend) return;

    QString id = name.toLower().replace(QStringLiteral(" "), QStringLiteral("-"));
    
    // Ensure unique ID
    if (backend->getCalendarIds().contains(id)) {
        id += QString::number(QDateTime::currentMSecsSinceEpoch() % 1000);
    }

    if (backend->createCalendar(id, name)) {
        backend->setCalendarColor(id, color);
        qDebug() << "Created calendar:" << name << color;
    }
}

void CalendarApp::deleteCalendar(const QString &id)
{
    if (!m_storage) return;
    m_storage->deleteCalendar(id);
    m_eventsModel->refresh();
}

void CalendarApp::setCalendarColor(const QString &id, const QString &color)
{
    auto backend = std::dynamic_pointer_cast<DirectoryBackend>(m_storage);
    if (backend) {
        backend->setCalendarColor(id, color);
        m_eventsModel->refresh(); // Refresh needed if events use calendar color
    }
}

void CalendarApp::setCalendarVisibility(const QString &id, bool visible)
{
    auto backend = std::dynamic_pointer_cast<DirectoryBackend>(m_storage);
    if (backend) {
        backend->setCalendarVisibility(id, visible);
        m_eventsModel->refresh(); // Refresh to hide/show events
    }
}

QVariantList CalendarApp::getCalendars()
{
    QVariantList list;
    auto backend = std::dynamic_pointer_cast<DirectoryBackend>(m_storage);
    if (!backend) return list;

    for (const auto &id : backend->getCalendarIds()) {
        QVariantMap map;
        map[QStringLiteral("id")] = id;
        map[QStringLiteral("name")] = backend->getCalendarName(id);
        map[QStringLiteral("color")] = backend->getCalendarColor(id);
        map[QStringLiteral("visible")] = backend->getCalendarVisibility(id);
        list.append(map);
    }
    return list;
}

QStringList CalendarApp::getCalendarNames()
{
    QStringList names;
    if (m_storage) {
        for (const auto &id : m_storage->getCalendarIds()) {
            names << m_storage->getCalendarName(id);
        }
    }
    return names;
}
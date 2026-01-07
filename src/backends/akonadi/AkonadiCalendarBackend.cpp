// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "AkonadiCalendarBackend.h"
#include "AkonadiDataConverter.h"
#include <Akonadi/ETMCalendar>
#include <Akonadi/IncidenceChanger>
#include <KCalendarCore/Event>
#include <KCalendarCore/Todo>
#include <QDebug>
#include <QEventLoop>

namespace PersonalCalendar::Akonadi
{

AkonadiCalendarBackend::AkonadiCalendarBackend(QObject *parent) : Core::ICalendarStorage()
{
    qDebug() << "AkonadiCalendarBackend: Initializing...";
    initialize();
}

AkonadiCalendarBackend::~AkonadiCalendarBackend()
{
    qDebug() << "AkonadiCalendarBackend: Destroyed";
}

bool AkonadiCalendarBackend::initialize()
{
    // Create ETMCalendar (Entity Tree Model Calendar)
    // This loads the Akonadi calendar data into a local model
    m_akonadiCalendar = std::make_shared<::Akonadi::ETMCalendar>();

    // Create IncidenceChanger for modifying data
    m_incidenceChanger = std::make_shared<::Akonadi::IncidenceChanger>();
    m_incidenceChanger->setShowDialogsOnError(false);

    // Initial load might be async, but ETMCalendar usually starts populating immediately
    // For a robust app, we should connect to loadingFinished signals

    return true;
}

bool AkonadiCalendarBackend::createEvent(const Core::CalendarEventPtr &event)
{
    if (!event || !event->isValid()) {
        m_lastError = QLatin1String("Event is invalid");
        return false;
    }

    auto incidence = AkonadiDataConverter::toAkonadiIncidence(event);
    if (!incidence) {
        m_lastError = QLatin1String("Conversion failed");
        return false;
    }

    // Use default collection if none specified
    // In a real app, we would resolve event->calendarId to a Collection::Id
    ::Akonadi::Collection collection;
    if (!event->calendarId.isEmpty()) {
        collection = ::Akonadi::Collection(event->calendarId.toLongLong());
    } else {
        // Fallback to default collection from ETMCalendar?
        // For now, let IncidenceChanger handle default or ask user (we can't ask user here)
        // We pick the first writable collection found
        // This is a simplification
    }

    // Create via IncidenceChanger
    // Note: This is asynchronous. We can't easily return success/fail here without blocking.
    // For now, we assume success if we trigger the change.
    m_incidenceChanger->createIncidence(incidence, collection);

    return true;
}

Core::CalendarEventPtr AkonadiCalendarBackend::getEvent(const QString &uid)
{
    if (uid.isEmpty()) {
        m_lastError = QLatin1String("UID is empty");
        return nullptr;
    }

    auto incidence = m_akonadiCalendar->incidence(uid);
    if (!incidence) {
        return nullptr;
    }

    return AkonadiDataConverter::fromAkonadiIncidence(incidence);
}

bool AkonadiCalendarBackend::updateEvent(const Core::CalendarEventPtr &event)
{
    if (!event || !event->isValid()) {
        m_lastError = QLatin1String("Event is invalid");
        return false;
    }

    auto newIncidence = AkonadiDataConverter::toAkonadiIncidence(event);
    auto item = m_akonadiCalendar->item(event->uid);

    if (!item.isValid()) {
        m_lastError = QLatin1String("Event not found for update");
        return false;
    }

    // modifyIncidence takes the original Item and the new Incidence payload
    m_incidenceChanger->modifyIncidence(item, newIncidence);

    return true;
}

bool AkonadiCalendarBackend::deleteEvent(const QString &uid)
{
    if (uid.isEmpty()) {
        m_lastError = QLatin1String("UID is empty");
        return false;
    }

    auto item = m_akonadiCalendar->item(uid);
    if (!item.isValid()) {
        m_lastError = QLatin1String("Event not found");
        return false;
    }

    m_incidenceChanger->deleteIncidence(item);
    return true;
}

QList<Core::CalendarEventPtr> AkonadiCalendarBackend::getEventsByDate(const QDate &date)
{
    QList<Core::CalendarEventPtr> events;
    if (!date.isValid())
        return events;

    // ETMCalendar doesn't have a direct "events on date" excluding ranges,
    // but events(date, date) should work.
    auto incidences = m_akonadiCalendar->events(date, date);

    for (const auto &incidence : incidences) {
        events.append(
            AkonadiDataConverter::fromAkonadiIncidence(qSharedPointerCast<KCalendarCore::Incidence>(incidence)));
    }
    return events;
}

QList<Core::CalendarEventPtr> AkonadiCalendarBackend::getEventsByDateRange(const QDate &start, const QDate &end)
{
    QList<Core::CalendarEventPtr> events;
    if (!start.isValid() || !end.isValid())
        return events;

    auto incidences = m_akonadiCalendar->events(start, end);

    for (const auto &incidence : incidences) {
        events.append(
            AkonadiDataConverter::fromAkonadiIncidence(qSharedPointerCast<KCalendarCore::Incidence>(incidence)));
    }
    return events;
}

QList<Core::CalendarEventPtr> AkonadiCalendarBackend::getEventsByCollection(const QString &collectionId)
{
    // ETMCalendar model structure is complex.
    // We would need to iterate the model for items with parent collection ID.
    // Simplifying: Return empty for now or implement full model scan.
    return QList<Core::CalendarEventPtr>();
}

QList<QString> AkonadiCalendarBackend::getCalendarIds()
{
    QList<QString> ids;
    // Iterate ETMCalendar model to find collections
    // TODO: access the model directly via m_akonadiCalendar->model()
    return ids;
}

QString AkonadiCalendarBackend::getCalendarName(const QString &id)
{
    // Need model access
    return QString();
}

bool AkonadiCalendarBackend::createCalendar(const QString &id, const QString &name)
{
    // Creating collections in Akonadi is done via CollectionCreateJob
    // Not implemented in this phase
    Q_UNUSED(id);
    Q_UNUSED(name);
    return false;
}

bool AkonadiCalendarBackend::deleteCalendar(const QString &id)
{
    // CollectionDeleteJob
    Q_UNUSED(id);
    return false;
}

QString AkonadiCalendarBackend::getCalendarColor(const QString &id)
{
    // Akonadi stores colors in attributes
    return QLatin1String("#9C27B0"); // Default Purple for Akonadi
}

void AkonadiCalendarBackend::setCalendarColor(const QString &id, const QString &color)
{
    Q_UNUSED(id);
    Q_UNUSED(color);
}

bool AkonadiCalendarBackend::getCalendarVisibility(const QString &id)
{
    return true;
}

void AkonadiCalendarBackend::setCalendarVisibility(const QString &id, bool visible)
{
    Q_UNUSED(id);
    Q_UNUSED(visible);
}

bool AkonadiCalendarBackend::sync()
{
    // Akonadi handles sync automatically
    return true;
}

bool AkonadiCalendarBackend::isOnline() const
{
    return true;
}

QString AkonadiCalendarBackend::getLastSyncTime(const QString &collectionId)
{
    Q_UNUSED(collectionId);
    return QString();
}

} // namespace PersonalCalendar::Akonadi

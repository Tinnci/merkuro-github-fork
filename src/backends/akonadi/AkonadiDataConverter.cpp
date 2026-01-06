// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "AkonadiDataConverter.h"

// Note: These headers are only available when KDE dependencies are installed
#if __has_include(<KCalendarCore/Event>)
#include <KCalendarCore/Event>
#include <KCalendarCore/Todo>
#include <KCalendarCore/Attendee>
#include <KCalendarCore/Alarm>
#include <KCalendarCore/Recurrence>
#else
// Stub headers or errors would happen here if we forced compilation
// But CMake protects us.
#endif

#include <QDebug>

namespace PersonalCalendar::Akonadi {

using namespace KCalendarCore;

IncidencePtr AkonadiDataConverter::toAkonadiIncidence(const Core::CalendarEventPtr& event)
{
#if defined(KCALENDARCORE_FOUND) || defined(KF5CalendarCore_FOUND)
    if (!event) return nullptr;

    IncidencePtr incidence;

    // Factory based on type
    if (event->type == Core::EventType::Todo) {
        incidence = IncidencePtr(new Todo());
    } else {
        incidence = IncidencePtr(new Event());
    }

    incidence->setUid(event->uid);
    incidence->setSummary(event->title);
    incidence->setDescription(event->description);
    incidence->setLocation(event->location);
    incidence->setAllDay(event->isAllDay);
    incidence->setDtStart(event->startDateTime);
    
    // For Events, we set dtEnd. For Todos, it might be due date.
    if (event->type == Core::EventType::Event) {
        std::static_pointer_cast<Event>(incidence)->setDtEnd(event->endDateTime);
    } else if (event->type == Core::EventType::Todo) {
        std::static_pointer_cast<Todo>(incidence)->setDtDue(event->endDateTime);
    }

    // Convert Recurrence
    convertRecurrenceRule(event->recurrence, incidence.get());

    return incidence;
#else
    Q_UNUSED(event);
    qWarning() << "KCalendarCore not available, returning nullptr";
    return nullptr;
#endif
}

Core::CalendarEventPtr AkonadiDataConverter::fromAkonadiIncidence(const IncidencePtr& incidence)
{
#if defined(KCALENDARCORE_FOUND) || defined(KF5CalendarCore_FOUND)
    if (!incidence) return nullptr;

    auto event = std::make_shared<Core::CalendarEvent>();

    event->uid = incidence->uid();
    event->title = incidence->summary();
    event->description = incidence->description();
    event->location = incidence->location();
    event->isAllDay = incidence->allDay();
    event->startDateTime = incidence->dtStart();

    if (incidence->type() == Incidence::TypeTodo) {
        event->type = Core::EventType::Todo;
        event->endDateTime = std::static_pointer_cast<Todo>(incidence)->dtDue();
    } else {
        event->type = Core::EventType::Event;
        event->endDateTime = std::static_pointer_cast<Event>(incidence)->dtEnd();
    }

    // Convert Recurrence
    event->recurrence = convertRecurrenceRule(incidence.get());

    return event;
#else
    Q_UNUSED(incidence);
    qWarning() << "KCalendarCore not available, returning nullptr";
    return nullptr;
#endif
}

void AkonadiDataConverter::convertRecurrenceRule(const Core::Recurrence& coreRec, Incidence* incidence)
{
#if defined(KCALENDARCORE_FOUND) || defined(KF5CalendarCore_FOUND)
    if (coreRec.pattern == Core::Recurrence::Pattern::None) {
        return;
    }

    Recurrence* r = incidence->recurrence();
    switch (coreRec.pattern) {
        case Core::Recurrence::Pattern::Daily:
            r->setDaily(coreRec.interval);
            break;
        case Core::Recurrence::Pattern::Weekly:
            r->setWeekly(coreRec.interval);
            break;
        case Core::Recurrence::Pattern::Monthly:
            r->setMonthly(coreRec.interval);
            break;
        case Core::Recurrence::Pattern::Yearly:
            r->setYearly(coreRec.interval);
            break;
        default:
            break;
    }

    if (coreRec.endDate.isValid()) {
        r->setEndDate(coreRec.endDate);
    }
#else
    Q_UNUSED(coreRec);
    Q_UNUSED(incidence);
#endif
}

Core::Recurrence AkonadiDataConverter::convertRecurrenceRule(const Incidence* incidence)
{
    Core::Recurrence rec;
#if defined(KCALENDARCORE_FOUND) || defined(KF5CalendarCore_FOUND)
    Recurrence* r = incidence->recurrence();
    if (!r || r->recurrenceType() == Recurrence::rNone) {
        return rec;
    }

    rec.interval = r->frequency();
    
    switch (r->recurrenceType()) {
        case Recurrence::rDaily:
            rec.pattern = Core::Recurrence::Pattern::Daily;
            break;
        case Recurrence::rWeekly:
            rec.pattern = Core::Recurrence::Pattern::Weekly;
            break;
        case Recurrence::rMonthly:
            rec.pattern = Core::Recurrence::Pattern::Monthly;
            break;
        case Recurrence::rYearly:
            rec.pattern = Core::Recurrence::Pattern::Yearly;
            break;
        default:
            rec.pattern = Core::Recurrence::Pattern::None;
            break;
    }

    rec.endDate = r->endDate();
#else
    Q_UNUSED(incidence);
#endif
    return rec;
}

// ... Implement attendees/alarms similarly

void AkonadiDataConverter::convertAttendees(const QList<Core::Attendee>& coreAttendees, KCalendarCore::Incidence* akonadiIncidence)
{
    // Implementation placeholder
}

QList<Core::Attendee> AkonadiDataConverter::convertAttendees(const KCalendarCore::Incidence* akonadiIncidence)
{
    return QList<Core::Attendee>();
}

void AkonadiDataConverter::convertAlarms(const QList<Core::Alarm>& coreAlarms, KCalendarCore::Incidence* akonadiIncidence)
{
    // Implementation placeholder
}

QList<Core::Alarm> AkonadiDataConverter::convertAlarms(const KCalendarCore::Incidence* akonadiIncidence)
{
    return QList<Core::Alarm>();
}

} // namespace PersonalCalendar::Akonadi
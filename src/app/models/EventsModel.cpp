#include "EventsModel.h"
#include <QDebug>

using namespace PersonalCalendar::Core;

EventsModel::EventsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_selectedDate(QDate::currentDate())
{
}

void EventsModel::setStorage(ICalendarStoragePtr storage)
{
    m_storage = storage;
    updateEvents();
}

int EventsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_events.count();
}

QVariant EventsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_events.count())
        return QVariant();

    const auto &event = m_events[index.row()];

    switch (role) {
    case UidRole:
        return event->uid;
    case TitleRole:
        return event->title;
    case DescriptionRole:
        return event->description;
    case StartDateRole:
        return event->startDateTime;
    case EndDateRole:
        return event->endDateTime;
    case IsAllDayRole:
        return event->isAllDay;
    case LocationRole:
        return event->location;
    case CalendarIdRole:
        return event->calendarId;
    }

    return QVariant();
}

QHash<int, QByteArray> EventsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UidRole] = QByteArrayLiteral("uid");
    roles[TitleRole] = QByteArrayLiteral("title");
    roles[DescriptionRole] = QByteArrayLiteral("description");
    roles[StartDateRole] = QByteArrayLiteral("startDate");
    roles[EndDateRole] = QByteArrayLiteral("endDate");
    roles[IsAllDayRole] = QByteArrayLiteral("isAllDay");
    roles[LocationRole] = QByteArrayLiteral("location");
    roles[CalendarIdRole] = QByteArrayLiteral("calendarId");
    return roles;
}

QDate EventsModel::selectedDate() const
{
    return m_selectedDate;
}

void EventsModel::setSelectedDate(const QDate &date)
{
    if (m_selectedDate != date) {
        m_selectedDate = date;
        emit selectedDateChanged();
        updateEvents();
    }
}

void EventsModel::refresh()
{
    updateEvents();
}

void EventsModel::updateEvents()
{
    if (!m_storage) return;

    beginResetModel();
    // Fetch events for the selected date
    // Note: getEventsByDate returns events that occur on this date (including spanning events)
    m_events = m_storage->getEventsByDate(m_selectedDate);
    
    // Sort by start time
    std::sort(m_events.begin(), m_events.end(), [](const CalendarEventPtr &a, const CalendarEventPtr &b) {
        return a->startDateTime < b->startDateTime;
    });
    
    endResetModel();
    
    qDebug() << "Loaded" << m_events.size() << "events for" << m_selectedDate;
}

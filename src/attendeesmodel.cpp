// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QMetaEnum>
#include "attendeesmodel.h"

AttendeesModel::AttendeesModel(QObject* parent, KCalendarCore::Event::Ptr eventPtr)
    : QAbstractItemModel(parent)
    , m_event(eventPtr)
{

}

KCalendarCore::Event::Ptr AttendeesModel::eventPtr()
{
    return m_event;
}

void AttendeesModel::setEventPtr(KCalendarCore::Event::Ptr event)
{
    if (m_event == event) {
        return;
    }
    m_event = event;
    Q_EMIT eventPtrChanged();
}

KCalendarCore::Attendee::List AttendeesModel::attendees()
{
    return m_event->attendees();
}

QVariant AttendeesModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }
    auto attendee = m_event->attendees()[idx.row()];
    switch (role) {
        case CuType:
            return attendee.cuType();
        case Delegate:
            return attendee.delegate();
        case Delegator:
            return attendee.delegator();
        case Email:
            return attendee.email();
        case FullName:
            return attendee.fullName();
        case isNull:
            return attendee.isNull();
        case Name:
            return attendee.name();
        case Role:
            return attendee.role();
        case RSVP:
            return attendee.RSVP();
        case Status:
            return attendee.status();
        case Uid:
            return attendee.uid();
        default:
            qWarning() << "Unknown role for event:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
    }
}

QModelIndex AttendeesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column);
    }
    return {};
}

QModelIndex AttendeesModel::parent(const QModelIndex &) const
{
    return {};
}

int AttendeesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_event->attendeeCount();
    }
    return 0;
}

int AttendeesModel::columnCount(const QModelIndex &) const
{
    // Our data is a list, so it's one dimensional and only has 1 column.
    return 1;
}

void AttendeesModel::addAttendee(QString name, QString email)
{
    KCalendarCore::Attendee attendee(name, email);
    m_event->addAttendee(attendee);
    Q_EMIT attendeesChanged();
    Q_EMIT layoutChanged();
    qDebug() << rowCount();
}

void AttendeesModel::deleteAttendee(int row)
{
    qDebug() << row;
    if (!hasIndex(row, 0)) {
        return;
    }
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees.removeAt(row);
    m_event->setAttendees(currentAttendees);
    rowCount();
    Q_EMIT attendeesChanged();
    Q_EMIT layoutChanged();
}


void AttendeesModel::setAttendeeCuType(int row, KCalendarCore::Attendee::CuType cutype)
{
    m_event->attendees()[row].setCuType(cutype);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeDelegate(int row, QString delegate)
{
    m_event->attendees()[row].setDelegate(delegate);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeDelegator(int row, QString delegator)
{
    m_event->attendees()[row].setDelegator(delegator);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeEmail(int row, QString email)
{
    m_event->attendees()[row].setEmail(email);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeName(int row, QString name)
{
    m_event->attendees()[row].setName(name);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeRole(int row, KCalendarCore::Attendee::Role role)
{
    m_event->attendees()[row].setRole(role);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeRSVP(int row, bool rsvp)
{
    m_event->attendees()[row].setRSVP(rsvp);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeStatus(int row, KCalendarCore::Attendee::PartStat status)
{
    m_event->attendees()[row].setStatus(status);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeUid(int row, QString uid)
{
    m_event->attendees()[row].setUid(uid);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

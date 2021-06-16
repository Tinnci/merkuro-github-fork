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
        return m_event->attendees().size();
    }
    return 0;
}

int AttendeesModel::columnCount(const QModelIndex &) const
{
    // Our data is a list, so it's one dimensional and only has 1 column.
    return 1;
}


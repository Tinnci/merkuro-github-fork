// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QMetaEnum>
#include <QRegularExpression>
#include "attendeesmodel.h"

AttendeeStatusModel::AttendeeStatusModel(QObject *parent)
    : QAbstractItemModel(parent)
{

}

QVariant AttendeeStatusModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }

    int value = QMetaEnum::fromType<KCalendarCore::Attendee::PartStat>().value(idx.row());
    // Workaround for QT_NO_CAST_FROM_ASCII
    QString enumName = QLatin1String(QMetaEnum::fromType<KCalendarCore::Attendee::PartStat>().valueToKey(value));

    switch (role) {
        case EnumName:
            return enumName;
        case DisplayName:
            // Regular expression adds space between every lowercase and Capitalised character
            return enumName.replace(QRegularExpression(QLatin1String("([a-z])([A-Z])")), QLatin1String("\\1 \\2"));
        case Value:
            return value;
        default:
            qWarning() << "Unknown role for event:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
    }
}

QHash<int, QByteArray> AttendeeStatusModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    for (int i = 0; i < QMetaEnum::fromType<Roles>().keyCount(); i++) {
        roles.insert(Qt::UserRole + i + 1, QMetaEnum::fromType<Roles>().key(i));
    }
    return roles;
}

QModelIndex AttendeeStatusModel::index(int row, int column, const QModelIndex &parent) const
{

    if (!hasIndex(row, column, parent)) {
        qWarning() << "Invalid index: " << row;
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column);
    }
    return {};
}

QModelIndex AttendeeStatusModel::parent(const QModelIndex &) const
{
    return {};
}

int AttendeeStatusModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return QMetaEnum::fromType<KCalendarCore::Attendee::PartStat>().keyCount();
    }
    return 0;
}

int AttendeeStatusModel::columnCount(const QModelIndex &) const
{
    return 1;
}








AttendeesModel::AttendeesModel(QObject* parent, KCalendarCore::Event::Ptr eventPtr)
    : QAbstractItemModel(parent)
    , m_event(eventPtr)
    , m_attendeeStatusModel(parent)
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

AttendeeStatusModel * AttendeesModel::attendeeStatusModel()
{
    return &m_attendeeStatusModel;
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

QHash<int, QByteArray> AttendeesModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	for (int i = 0; i < QMetaEnum::fromType<Roles>().keyCount(); i++) {
		roles.insert(Qt::UserRole + i + 1, QMetaEnum::fromType<Roles>().key(i));
	}
	return roles;
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

void AttendeesModel::addAttendee()
{
    // QLatin1String is a workaround for QT_NO_CAST_FROM_ASCII
    KCalendarCore::Attendee attendee(QLatin1String(""), QLatin1String(""));
    m_event->addAttendee(attendee);
    Q_EMIT attendeesChanged();
    Q_EMIT layoutChanged();
}

void AttendeesModel::deleteAttendee(int row)
{
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

// When modifying attendees, remember you cannot change them directly from m_event->attendees (is a const).

void AttendeesModel::setAttendeeCuType(int row, KCalendarCore::Attendee::CuType cutype)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setCuType(cutype);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeDelegate(int row, QString delegate)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setDelegate(delegate);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeDelegator(int row, QString delegator)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setDelegator(delegator);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeEmail(int row, QString email)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setEmail(email);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeName(int row, QString name)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setName(name);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeRole(int row, KCalendarCore::Attendee::Role role)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setRole(role);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeRSVP(int row, bool rsvp)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setRSVP(rsvp);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeStatus(int row, KCalendarCore::Attendee::PartStat status)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setStatus(status);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void AttendeesModel::setAttendeeUid(int row, QString uid)
{
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());
    currentAttendees[row].setUid(uid);
    m_event->setAttendees(currentAttendees);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

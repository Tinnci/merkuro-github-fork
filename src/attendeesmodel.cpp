// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QMetaEnum>
#include <QRegularExpression>
#include "attendeesmodel.h"

AttendeeStatusModel::AttendeeStatusModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

QVariant AttendeeStatusModel::data(const QModelIndex &idx, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    int value = QMetaEnum::fromType<KCalendarCore::Attendee::PartStat>().value(idx.row());
    // QLatin1String is a workaround for QT_NO_CAST_FROM_ASCII
    QString enumName = QLatin1String(QMetaEnum::fromType<KCalendarCore::Attendee::PartStat>().valueToKey(value));

    switch (role) {
        case EnumNameRole:
            return enumName;
        case DisplayNameRole:
        {
            // Regular expression adds space between every lowercase and Capitalised character then does the same
            // for capitalised letters together, e.g. ThisIsATest. Not a problem right now, but best to be safe.
            QString displayName = enumName.replace(QRegularExpression(QLatin1String("Role")), QLatin1String(""));
            displayName.replace(QRegularExpression(QLatin1String("([a-z])([A-Z])")), QLatin1String("\\1 \\2"));
            displayName.replace(QRegularExpression(QLatin1String("([A-Z])([A-Z])")), QLatin1String("\\1 \\2"));
            return enumName.replace(QRegularExpression(QLatin1String("([a-z])([A-Z])")), QLatin1String("\\1 \\2"));
        }
        case ValueRole:
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

int AttendeeStatusModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return QMetaEnum::fromType<KCalendarCore::Attendee::PartStat>().keyCount();
    }
    return 0;
}








AttendeesModel::AttendeesModel(QObject* parent, KCalendarCore::Event::Ptr eventPtr)
    : QAbstractListModel(parent)
    , m_event(eventPtr)
    , m_attendeeStatusModel(parent)
{
    for(int i = 0; i < QMetaEnum::fromType<AttendeesModel::Roles>().keyCount(); i++) {
        int value = QMetaEnum::fromType<AttendeesModel::Roles>().value(i);
        QLatin1String key(QMetaEnum::fromType<AttendeesModel::Roles>().key(i));
        m_dataRoles[key] = value;
    }
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

QVariantMap AttendeesModel::dataroles()
{
    return m_dataRoles;
}

QVariant AttendeesModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }
    auto attendee = m_event->attendees()[idx.row()];
    switch (role) {
        case CuTypeRole:
            return attendee.cuType();
        case DelegateRole:
            return attendee.delegate();
        case DelegatorRole:
            return attendee.delegator();
        case EmailRole:
            return attendee.email();
        case FullNameRole:
            return attendee.fullName();
        case IsNullRole:
            return attendee.isNull();
        case NameRole:
            return attendee.name();
        case RoleRole:
            return attendee.role();
        case RSVPRole:
            return attendee.RSVP();
        case StatusRole:
            return attendee.status();
        case UidRole:
            return attendee.uid();
        default:
            qWarning() << "Unknown role for event:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
    }
}

bool AttendeesModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (!idx.isValid()) {
        return false;
    }

    // When modifying attendees, remember you cannot change them directly from m_event->attendees (is a const).
    KCalendarCore::Attendee::List currentAttendees(m_event->attendees());

    switch (role) {
        case CuTypeRole:
        {
            KCalendarCore::Attendee::CuType cuType = static_cast<KCalendarCore::Attendee::CuType>(value.toInt());
            currentAttendees[idx.row()].setCuType(cuType);
            break;
        }
        case DelegateRole:
        {
            QString delegate = value.toString();
            currentAttendees[idx.row()].setDelegate(delegate);
            break;
        }
        case DelegatorRole:
        {
            QString delegator = value.toString();
            currentAttendees[idx.row()].setDelegator(delegator);
            break;
        }
        case EmailRole:
        {
            QString email = value.toString();
            currentAttendees[idx.row()].setEmail(email);
            break;
        }
        case FullNameRole:
        {
            // Not a writable property
            return false;
        }
        case IsNullRole:
        {
            // Not an editable value
            return false;
        }
        case NameRole:
        {
            QString name = value.toString();
            currentAttendees[idx.row()].setName(name);
            break;
        }
        case RoleRole:
        {
            KCalendarCore::Attendee::Role role = static_cast<KCalendarCore::Attendee::Role>(value.toInt());
            currentAttendees[idx.row()].setRole(role);
            break;
        }
        case RSVPRole:
        {
            bool rsvp = value.toBool();
            currentAttendees[idx.row()].setRSVP(rsvp);
            break;
        }
        case StatusRole:
        {
            KCalendarCore::Attendee::PartStat status = static_cast<KCalendarCore::Attendee::PartStat>(value.toInt());
            currentAttendees[idx.row()].setStatus(status);
            break;
        }
        case UidRole:
        {
            QString uid = value.toString();
            currentAttendees[idx.row()].setUid(uid);
            break;
        }
        default:
            qWarning() << "Unknown role for event:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return false;
    }
    m_event->setAttendees(currentAttendees);
    emit dataChanged(idx, idx);
    return true;
}

QHash<int, QByteArray> AttendeesModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	for (int i = 0; i < QMetaEnum::fromType<Roles>().keyCount(); i++) {
		roles.insert(Qt::UserRole + i + 1, QMetaEnum::fromType<Roles>().key(i));
	}
	return roles;
}

int AttendeesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_event->attendeeCount();
    }
    return 0;
}

void AttendeesModel::addAttendee()
{
    // QLatin1String is a workaround for QT_NO_CAST_FROM_ASCII
    KCalendarCore::Attendee attendee(QLatin1String(""), QLatin1String(""));
    // addAttendee won't actually add any attendees without a set name
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

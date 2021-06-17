// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QAbstractItemModel>
#include <QCalendar>
#include <KCalendarCore/Calendar>
#include <QDebugStateSaver>
#include <QDebug>

/**
 *
 */
class AttendeeStatusModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        EnumName = Qt::UserRole + 1,
        DisplayName,
        Value
    };
    Q_ENUM(Roles);

    AttendeeStatusModel(QObject *parent = nullptr);
    ~AttendeeStatusModel() = default;

    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;

private:
    QHash<int, QString> m_status;
};




class AttendeesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(KCalendarCore::Event::Ptr eventPtr READ eventPtr WRITE setEventPtr NOTIFY eventPtrChanged)
    Q_PROPERTY(KCalendarCore::Attendee::List attendees READ attendees NOTIFY attendeesChanged)
    Q_PROPERTY(AttendeeStatusModel * attendeeStatusModel READ attendeeStatusModel NOTIFY attendeeStatusModelChanged)

public:
    enum Roles {
        CuType = Qt::UserRole + 1,
        Delegate,
        Delegator,
        Email,
        FullName,
        IsNull,
        Name,
        Role,
        RSVP,
        Status,
        Uid
    };
    Q_ENUM(Roles);

    explicit AttendeesModel(QObject *parent = nullptr, KCalendarCore::Event::Ptr eventPtr = nullptr);
    ~AttendeesModel() = default;

    KCalendarCore::Event::Ptr eventPtr();
    void setEventPtr(KCalendarCore::Event::Ptr event);
    KCalendarCore::Attendee::List attendees();
    AttendeeStatusModel * attendeeStatusModel();

    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;

    Q_INVOKABLE void addAttendee();
    Q_INVOKABLE void deleteAttendee(int row);

    Q_INVOKABLE void setAttendeeCuType(int row, KCalendarCore::Attendee::CuType cutype);
    Q_INVOKABLE void setAttendeeDelegate(int row, QString delegate);
    Q_INVOKABLE void setAttendeeDelegator(int row, QString delegator);
    Q_INVOKABLE void setAttendeeEmail(int row, QString email);
    Q_INVOKABLE void setAttendeeName(int row, QString name);
    Q_INVOKABLE void setAttendeeRole(int row, KCalendarCore::Attendee::Role role);
    Q_INVOKABLE void setAttendeeRSVP(int row, bool rsvp);
    Q_INVOKABLE void setAttendeeStatus(int row, KCalendarCore::Attendee::PartStat status);
    Q_INVOKABLE void setAttendeeUid(int row, QString uid);

Q_SIGNALS:
    void eventPtrChanged();
    void attendeesChanged();
    void attendeeStatusModelChanged();

private:
    KCalendarCore::Event::Ptr m_event;
    AttendeeStatusModel m_attendeeStatusModel;
};

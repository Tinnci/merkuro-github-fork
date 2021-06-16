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

class AttendeesModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(KCalendarCore::Event::Ptr eventPtr READ eventPtr WRITE setEventPtr NOTIFY eventPtrChanged)
    Q_PROPERTY(KCalendarCore::Attendee::List attendees READ attendees NOTIFY attendeesChanged)

public:
    enum Roles {
        CuType = Qt::UserRole + 1,
        Delegate,
        Delegator,
        Email,
        FullName,
        isNull,
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

    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent) const override;

    Q_INVOKABLE void addAttendee(QString name, QString email);
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

private:
    KCalendarCore::Event::Ptr m_event;
};

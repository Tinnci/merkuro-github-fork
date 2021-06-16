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
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent) const override;

Q_SIGNALS:
    void eventPtrChanged();
    void attendeesChanged();

private:
    KCalendarCore::Event::Ptr m_event;
};

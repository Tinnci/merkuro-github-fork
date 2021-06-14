// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QAbstractItemModel>
#include <QCalendar>
#include <KCalendarCore/Calendar>
#include <QDebugStateSaver>
#include <QDebug>

class RemindersModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(KCalendarCore::Event::Ptr eventPtr READ eventPtr WRITE setEventPtr NOTIFY eventPtrChanged)

public:
    enum Roles {
        Type = Qt::UserRole + 1,
        Time,
        StartOffset,
        EndOffset
    };
    Q_ENUM(Roles);

    explicit RemindersModel(QObject *parent = nullptr);
    ~RemindersModel();

    KCalendarCore::Event::Ptr eventPtr();
    void setEventPtr(KCalendarCore::Event::Ptr event);
    void loadReminders();

    QVariant data(const QModelIndex &idx, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &) const override;
    int rowCount(const QModelIndex &parent) const override;

Q_SIGNALS:
    void eventPtrChanged();

private:
    KCalendarCore::Event::Ptr m_event;
    KCalendarCore::Alarm::List m_alarms;
};

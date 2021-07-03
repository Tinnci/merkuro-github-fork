// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QAbstractItemModel>
#include <QCalendar>
#include <KCalendarCore/Calendar>
#include <QDebug>


class RecurrenceExceptionsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(KCalendarCore::Event::Ptr eventPtr READ eventPtr WRITE setEventPtr NOTIFY eventPtrChanged)
    Q_PROPERTY(KCalendarCore::RecurrenceRule::List exceptions READ exceptions NOTIFY exceptionsChanged)
    Q_PROPERTY(QVariantMap dataroles READ dataroles CONSTANT)

public:
    enum Roles {
        RecurrenceExceptionRuleRole = Qt::UserRole + 1,
        DateRole
    };
    Q_ENUM(Roles);

    explicit RecurrenceExceptionsModel(QObject *parent = nullptr, KCalendarCore::Event::Ptr eventPtr = nullptr);
    ~RecurrenceExceptionsModel() = default;

    KCalendarCore::Event::Ptr eventPtr();
    void setEventPtr(KCalendarCore::Event::Ptr event);
    KCalendarCore::RecurrenceRule::List exceptions();
    QVariantMap dataroles();

    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;

    Q_INVOKABLE void addExceptionDate(QDateTime date);
    Q_INVOKABLE void addExceptionRule(KCalendarCore::RecurrenceRule *rrule);
    Q_INVOKABLE void deleteExceptionRule(KCalendarCore::RecurrenceRule *rrule);

Q_SIGNALS:
    void eventPtrChanged();
    void exceptionsChanged();

private:
    KCalendarCore::Event::Ptr m_event;

    QVariantMap m_dataRoles;
};

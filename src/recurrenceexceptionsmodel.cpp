// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QMetaEnum>
#include "recurrenceexceptionsmodel.h"

RecurrenceExceptionsModel::RecurrenceExceptionsModel(QObject* parent, KCalendarCore::Event::Ptr eventPtr)
    : QAbstractListModel(parent)
    , m_event(eventPtr)
{
    for(int i = 0; i < QMetaEnum::fromType<RecurrenceExceptionsModel::Roles>().keyCount(); i++) {
        int value = QMetaEnum::fromType<RecurrenceExceptionsModel::Roles>().value(i);
        QString key = QLatin1String(roleNames()[value]);
        m_dataRoles[key] = value;
    }
}

KCalendarCore::Event::Ptr RecurrenceExceptionsModel::eventPtr()
{
    return m_event;
}

void RecurrenceExceptionsModel::setEventPtr(KCalendarCore::Event::Ptr event)
{
    if (m_event == event) {
        return;
    }
    m_event = event;
    Q_EMIT eventPtrChanged();
}

KCalendarCore::RecurrenceRule::List RecurrenceExceptionsModel::exceptions()
{
    return m_event->recurrence()->exRules();
}

QVariantMap RecurrenceExceptionsModel::dataroles()
{
    return m_dataRoles;
}

QVariant RecurrenceExceptionsModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }
    KCalendarCore::RecurrenceRule *exception = m_event->recurrence()->exRules()[idx.row()];
    switch (role) {
        case RecurrenceExceptionRuleRole:
            return QVariant::fromValue(exception);
        case DateRole:
            return exception->startDt();
        default:
            qWarning() << "Unknown role for event:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
    }
}

QHash<int, QByteArray> RecurrenceExceptionsModel::roleNames() const
{
	return {
        { RecurrenceExceptionRuleRole, QByteArrayLiteral("recurrenceExceptionRule") },
        { DateRole, QByteArrayLiteral("date") },
    };
}

int RecurrenceExceptionsModel::rowCount(const QModelIndex &) const
{
    return m_event->recurrence()->exRules().size();
}

void RecurrenceExceptionsModel::addExceptionDate(QDateTime date)
{
    m_event->recurrence()->addExDate(date.date());
}

void RecurrenceExceptionsModel::addExceptionRule(KCalendarCore::RecurrenceRule *rrule)
{
    m_event->recurrence()->addExRule(rrule);
}

void RecurrenceExceptionsModel::deleteExceptionRule(KCalendarCore::RecurrenceRule *rrule)
{
    m_event->recurrence()->deleteExRule(rrule);
}

Q_DECLARE_METATYPE(KCalendarCore::RecurrenceRule*)

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
    Q_EMIT exceptionsChanged();
    Q_EMIT layoutChanged();
}

QList<QDateTime> RecurrenceExceptionsModel::exceptions()
{
    return m_event->recurrence()->exDateTimes();
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
    QDateTime exception = m_event->recurrence()->exDateTimes()[idx.row()];
    qDebug() << exception;
    switch (role) {
        case DateRole:
            return exception;
        default:
            qWarning() << "Unknown role for event:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
    }
}

QHash<int, QByteArray> RecurrenceExceptionsModel::roleNames() const
{
	return {
        { DateRole, QByteArrayLiteral("date") }
    };
}

int RecurrenceExceptionsModel::rowCount(const QModelIndex &) const
{
    qDebug() << m_event->recurrence()->exDateTimes().size();
    return m_event->recurrence()->exDateTimes().size();
}

void RecurrenceExceptionsModel::addExceptionDateTime(QDateTime date)
{
    m_event->recurrence()->addExDateTime(date);
    Q_EMIT exceptionsChanged();
    Q_EMIT layoutChanged();
}

void RecurrenceExceptionsModel::deleteExceptionDateTime(QDateTime date)
{
    auto dateTimes = m_event->recurrence()->exDateTimes();
    dateTimes.removeAt(dateTimes.indexOf(date));
    m_event->recurrence()->setExDateTimes(dateTimes);
    Q_EMIT exceptionsChanged();
    Q_EMIT layoutChanged();
}

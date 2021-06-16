// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QMetaEnum>
#include <remindersmodel.h>

RemindersModel::RemindersModel(QObject *parent, KCalendarCore::Event::Ptr eventPtr)
    : QAbstractItemModel(parent)
    , m_event(eventPtr)
{

}

KCalendarCore::Event::Ptr RemindersModel::eventPtr()
{
    return m_event;
}

void RemindersModel::setEventPtr(KCalendarCore::Event::Ptr event)
{
    if (m_event == event) {
        return;
    }
    m_event = event;
    Q_EMIT eventPtrChanged();
}

KCalendarCore::Alarm::List RemindersModel::alarms()
{
    return m_event->alarms();
}

QVariant RemindersModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }
    auto alarm = m_event->alarms()[idx.row()];
    switch (role) {
        case Type:
            return alarm->type();
        case Time:
            return alarm->time();
        case StartOffset:
            return alarm->startOffset().asSeconds();
        case EndOffset:
            return alarm->endOffset().asSeconds();
        default:
            qWarning() << "Unknown role for event:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
    }
}

QModelIndex RemindersModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column);
    }
    return {};
}

QModelIndex RemindersModel::parent(const QModelIndex &) const
{
    return {};
}

int RemindersModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_event->alarms().size();
    }
    return 0;
}

int RemindersModel::columnCount(const QModelIndex &) const
{
    // Our data is a list, so it's one dimensional and only has 1 column.
    return 1;
}

void RemindersModel::addAlarm()
{
    KCalendarCore::Alarm::Ptr alarm (new KCalendarCore::Alarm(nullptr));
    m_event->addAlarm(alarm);
    Q_EMIT alarmsChanged();
    Q_EMIT layoutChanged();
}

void RemindersModel::deleteAlarm(int row)
{
    if (!hasIndex(row, 0)) {
        return;
    }

    m_event->removeAlarm(m_event->alarms()[row]);
    Q_EMIT alarmsChanged();
    Q_EMIT layoutChanged();
}

void RemindersModel::setAlarmStartOffset(int row, int seconds)
{
    // offset can be set in seconds or days, if we want it to be before the event,
    // it has to be set to a negative value.
    KCalendarCore::Duration offset(seconds);
    m_event->alarms()[row]->setStartOffset(offset);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void RemindersModel::setAlarmEndOffset(int row, int seconds)
{
    KCalendarCore::Duration offset(seconds);
    m_event->alarms()[row]->setEndOffset(offset);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void RemindersModel::setAlarmType(int row, KCalendarCore::Alarm::Type type)
{
    m_event->alarms()[row]->setType(type);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

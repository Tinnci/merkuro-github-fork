// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QMetaEnum>
#include <remindersmodel.h>

RemindersModel::RemindersModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_event(nullptr)
{
    QObject::connect(this, &RemindersModel::eventPtrChanged, this, &RemindersModel::loadReminders);
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

void RemindersModel::loadReminders()
{
    if (!m_event) {
        return;
    }
    beginResetModel();
    m_alarms = m_event->alarms();
    endResetModel();
}

QVariant RemindersModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }
    auto alarm = m_alarms[idx.row()];
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
        return m_alarms.size();
    }
    return 0;
}

int RemindersModel::columnCount(const QModelIndex &) const
{
    // Our data, m_alarms, is a list -- so it's one dimensional, and only has 1 column.
    return 1;
}

void RemindersModel::addAlarm()
{
    QModelIndex indexToUse = index(rowCount(), 0);
    // This should maybe be reimplemented with insertRows()?
    beginInsertRows(indexToUse, rowCount(), rowCount());

    KCalendarCore::Alarm::Ptr alarm (new KCalendarCore::Alarm(nullptr));
    m_alarms.append(alarm);
    Q_EMIT alarmsChanged();

    endInsertRows();
}

void RemindersModel::deleteAlarm(int row)
{
    if (!hasIndex(row, 0)) {
        return;
    }

    QModelIndex indexToUse = index(row, 0);
    beginRemoveRows(indexToUse, row, row);

    m_alarms.removeAt(row);
    qDebug() << rowCount();
    Q_EMIT alarmsChanged();

    endRemoveRows();
    Q_EMIT layoutChanged();
}

void RemindersModel::setAlarmStartOffset(int row, int seconds)
{
    // offset can be set in seconds or days, if we want it to be before the event,
    // it has to be set to a negative value.
    KCalendarCore::Duration offset(seconds);
    m_alarms[row]->setStartOffset(offset);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void RemindersModel::setAlarmEndOffset(int row, int seconds)
{
    KCalendarCore::Duration offset(seconds);
    m_alarms[row]->setEndOffset(offset);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

void RemindersModel::setAlarmType(int row, KCalendarCore::Alarm::Type type)
{
    m_alarms[row]->setType(type);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
}

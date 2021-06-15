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
    auto alarm = m_alarms.at(idx.row());
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
    return 1;
}

void RemindersModel::addAlarm()
{
    beginInsertRows(index(rowCount(), 0), rowCount(), rowCount());
    endInsertRows();
    qDebug() << rowCount();
}

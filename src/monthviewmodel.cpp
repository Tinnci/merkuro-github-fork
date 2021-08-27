// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QMetaEnum>
#include <monthviewmodel.h>

MonthViewModel::MonthViewModel(QObject* parent)
    : QAbstractListModel(parent)
{
    int startingRows = 1000;

    beginInsertRows(QModelIndex(), 0, startingRows);

    QDate today = QDate::currentDate();
    QDate firstDate(today.year(), today.month(), 1);
    firstDate = firstDate.addMonths(-startingRows / 2);
    QDate startDate = firstDate;

    if(startDate.dayOfWeek() == m_locale.firstDayOfWeek()) {
        startDate = startDate.addDays(-1);
    }

    for(int i = 0; i < startingRows; i++) {
        m_firstDayOfMonthDates.append(firstDate);
        while(startDate.dayOfWeek() != m_locale.firstDayOfWeek()) {
            startDate = startDate.addDays(-1);
        }
        m_startDates.append(startDate);
        firstDate = firstDate.addMonths(1);
        startDate = firstDate;
    }

    endInsertRows();
}

QVariant MonthViewModel::data(const QModelIndex& idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }

    QDate startDate = m_startDates[idx.row()];
    QDate firstDate = m_firstDayOfMonthDates[idx.row()];

    switch(role) {
        case StartDateRole:
            return startDate.startOfDay();
        case FirstDateOfMonthRole:
            return firstDate.startOfDay();
        case SelectedMonthRole:
            return firstDate.month();
        default:
            qWarning() << "Unknown role for startdate:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
    }
}

int MonthViewModel::rowCount(const QModelIndex& parent) const
{
    return m_startDates.length();
}

QHash<int, QByteArray> MonthViewModel::roleNames() const
{
    return {
        { StartDateRole, QByteArrayLiteral("startDate") },
        { FirstDateOfMonthRole, QByteArrayLiteral("firstDate") },
        { SelectedMonthRole, QByteArrayLiteral("selectedMonth") }
    };
}

/*bool MonthViewModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;
    return true;
}*/

void MonthViewModel::addDate(bool atEnd)
{
    int newRow = atEnd ? rowCount() : 0;

    beginInsertRows(QModelIndex(), newRow, newRow);

    QDate firstDate = atEnd ? m_firstDayOfMonthDates[rowCount()-1].addMonths(1) : m_firstDayOfMonthDates[0].addMonths(-1);
    QDate startDate = firstDate;

    if(atEnd) {
        m_firstDayOfMonthDates.append(firstDate);
    } else {
        m_firstDayOfMonthDates.insert(0, firstDate);
    }

    while(startDate.dayOfWeek() != m_locale.firstDayOfWeek()) {
        startDate = startDate.addDays(-1);
    }

    if(atEnd) {
        m_startDates.append(startDate);
    } else {
        m_startDates.insert(0, startDate);
    }

    endInsertRows();
}


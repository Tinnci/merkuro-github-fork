// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QAbstractItemModel>
#include <QCalendar>
#include <QDateTime>
#include <QLocale>
#include <QDebug>


class MonthViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        StartDateRole = Qt::UserRole + 1,
        FirstDayOfMonthRole,
        SelectedMonthRole
    };
    Q_ENUM(Roles);

    explicit MonthViewModel(QObject *parent = nullptr);
    ~MonthViewModel() = default;

    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    //bool canFetchMore(const QModelIndex &parent) const override;
    Q_INVOKABLE void addDate(bool atEnd);

private:
    QVector<QDate> m_startDates;
    QVector<QDate> m_firstDayOfMonthDates;
    QLocale m_locale;
};

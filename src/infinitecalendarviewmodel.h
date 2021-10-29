// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "hourlyincidencemodel.h"
#include "multidayincidencemodel.h"
#include <Akonadi/Calendar/ETMCalendar>
#include <QAbstractItemModel>
#include <QCalendar>
#include <QDateTime>
#include <QLocale>

class InfiniteCalendarViewModel : public QAbstractListModel
{
    Q_OBJECT
    // Amount of dates to add each time the model adds more dates
    Q_PROPERTY(int datesToAdd READ datesToAdd WRITE setDatesToAdd NOTIFY datesToAddChanged)
    Q_PROPERTY(int scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(Akonadi::ETMCalendar *calendar READ calendar WRITE setCalendar NOTIFY calendarChanged)

public:
    // The decade scale is designed to be used in a 4x3 grid, so shows 12 years at a time
    enum Scale { WeekScale, MonthScale, YearScale, DecadeScale };
    Q_ENUM(Scale);

    enum Roles {
        StartDateRole = Qt::UserRole + 1,
        FirstDayOfMonthRole,
        SelectedMonthRole,
        SelectedYearRole,
        MonthViewModelRole,
        ScheduleViewModelRole,
        WeekViewModelRole
    };
    Q_ENUM(Roles);

    explicit InfiniteCalendarViewModel(QObject *parent = nullptr);
    ~InfiniteCalendarViewModel() override = default;

    void setup();
    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;

    Q_INVOKABLE void addDates(bool atEnd, const QDate startFrom = QDate());
    void addWeekDates(bool atEnd, const QDate &startFrom);
    void addMonthDates(bool atEnd, const QDate &startFrom);
    void addYearDates(bool atEnd, const QDate &startFrom);
    void addDecadeDates(bool atEnd, const QDate &startFrom);

    int datesToAdd() const;
    void setDatesToAdd(int datesToAdd);

    int scale();
    void setScale(int scale);

    Akonadi::ETMCalendar *calendar();
    void setCalendar(Akonadi::ETMCalendar *calendar);

    void handleCalendarDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void handleCalendarRowsInserted(const QModelIndex &parent, int first, int last);
    void handleCalendarRowsRemoved(const QModelIndex &parent, int first, int last);

Q_SIGNALS:
    void datesToAddChanged();
    void scaleChanged();
    void calendarChanged();

private:
    QVector<QDate> m_startDates;
    QVector<QDate> m_firstDayOfMonthDates;
    QLocale m_locale;
    int m_datesToAdd = 10;
    int m_scale = MonthScale;
    mutable QHash<QDate, MultiDayIncidenceModel *> m_monthViewModels;
    mutable QHash<QDate, MultiDayIncidenceModel *> m_scheduleViewModels;
    mutable QHash<QDate, HourlyIncidenceModel *> m_weekViewModels;
    Akonadi::ETMCalendar *m_calendar;
};

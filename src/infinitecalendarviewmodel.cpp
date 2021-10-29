// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "incidenceoccurrencemodel.h"
#include <QDebug>
#include <QMetaEnum>
#include <akonadi_version.h>
#include <cmath>
#include <infinitecalendarviewmodel.h>
#if AKONADI_VERSION >= QT_VERSION_CHECK(5, 18, 41)
#include <Akonadi/EntityTreeModel>
#else
#include <AkonadiCore/EntityTreeModel>
#endif

InfiniteCalendarViewModel::InfiniteCalendarViewModel(QObject *parent)
    : QAbstractListModel(parent)
{
    setup();
}

void InfiniteCalendarViewModel::setup()
{
    const QDate today = QDate::currentDate();

    switch (m_scale) {
    case WeekScale: {
        QDate firstDay = today.addDays(-today.dayOfWeek() + m_locale.firstDayOfWeek());
        // We create dates before and after where our view will start from (which is today)
        firstDay = firstDay.addDays((-m_datesToAdd * 7) / 2);

        addWeekDates(true, firstDay);
        break;
    }
    case MonthScale: {
        QDate firstDay(today.year(), today.month(), 1);
        firstDay = firstDay.addMonths(-m_datesToAdd / 2);

        addMonthDates(true, firstDay);
        break;
    }
    case YearScale: {
        QDate firstDay(today.year(), today.month(), 1);
        firstDay = firstDay.addYears(-m_datesToAdd / 2);

        addYearDates(true, firstDay);
        break;
    }
    case DecadeScale: {
        const int firstYear = ((floor(today.year() / 10)) * 10) - 1; // E.g. For 2020 have view start at 2019...
        QDate firstDay(firstYear, today.month(), 1);
        firstDay = firstDay.addYears(((-m_datesToAdd * 12) / 2) + 10); // 3 * 4 grid so 12 years, end at 2030, and align for mid index to be current decade

        addDecadeDates(true, firstDay);
        break;
    }
    }
}

QVariant InfiniteCalendarViewModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }

    const QDate startDate = m_startDates[idx.row()];

    auto generateMultiDayIncidenceModel = [=](QDate start, int length, int periodLength) {
        auto model = new MultiDayIncidenceModel;
        model->setPeriodLength(periodLength);
        model->setModel(new IncidenceOccurrenceModel);
        model->model()->setHandleOwnRefresh(false);
        model->model()->setStart(start);
        model->model()->setLength(length);
        model->model()->setCalendar(m_calendar);
        return model;
    };

    if (m_scale == MonthScale && role != StartDateRole) {
        const QDate firstDay = m_firstDayOfMonthDates[idx.row()];

        switch (role) {
        case FirstDayOfMonthRole:
            return firstDay.startOfDay();
        case SelectedMonthRole:
            return firstDay.month();
        case SelectedYearRole:
            return firstDay.year();
        case MonthViewModelRole: {
            if (!m_monthViewModels.contains(startDate)) {
                m_monthViewModels[startDate] = generateMultiDayIncidenceModel(startDate, 42, 7);
            }

            return QVariant::fromValue(m_monthViewModels[startDate]);
        }
        case ScheduleViewModelRole: {
            if (!m_scheduleViewModels.contains(firstDay)) {
                m_scheduleViewModels[firstDay] = generateMultiDayIncidenceModel(firstDay, firstDay.daysInMonth(), 1);
            }

            return QVariant::fromValue(m_scheduleViewModels[firstDay]);
        }
        default:
            qWarning() << "Unknown role for startdate:" << QMetaEnum::fromType<Roles>().valueToKey(role);
            return {};
        }
    }

    switch (role) {
    case StartDateRole:
        return startDate.startOfDay();
    case SelectedMonthRole:
        return startDate.month();
    case SelectedYearRole:
        return startDate.year();
    case WeekViewModelRole: {
        if (!m_weekViewModels.contains(startDate)) {
            m_weekViewModels[startDate] = new HourlyIncidenceModel;
            m_weekViewModels[startDate]->setPeriodLength(7);
            m_weekViewModels[startDate]->setModel(new IncidenceOccurrenceModel);
            m_weekViewModels[startDate]->model()->setHandleOwnRefresh(false);
            m_weekViewModels[startDate]->model()->setStart(startDate);
            m_weekViewModels[startDate]->model()->setLength(7);
            m_weekViewModels[startDate]->model()->setCalendar(m_calendar);
            m_weekViewModels[startDate]->setFilters(HourlyIncidenceModel::NoAllDay | HourlyIncidenceModel::NoMultiDay);
        }

        return QVariant::fromValue(m_weekViewModels[startDate]);
    }
    default:
        qWarning() << "Unknown role for startdate:" << QMetaEnum::fromType<Roles>().valueToKey(role);
        return {};
    }
}

int InfiniteCalendarViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_startDates.length();
}

QHash<int, QByteArray> InfiniteCalendarViewModel::roleNames() const
{
    return {
        {StartDateRole, QByteArrayLiteral("startDate")},
        {FirstDayOfMonthRole, QByteArrayLiteral("firstDay")},
        {SelectedMonthRole, QByteArrayLiteral("selectedMonth")},
        {SelectedYearRole, QByteArrayLiteral("selectedYear")},
        {MonthViewModelRole, QByteArrayLiteral("monthViewModel")},
        {ScheduleViewModelRole, QByteArrayLiteral("scheduleViewModel")},
        {WeekViewModelRole, QByteArrayLiteral("weekViewModel")},
    };
}

void InfiniteCalendarViewModel::addDates(bool atEnd, const QDate startFrom)
{
    switch (m_scale) {
    case WeekScale:
        addWeekDates(atEnd, startFrom);
        break;
    case MonthScale:
        addMonthDates(atEnd, startFrom);
        break;
    case YearScale:
        addYearDates(atEnd, startFrom);
        break;
    case DecadeScale:
        addDecadeDates(atEnd, startFrom);
        break;
    }
}

void InfiniteCalendarViewModel::addWeekDates(bool atEnd, const QDate &startFrom)
{
    const int newRow = atEnd ? rowCount() : 0;

    beginInsertRows(QModelIndex(), newRow, newRow + m_datesToAdd - 1);

    for (int i = 0; i < m_datesToAdd; i++) {
        QDate startDate = startFrom.isValid() && i == 0 ? startFrom : atEnd ? m_startDates[rowCount() - 1].addDays(7) : m_startDates[0].addDays(-7);

        if (startDate.dayOfWeek() != m_locale.firstDayOfWeek()) {
            startDate = startDate.addDays(-startDate.dayOfWeek() + m_locale.firstDayOfWeek());
        }

        if (atEnd) {
            m_startDates.append(startDate);
        } else {
            m_startDates.insert(0, startDate);
        }
    }

    endInsertRows();
}

void InfiniteCalendarViewModel::addMonthDates(bool atEnd, const QDate &startFrom)
{
    const int newRow = atEnd ? rowCount() : 0;

    beginInsertRows(QModelIndex(), newRow, newRow + m_datesToAdd - 1);

    for (int i = 0; i < m_datesToAdd; i++) {
        const QDate firstDay = startFrom.isValid() && i == 0 ? startFrom
            : atEnd                                          ? m_firstDayOfMonthDates[rowCount() - 1].addMonths(1)
                                                             : m_firstDayOfMonthDates[0].addMonths(-1);
        QDate startDate = firstDay;

        startDate = startDate.addDays(-startDate.dayOfWeek() + m_locale.firstDayOfWeek());
        if (startDate >= firstDay) {
            startDate = startDate.addDays(-7);
        }

        if (atEnd) {
            m_firstDayOfMonthDates.append(firstDay);
            m_startDates.append(startDate);
        } else {
            m_firstDayOfMonthDates.insert(0, firstDay);
            m_startDates.insert(0, startDate);
        }
    }

    endInsertRows();
}

void InfiniteCalendarViewModel::addYearDates(bool atEnd, const QDate &startFrom)
{
    const int newRow = atEnd ? rowCount() : 0;

    beginInsertRows(QModelIndex(), newRow, newRow + m_datesToAdd - 1);

    for (int i = 0; i < m_datesToAdd; i++) {
        QDate startDate = startFrom.isValid() && i == 0 ? startFrom : atEnd ? m_startDates[rowCount() - 1].addYears(1) : m_startDates[0].addYears(-1);

        if (atEnd) {
            m_startDates.append(startDate);
        } else {
            m_startDates.insert(0, startDate);
        }
    }

    endInsertRows();
}

void InfiniteCalendarViewModel::addDecadeDates(bool atEnd, const QDate &startFrom)
{
    const int newRow = atEnd ? rowCount() : 0;

    beginInsertRows(QModelIndex(), newRow, newRow + m_datesToAdd - 1);

    for (int i = 0; i < m_datesToAdd; i++) {
        QDate startDate = startFrom.isValid() && i == 0 ? startFrom : atEnd ? m_startDates[rowCount() - 1].addYears(10) : m_startDates[0].addYears(-10);

        if (atEnd) {
            m_startDates.append(startDate);
        } else {
            m_startDates.insert(0, startDate);
        }
    }

    endInsertRows();
}

int InfiniteCalendarViewModel::datesToAdd() const
{
    return m_datesToAdd;
}

void InfiniteCalendarViewModel::setDatesToAdd(int datesToAdd)
{
    m_datesToAdd = datesToAdd;
    Q_EMIT datesToAddChanged();
}

int InfiniteCalendarViewModel::scale()
{
    return m_scale;
}

void InfiniteCalendarViewModel::setScale(int scale)
{
    beginResetModel();

    m_startDates.clear();
    m_firstDayOfMonthDates.clear();
    // m_monthViewModels.clear();
    // m_scheduleViewModels.clear();
    // m_weekViewModels.clear();

    m_scale = scale;
    setup();
    Q_EMIT scaleChanged();

    endResetModel();
}

Akonadi::ETMCalendar *InfiniteCalendarViewModel::calendar()
{
    return m_calendar;
}

void InfiniteCalendarViewModel::setCalendar(Akonadi::ETMCalendar *calendar)
{
    m_calendar = calendar;

    for (auto model : m_monthViewModels) {
        model->model()->setCalendar(calendar);
    }

    for (auto model : m_scheduleViewModels) {
        model->model()->setCalendar(calendar);
    }

    for (auto model : m_weekViewModels) {
        model->model()->setCalendar(calendar);
    }

    connect(m_calendar->model(), &QAbstractItemModel::dataChanged, this, &InfiniteCalendarViewModel::handleCalendarDataChanged);
    connect(m_calendar->model(), &QAbstractItemModel::rowsInserted, this, &InfiniteCalendarViewModel::handleCalendarRowsInserted);
    connect(m_calendar->model(), &QAbstractItemModel::rowsRemoved, this, &InfiniteCalendarViewModel::handleCalendarRowsRemoved);
}

void InfiniteCalendarViewModel::handleCalendarDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
}

void InfiniteCalendarViewModel::handleCalendarRowsInserted(const QModelIndex &parent, int first, int last)
{
    QVector<QDate> affectedMonthModels;
    QVector<QDate> affectedScheduleModels;
    QVector<QDate> affectedWeekModels;

    auto checkMonthModels = [&affectedMonthModels, this](QDate start, QDate end) {
        for (auto startDate : m_monthViewModels.keys()) {
            if (end < startDate || start > startDate.addDays(42)) {
                continue;
            } else {
                affectedMonthModels.append(startDate);
            }
        }
    };

    auto checkScheduleModels = [&affectedScheduleModels, this](QDate start, QDate end) {
        for (auto startDate : m_scheduleViewModels.keys()) {
            if (end < startDate || start > startDate.addDays(start.daysInMonth())) {
                continue;
            } else {
                affectedScheduleModels.append(startDate);
            }
        }
    };

    auto checkWeekModels = [&affectedWeekModels, this](QDate start, QDate end) {
        for (auto startDate : m_weekViewModels.keys()) {
            if (end < startDate || start > startDate.addDays(7)) {
                continue;
            } else {
                affectedWeekModels.append(startDate);
            }
        }
    };

    auto checkModels = [=](QDate start, QDate end) {
        checkMonthModels(start, end);
        checkScheduleModels(start, end);
        checkWeekModels(start, end);
    };

    for (int i = first; i <= last; i++) {
        const auto index = m_calendar->model()->index(i, 0, parent);
        const auto item = index.data(Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();

        if (item.hasPayload<KCalendarCore::Incidence::Ptr>()) {
            const auto incidence = item.payload<KCalendarCore::Incidence::Ptr>();

            if (incidence->type() == KCalendarCore::Incidence::TypeTodo) {
                const auto todo = incidence.staticCast<KCalendarCore::Todo>();
                const QDate dateStart = todo->dtStart().date();
                const QDate dateDue = todo->dtDue().date();

                if (!dateStart.isValid() && !dateDue.isValid()) {
                    continue;
                } else if (!dateStart.isValid()) {
                    checkModels(dateDue, dateDue);
                }

            } else if (!incidence->dtStart().isValid()) {
                continue;
            } else if (incidence->type() == KCalendarCore::Incidence::TypeEvent) {
                const auto event = incidence.staticCast<KCalendarCore::Event>();
                const QDate dateStart = event->dtStart().date();
                const QDate dateEnd = event->dtEnd().date();

                checkModels(dateStart, dateEnd);
            }
        }
    }

    for (auto startDate : affectedMonthModels) {
        m_monthViewModels[startDate]->model()->updateQuery();
    }
    for (auto startDate : affectedScheduleModels) {
        m_scheduleViewModels[startDate]->model()->updateQuery();
    }
    for (auto startDate : affectedWeekModels) {
        m_weekViewModels[startDate]->model()->updateQuery();
    }
}

void InfiniteCalendarViewModel::handleCalendarRowsRemoved(const QModelIndex &parent, int first, int last)
{
}

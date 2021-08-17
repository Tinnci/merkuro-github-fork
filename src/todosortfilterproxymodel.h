// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include <QSortFilterProxyModel>
#include <QAbstractProxyModel>
#include <CalendarSupport/KCalPrefs>
#include <CalendarSupport/Utils>
#include <KCheckableProxyModel>
#include <KDescendantsProxyModel>
#include <KExtraColumnsProxyModel>
#include <KFormat>
#include <todomodel.h>
#include <incidencetreemodel.h>

class ExtraTodoModel;

class TodoSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum BaseTodoModelColumns {
        SummaryColumn = TodoModel::SummaryColumn,
        PriorityColumn = TodoModel::PriorityColumn,
        PercentColumn = TodoModel::PercentColumn,
        StartDateColumn = TodoModel::StartDateColumn,
        DueDateColumn = TodoModel::DueDateColumn,
        CategoriesColumn = TodoModel::CategoriesColumn,
        DescriptionColumn = TodoModel::DescriptionColumn,
        CalendarColumn = TodoModel::CalendarColumn,
    };
    Q_ENUM(BaseTodoModelColumns);
    enum ExtraTodoModelColumns {
        StartTimeColumn = TodoModel::ColumnCount,
        EndTimeColumn,
        PriorityIntColumn
    };
    Q_ENUM(ExtraTodoModelColumns);

    TodoSortFilterProxyModel(QObject *parent = nullptr);
    ~TodoSortFilterProxyModel() = default;

    void setCalendar(Akonadi::ETMCalendar::Ptr calendar);
    void setIncidenceChanger(Akonadi::IncidenceChanger *changer);
    void setColorCache(QHash<QString, QColor> colorCache);

    Q_INVOKABLE QVariantMap getCollectionDetails(int row);
    Q_INVOKABLE void sortTodoModel(int sort, bool ascending);

private:
    ExtraTodoModel *m_extraTodoModel = nullptr;
};

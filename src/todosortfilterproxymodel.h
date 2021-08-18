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
#include <Akonadi/Calendar/IncidenceChanger>
#include <Akonadi/Calendar/ETMCalendar>
#include <todomodel.h>
#include <incidencetreemodel.h>

class ExtraTodoModel;

class TodoSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(Akonadi::IncidenceChanger *incidenceChanger WRITE setIncidenceChanger NOTIFY incidenceChangerChanged)
    Q_PROPERTY(Akonadi::ETMCalendar *calendar WRITE setCalendar NOTIFY calendarChanged)
    Q_PROPERTY(qint64 filterCollectionId READ filterCollectionId WRITE setFilterCollectionId NOTIFY filterCollectionIdChanged)

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

    bool filterAcceptsRow(int row, const QModelIndex &sourceParent) const override;

    void setCalendar(Akonadi::ETMCalendar *calendar);
    void setIncidenceChanger(Akonadi::IncidenceChanger *changer);
    void setColorCache(QHash<QString, QColor> colorCache);
    qint64 filterCollectionId();
    void setFilterCollectionId(qint64 filterCollectionId);

    Q_INVOKABLE QVariantMap getCollectionDetails(int row);
    Q_INVOKABLE void sortTodoModel(int sort, bool ascending);

Q_SIGNALS:
    void incidenceChangerChanged();
    void calendarChanged();
    void filterCollectionIdChanged();

private:
    ExtraTodoModel *m_extraTodoModel = nullptr;
    qint64 m_filterCollectionId;
};

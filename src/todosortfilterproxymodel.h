// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include <QSortFilterProxyModel>
#include <CalendarSupport/KCalPrefs>
#include <CalendarSupport/Utils>
#include <KCheckableProxyModel>
#include <KDescendantsProxyModel>
#include <KFormat>
#include <todomodel.h>
#include <incidencetreemodel.h>

class TodoSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum Roles {
        LocationRole = TodoModel::CalendarRole + 1,
        AllDayRole,
        CompletedRole,
        ColorRole,
        CollectionIdRole,
        DurationStringRole,
        IncidenceIdRole,
        IncidenceTypeRole,
        IncidenceTypeStrRole,
        IncidenceTypeIconRole,
        IncidencePtrRole
    };
    enum TodoModelSort {
        SortDueDate = 0,
        SortName,
        SortPriority,
    };
    Q_ENUM(TodoModelSort);

    TodoSortFilterProxyModel(QObject *parent = nullptr);
    ~TodoSortFilterProxyModel() = default;

    void setCalendar(Akonadi::ETMCalendar::Ptr calendar);
    void setIncidenceChanger(Akonadi::IncidenceChanger *changer);
    void setColorCache(QHash<QString, QColor> colorCache);

    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap getCollectionDetails(int row);
    Q_INVOKABLE QVariantMap getTodoData(int row);
    Q_INVOKABLE void sortTodoModel(int sort, bool ascending);

private:
    IncidenceTreeModel *m_todoTreeModel = nullptr;
    TodoModel *m_baseTodoModel = nullptr;
    KDescendantsProxyModel *m_todoModel = nullptr;
    QHash<QString, QColor> m_colors;
};

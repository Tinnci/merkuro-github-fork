// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <todosortfilterproxymodel.h>

class ExtraTodoModel : public KExtraColumnsProxyModel
{
    Q_OBJECT

public:
    enum Columns {
        StartTimeColumn = 0,
        EndTimeColumn,
        PriorityIntColumn
    };
    Q_ENUM(Columns);
    enum Roles {
        StartTimeRole = TodoModel::CalendarRole + 1,
        EndTimeRole,
        LocationRole,
        AllDayRole,
        CompletedRole,
        ColorRole,
        CollectionIdRole,
        DurationStringRole,
        IncidenceIdRole,
        IncidenceTypeRole,
        IncidenceTypeStrRole,
        IncidenceTypeIconRole,
        IncidencePtrRole,
    };
    Q_ENUM(Roles);

    ExtraTodoModel(QObject *parent = nullptr) : KExtraColumnsProxyModel(parent) {
        const QString todoMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.todo");
        m_todoTreeModel = new IncidenceTreeModel(QStringList() << todoMimeType, this);
        const auto pref = EventViews::PrefsPtr();
        m_baseTodoModel = new TodoModel(pref, this);
        m_baseTodoModel->setSourceModel(m_todoTreeModel);
        m_todoModel = new KDescendantsProxyModel(this);
        m_todoModel->setSourceModel(m_baseTodoModel);
        m_todoModel->setExpandsByDefault(true);
        setSourceModel(m_todoModel);

        appendColumn(QLatin1String("StartDateTime"));
        appendColumn(QLatin1String("EndDateTime"));
        appendColumn(QLatin1String("PriorityInt"));
    };
    ~ExtraTodoModel() = default;

    QVariant extraColumnData(const QModelIndex &parent, int row, int extraColumn, int role = Qt::DisplayRole) const override {
        if (role != Qt::DisplayRole && role != Qt::EditRole) {
            return QVariant();
        }

        const auto todoItem = index(row, extraColumn, parent).data(TodoModel::TodoRole).value<Akonadi::Item>();
        const auto todoPtr = CalendarSupport::todo(todoItem);

        switch (extraColumn) {
        case StartTimeColumn:
            return todoPtr->dtStart();
        case EndTimeColumn:
            return todoPtr->dtDue();
        case PriorityIntColumn:
            return todoPtr->priority();
        default:
            return QVariant();
        }
    };

    QVariant data (const QModelIndex &index, int  role = Qt::DisplayRole) const override {
        if (!index.isValid()) {
            return QVariant();
        }

        auto idx = mapToSource(index);
        auto todoItem = idx.data(TodoModel::TodoRole).value<Akonadi::Item>();
        auto collectionId = todoItem.parentCollection().id();
        auto todoPtr = CalendarSupport::todo(todoItem);

        if(role == Roles::StartTimeRole) {
            return todoPtr->dtStart();
        } else if (role == Roles::EndTimeRole) {
            return todoPtr->dtDue();
        } else if(role == Roles::LocationRole) {
            return todoPtr->location();
        } else if(role == Roles::AllDayRole) {
            return todoPtr->allDay();
        } else if(role == Roles::ColorRole) {
            QColor nullcolor;
            return m_colors.contains(QString::number(collectionId)) ? m_colors[QString::number(collectionId)] : nullcolor;
        } else if(role == Roles::CompletedRole) {
            return todoPtr->isCompleted();
        } else if(role == Roles::CollectionIdRole) {
            return collectionId;
        } else if (role == DurationStringRole) {
            KFormat format;
            if (todoPtr->allDay()) {
                return format.formatSpelloutDuration(24*60*60*1000); // format milliseconds in 1 day
            }
            return format.formatSpelloutDuration(todoPtr->duration().asSeconds() * 1000);
        } else if(role == Roles::IncidenceIdRole) {
            return todoPtr->uid();
        } else if(role == Roles::IncidenceTypeRole) {
            return todoPtr->type();
        } else if(role == Roles::IncidenceTypeStrRole) {
            return todoPtr->typeStr();
        } else if(role == Roles::IncidenceTypeIconRole) {
            return todoPtr->iconName();
        } else if(role == Roles::IncidencePtrRole) {
            return QVariant::fromValue(CalendarSupport::incidence(todoItem));
        }

        return KExtraColumnsProxyModel::data(index, role);
    };

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roleNames = KExtraColumnsProxyModel::roleNames();
        roleNames[TodoModel::SummaryRole] = "text";
        roleNames[Roles::StartTimeRole] = "startTime";
        roleNames[Roles::EndTimeRole] = "endTime";
        roleNames[Roles::LocationRole] = "location";
        roleNames[Roles::AllDayRole] = "allDay";
        roleNames[Roles::ColorRole] = "color";
        roleNames[Roles::CompletedRole] = "todoCompleted";
        roleNames[Roles::CollectionIdRole] = "collectionId";
        roleNames[Roles::DurationStringRole] = "durationString";
        roleNames[Roles::IncidenceIdRole] = "incidenceId";
        roleNames[Roles::IncidenceTypeRole] = "incidenceType";
        roleNames[Roles::IncidenceTypeStrRole] = "incidenceTypeStr";
        roleNames[Roles::IncidenceTypeIconRole] = "incidenceTypeIcon";
        roleNames[Roles::IncidencePtrRole] = "incidencePtr";

        return roleNames;
    }

    void setCalendar(Akonadi::ETMCalendar::Ptr calendar) {
        m_todoTreeModel->setSourceModel(calendar->model());
        m_baseTodoModel->setCalendar(calendar);
    };

    void setIncidenceChanger(Akonadi::IncidenceChanger* changer) {
        m_baseTodoModel->setIncidenceChanger(changer);
    };

    QHash<QString, QColor> colorCache() {
        return m_colors;
    };

    void setColorCache(QHash<QString, QColor> colorCache) {
        m_colors = colorCache;
    };

private:
    IncidenceTreeModel *m_todoTreeModel = nullptr;
    TodoModel *m_baseTodoModel = nullptr;
    KDescendantsProxyModel *m_todoModel = nullptr;
    QHash<QString, QColor> m_colors;
};





TodoSortFilterProxyModel::TodoSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    m_extraTodoModel = new ExtraTodoModel;
    setSourceModel(m_extraTodoModel);
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void TodoSortFilterProxyModel::setCalendar(Akonadi::ETMCalendar::Ptr calendar)
{
    m_extraTodoModel->setCalendar(calendar);
}

void TodoSortFilterProxyModel::setIncidenceChanger(Akonadi::IncidenceChanger* changer)
{
    m_extraTodoModel->setIncidenceChanger(changer);
}

void TodoSortFilterProxyModel::setColorCache(QHash<QString, QColor> colorCache)
{
    m_extraTodoModel->setColorCache(colorCache);
}

QVariantMap TodoSortFilterProxyModel::getCollectionDetails(int row)
{
    auto idx = m_extraTodoModel->index(row, 0);
    auto todoItem = idx.data(TodoModel::TodoRole).value<Akonadi::Item>();
    auto collection = todoItem.parentCollection();
    QVariantMap collectionDetails;

    collectionDetails[QLatin1String("id")] = collection.id();
    collectionDetails[QLatin1String("name")] = collection.name();
    collectionDetails[QLatin1String("displayName")] = collection.displayName();
    collectionDetails[QLatin1String("readOnly")] = collection.rights().testFlag(Akonadi::Collection::ReadOnly);

    return collectionDetails;
}

void TodoSortFilterProxyModel::sortTodoModel(int column, bool ascending)
{
    Q_EMIT layoutAboutToBeChanged();
    auto order = ascending ? Qt::AscendingOrder : Qt::DescendingOrder;
    this->sort(column, order);
    Q_EMIT layoutChanged();
}

Q_DECLARE_METATYPE(KCalendarCore::Incidence::Ptr)
#include "todosortfilterproxymodel.moc"


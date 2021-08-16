// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <todosortfilterproxymodel.h>

TodoSortFilterProxyModel::TodoSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    const QString todoMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.todo");
    m_todoTreeModel = new IncidenceTreeModel(QStringList() << todoMimeType, this);
    const auto pref = EventViews::PrefsPtr();
    m_baseTodoModel = new TodoModel(pref, this);
    m_baseTodoModel->setSourceModel(m_todoTreeModel);
    m_todoModel = new KDescendantsProxyModel(this);
    m_todoModel->setSourceModel(m_baseTodoModel);
    m_todoModel->setExpandsByDefault(true);
    setSourceModel(m_todoModel);
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void TodoSortFilterProxyModel::setCalendar(Akonadi::ETMCalendar::Ptr calendar)
{
    m_todoTreeModel->setSourceModel(calendar->model());
    m_baseTodoModel->setCalendar(calendar);
}

void TodoSortFilterProxyModel::setIncidenceChanger(Akonadi::IncidenceChanger* changer)
{
    m_baseTodoModel->setIncidenceChanger(changer);
}

void TodoSortFilterProxyModel::setColorCache(QHash<QString, QColor> colorCache)
{
    m_colors = colorCache;
}

QVariantMap TodoSortFilterProxyModel::getTodoData(int row)
{
    auto idx = m_todoModel->index(row, 0);
    auto todoItem = idx.data(TodoModel::TodoRole).value<Akonadi::Item>();
    auto collectionId = todoItem.parentCollection().id();
    auto todoPtr = CalendarSupport::todo(todoItem);
    auto todoIncidencePtr = CalendarSupport::incidence(todoItem);

    QString durationString;
    KFormat format;
    if (todoPtr->allDay()) {
        durationString = format.formatSpelloutDuration(24*60*60*1000); // format milliseconds in 1 day
    } else {
        durationString = format.formatSpelloutDuration(todoPtr->duration().asSeconds() * 1000);
    }

    QColor color;
    if (m_colors.contains(QString::number(collectionId))) {
        color = m_colors[QString::number(collectionId)];
    }

    return QVariantMap{
        {QStringLiteral("text"), todoPtr->summary()},
        {QStringLiteral("description"), todoPtr->description()},
        {QStringLiteral("location"), todoPtr->location()},
        {QStringLiteral("startTime"), todoPtr->dtStart()},
        {QStringLiteral("endTime"), todoPtr->dtDue()},
        {QStringLiteral("allDay"), todoPtr->allDay()},
        {QStringLiteral("todoCompleted"), todoPtr->isCompleted()},
        //{QStringLiteral("starts"), start},
        //{QStringLiteral("duration"), duration},
        {QStringLiteral("durationString"), durationString},
        {QStringLiteral("color"), color},
        {QStringLiteral("collectionId"), collectionId},
        {QStringLiteral("incidenceId"), todoPtr->uid()},
        {QStringLiteral("incidenceType"), todoPtr->type()},
        {QStringLiteral("incidenceTypeStr"), todoPtr->typeStr()},
        {QStringLiteral("incidenceTypeIcon"), todoPtr->iconName()},
        {QStringLiteral("incidencePtr"), QVariant::fromValue(todoIncidencePtr)},
        //{QStringLiteral("incidenceOccurrence"), idx.data(IncidenceOccurrenceModel::IncidenceOccurrence)},
    };
}

QVariantMap TodoSortFilterProxyModel::getCollectionDetails(int row)
{
    auto idx = m_todoModel->index(row, 0);
    auto todoItem = idx.data(TodoModel::TodoRole).value<Akonadi::Item>();
    auto collection = todoItem.parentCollection();
    QVariantMap collectionDetails;

    collectionDetails[QLatin1String("id")] = collection.id();
    collectionDetails[QLatin1String("name")] = collection.name();
    collectionDetails[QLatin1String("displayName")] = collection.displayName();
    collectionDetails[QLatin1String("readOnly")] = collection.rights().testFlag(Akonadi::Collection::ReadOnly);

    return collectionDetails;
}

QVariant TodoSortFilterProxyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto idx = mapToSource(index);
    auto todoItem = idx.data(TodoModel::TodoRole).value<Akonadi::Item>();
    auto collectionId = todoItem.parentCollection().id();
    auto todoPtr = CalendarSupport::todo(todoItem);

    if(role == Roles::LocationRole) {
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

    return QSortFilterProxyModel::data(index, role);
}

QHash<int, QByteArray> TodoSortFilterProxyModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QSortFilterProxyModel::roleNames();
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

void TodoSortFilterProxyModel::sortTodoModel(int sort, bool ascending)
{
    qDebug() << sort;
    auto order = ascending ? Qt::AscendingOrder : Qt::DescendingOrder;
    switch(sort) {
        case TodoModelSort::SortDueDate:
            this->sort(TodoModel::DueDateColumn, order);
            break;
        case TodoModelSort::SortName:
            this->sort(TodoModel::SummaryColumn, order);
            break;
        case TodoModelSort::SortPriority:
            this->sort(TodoModel::PriorityColumn, order);
            break;
    }
    Q_EMIT layoutChanged();
}

Q_DECLARE_METATYPE(KCalendarCore::Incidence::Ptr)

#pragma once

#include "core/data/ICalendarStorage.h"
#include "core/models/CalendarEvent.h"
#include <QAbstractListModel>
#include <QDateTime>
#include <QList>

class EventsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY selectedDateChanged)

public:
    enum EventRoles {
        UidRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        StartDateRole,
        EndDateRole,
        IsAllDayRole,
        LocationRole,
        CalendarIdRole
    };

    explicit EventsModel(QObject *parent = nullptr);

    // Set the storage backend
    void setStorage(PersonalCalendar::Core::ICalendarStoragePtr storage);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Property accessors
    QDate selectedDate() const;
    void setSelectedDate(const QDate &date);

    // Invokable methods for QML
    Q_INVOKABLE void refresh();

Q_SIGNALS:
    void selectedDateChanged();

private:
    void updateEvents();

    PersonalCalendar::Core::ICalendarStoragePtr m_storage;
    QList<PersonalCalendar::Core::CalendarEventPtr> m_events;
    QDate m_selectedDate;
};

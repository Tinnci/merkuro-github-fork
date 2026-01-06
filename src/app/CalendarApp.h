#pragma once

#include <QObject>
#include <QDate>
#include "core/data/ICalendarStorage.h"
#include "models/EventsModel.h"

class CalendarApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(EventsModel* eventsModel READ eventsModel CONSTANT)

public:
    explicit CalendarApp(QObject *parent = nullptr);
    
    // Initialize with a storage path (or default)
    void initialize(const QString &storagePath = QString());

    EventsModel* eventsModel() const;

    // QML Invokables
    Q_INVOKABLE void createEvent(const QString &title, const QDateTime &start, const QDateTime &end, bool allDay);
    Q_INVOKABLE void updateEvent(const QString &uid, const QString &title, const QDateTime &start, const QDateTime &end, bool allDay);
    Q_INVOKABLE void deleteEvent(const QString &uid);
    Q_INVOKABLE void createCalendar(const QString &name);
    Q_INVOKABLE QStringList getCalendarNames();

private:
    PersonalCalendar::Core::ICalendarStoragePtr m_storage;
    EventsModel *m_eventsModel;
};

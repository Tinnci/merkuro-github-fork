#pragma once

#include "core/data/ICalendarStorage.h"
#include "models/EventsModel.h"
#include "models/MonthModel.h"
#include <QDate>
#include <QObject>

class CalendarApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(EventsModel *eventsModel READ eventsModel CONSTANT)
    Q_PROPERTY(MonthModel *monthModel READ monthModel CONSTANT)

public:
    explicit CalendarApp(QObject *parent = nullptr);

    // Initialize with a storage path (or default)
    void initialize(const QString &storagePath = QString());

    EventsModel *eventsModel() const;
    MonthModel *monthModel() const;

    // QML Invokables
    Q_INVOKABLE void createEvent(const QString &title, const QDateTime &start, const QDateTime &end, bool allDay,
                                 const QString &description = QString(), const QString &location = QString(),
                                 const QString &calendarId = QString());
    Q_INVOKABLE void updateEvent(const QString &uid, const QString &title, const QDateTime &start, const QDateTime &end,
                                 bool allDay, const QString &description = QString(),
                                 const QString &location = QString());
    Q_INVOKABLE void deleteEvent(const QString &uid);
    
    // Calendar Management
    Q_INVOKABLE void createCalendar(const QString &name, const QString &color);
    Q_INVOKABLE void deleteCalendar(const QString &id);
    Q_INVOKABLE void setCalendarColor(const QString &id, const QString &color);
    Q_INVOKABLE void setCalendarVisibility(const QString &id, bool visible);
    Q_INVOKABLE QVariantList getCalendars();
    
    Q_INVOKABLE QStringList getCalendarNames(); // Legacy, keep for now

private:
    PersonalCalendar::Core::ICalendarStoragePtr m_storage;
    EventsModel *m_eventsModel;
    MonthModel *m_monthModel;
};

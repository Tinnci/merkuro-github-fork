#pragma once

#include "calendarmanager.h"
#include <Akonadi/ItemMonitor>
#include <QObject>
#include <QPointer>

/*
 * This class is a wrapper specifically for Availability data. The behaviour and objective
 * is similar to IncidenceWrapper class
 *
 */
class AvailabilityWrapper : public QObject, public Akonadi::ItemMonitor
{
    Q_OBJECT

    Q_PROPERTY(KCalendarCore::Availability::Ptr availabilityPtr READ availabilityPtr WRITE setAvailabilityPtr NOTIFY availabilityPtrChanged)

public:
    typedef QSharedPointer<AvailabilityWrapper> Ptr;

    explicit AvailabilityWrapper(CalendarManager *CalendarManager, QObject *parent = nullptr);

    ~AvailabilityWrapper() override;

    KCalendarCore::Availability::Ptr availabilityPtr() const;

Q_SIGNALS:

    void availabilityPtrChanged();

protected:
    void itemChanged(const Akonadi::Item &item) override;

private:
    void setAvailabilityPtr(const KCalendarCore::Availability::Ptr &availabilityPtr);

    QPointer<CalendarManager> m_calendarManager = nullptr;
    KCalendarCore::Availability::Ptr mAvailabilityPtr;
};

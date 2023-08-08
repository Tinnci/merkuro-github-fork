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

    // Akonadi properties
    Q_PROPERTY(qint64 collectionId READ collectionId WRITE setCollectionId NOTIFY collectionIdChanged)

    Q_PROPERTY(KCalendarCore::Availability::Ptr availabilityPtr READ availabilityPtr WRITE setAvailabilityPtr NOTIFY availabilityPtrChanged)

public:
    typedef QSharedPointer<AvailabilityWrapper> Ptr;

    explicit AvailabilityWrapper(CalendarManager *CalendarManager, QObject *parent = nullptr);

    ~AvailabilityWrapper() override;

    KCalendarCore::Availability::Ptr availabilityPtr() const;

    qint64 collectionId() const;
    void setCollectionId(qint64 collectionId);

Q_SIGNALS:

    void collectionIdChanged();
    void availabilityPtrChanged(KCalendarCore::Availability::Ptr availabilityPtr);

protected:
    void itemChanged(const Akonadi::Item &item) override;

private:
    void setAvailabilityPtr(const KCalendarCore::Availability::Ptr &availabilityPtr);

    QPointer<CalendarManager> m_calendarManager = nullptr;
    KCalendarCore::Availability::Ptr mAvailabilityPtr;

    qint64 m_collectionId = -1; // For when we want to edit, this is temporary
};

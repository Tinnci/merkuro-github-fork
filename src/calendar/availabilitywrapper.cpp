#include "availabilitywrapper.h"
#include "calendarmanager.h"
#include <QObject>
#include <qobject.h>

AvailabilityWrapper::AvailabilityWrapper(CalendarManager *calendarmanager, QObject *parent)
    : QObject(parent)
    , Akonadi::ItemMonitor()
    , m_calendarManager(calendarmanager)
{
    // TODO
    qDebug() << "AvailabilityWrapper::AvailabilityWrapper"
             << " BEGIN";

    // TODO hack below
    setAvailabilityPtr(KCalendarCore::Availability::Ptr());
    qDebug() << "AvailabilityWrapper::AvailabilityWrapper"
             << " END";
}

AvailabilityWrapper::~AvailabilityWrapper()
{
}

KCalendarCore::Availability::Ptr AvailabilityWrapper::availabilityPtr() const
{
    return mAvailabilityPtr;
}

qint64 AvailabilityWrapper::collectionId() const
{
    return m_collectionId < 0 ? item().parentCollection().id() : m_collectionId;
}

void AvailabilityWrapper::setCollectionId(qint64 collectionId)
{
    m_collectionId = collectionId;
    Q_EMIT collectionIdChanged();
}

void AvailabilityWrapper::itemChanged(const Akonadi::Item &item)
{
}

void AvailabilityWrapper::setAvailabilityPtr(const KCalendarCore::Availability::Ptr &availabilityPtr)
{
    qDebug() << "AvailabilityWrapper::" << __FUNCTION__ << " BEGIN";
    mAvailabilityPtr = availabilityPtr;

    Q_EMIT availabilityPtrChanged(availabilityPtr);
}

// misc

QString AvailabilityWrapper::summary() const
{
    return mAvailabilityPtr->summary();
}

void AvailabilityWrapper::setSummary(const QString &summary)
{
    mAvailabilityPtr->setSummary(summary);
    Q_EMIT summaryChanged();
}

#ifndef UNITY_CMAKE_SUPPORT
Q_DECLARE_METATYPE(KCalendarCore::Availability::Ptr)
#endif

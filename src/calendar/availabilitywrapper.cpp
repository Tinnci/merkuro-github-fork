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
}

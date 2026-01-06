// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ServiceContainer.h"
#include <QDebug>

namespace PersonalCalendar::Core
{

ServiceContainer *ServiceContainer::s_instance = nullptr;

ServiceContainer::ServiceContainer()
{
    qDebug() << "ServiceContainer: Created";
}

ServiceContainer &ServiceContainer::instance()
{
    if (!s_instance) {
        s_instance = new ServiceContainer();
    }
    return *s_instance;
}

void ServiceContainer::registerCalendarStorage(ICalendarStoragePtr storage)
{
    if (!storage) {
        qWarning() << QLatin1String("ServiceContainer: Attempting to register null calendar storage");
        return;
    }

    m_calendarStorage = storage;
    qDebug() << "ServiceContainer: Calendar storage registered";

    // 如果已有事件操作服务，更新其存储实现
    if (m_eventOperations) {
        // 创建新的 EventOperations 实例
        m_eventOperations = std::make_shared<EventOperations>(storage);
    }
}

void ServiceContainer::registerEventOperations(std::shared_ptr<EventOperations> operations)
{
    if (!operations) {
        qWarning() << QLatin1String("ServiceContainer: Attempting to register null event operations");
        return;
    }

    m_eventOperations = operations;
    qDebug() << "ServiceContainer: Event operations registered";
}

ICalendarStoragePtr ServiceContainer::getCalendarStorage() const
{
    if (!m_calendarStorage) {
        qWarning() << QLatin1String("ServiceContainer: Calendar storage not initialized. ")
                   << "Call registerCalendarStorage() first.";
    }
    return m_calendarStorage;
}

std::shared_ptr<EventOperations> ServiceContainer::getEventOperations() const
{
    if (!m_eventOperations) {
        qWarning() << QLatin1String("ServiceContainer: Event operations not initialized. ")
                   << "Call registerEventOperations() first.";
    }
    return m_eventOperations;
}

bool ServiceContainer::isInitialized() const
{
    return m_calendarStorage && m_eventOperations;
}

void ServiceContainer::reset()
{
    m_calendarStorage.reset();
    m_eventOperations.reset();
    qDebug() << "ServiceContainer: Reset";
}

} // namespace PersonalCalendar::Core

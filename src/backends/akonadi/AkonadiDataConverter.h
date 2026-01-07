// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "core/models/CalendarEvent.h"
#include <QString>
#include <memory>

#include <QSharedPointer>

// Forward declarations
namespace KCalendarCore {
class Incidence;
using IncidencePtr = QSharedPointer<Incidence>;
}

namespace PersonalCalendar::Akonadi {

/**
 * @brief Akonadi 数据转换器
 * 
 * 在 Core::CalendarEvent 和 KCalendarCore::Incidence 之间进行转换。
 * 这是 Akonadi 适配器的核心，处理所有的数据类型映射。
 */
class AkonadiDataConverter {
public:
    /**
     * @brief 将 Core::CalendarEvent 转换为 KCalendarCore::Incidence
     * @param event 核心库事件
     * @return Akonadi incidence 对象
     */
    static KCalendarCore::IncidencePtr toAkonadiIncidence(
        const Core::CalendarEventPtr& event);
    
    /**
     * @brief 将 KCalendarCore::Incidence 转换为 Core::CalendarEvent
     * @param incidence Akonadi incidence 对象
     * @return 核心库事件
     */
    static Core::CalendarEventPtr fromAkonadiIncidence(
        const KCalendarCore::IncidencePtr& incidence);
    
private:
    // 递归规则转换
    static void convertRecurrenceRule(
        const Core::Recurrence& coreRecurrence,
        KCalendarCore::Incidence* akonadiIncidence);
    
    static Core::Recurrence convertRecurrenceRule(
        const KCalendarCore::Incidence* akonadiIncidence);
    
    // 参与者转换
    static void convertAttendees(
        const QList<Core::Attendee>& coreAttendees,
        KCalendarCore::Incidence* akonadiIncidence);
    
    static QList<Core::Attendee> convertAttendees(
        const KCalendarCore::Incidence* akonadiIncidence);
    
    // 提醒转换
    static void convertAlarms(
        const QList<Core::Alarm>& coreAlarms,
        KCalendarCore::Incidence* akonadiIncidence);
    
    static QList<Core::Alarm> convertAlarms(
        const KCalendarCore::Incidence* akonadiIncidence);
};

} // namespace PersonalCalendar::Akonadi

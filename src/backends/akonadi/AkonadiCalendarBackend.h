// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "core/data/ICalendarStorage.h"
#include <QString>
#include <memory>

// Forward declarations - Akonadi classes
namespace Akonadi
{
class ETMCalendar;
class IncidenceChanger;
} // namespace Akonadi

namespace PersonalCalendar::Akonadi
{

/**
 * @brief Akonadi 日历后端实现
 *
 * 将 Akonadi 日历系统适配到 ICalendarStorage 接口。
 * 这允许核心库与 Akonadi 解耦，Akonadi 成为可选的后端实现。
 */
class AkonadiCalendarBackend : public Core::ICalendarStorage
{
public:
    /**
     * @brief 构造函数
     * @param parent 父对象（用于 Qt 对象所有权）
     */
    explicit AkonadiCalendarBackend(QObject *parent = nullptr);

    ~AkonadiCalendarBackend() override;

    // 实现 ICalendarStorage 接口
    bool createEvent(const Core::CalendarEventPtr &event) override;
    Core::CalendarEventPtr getEvent(const QString &uid) override;
    bool updateEvent(const Core::CalendarEventPtr &event) override;
    bool deleteEvent(const QString &uid) override;

    QList<Core::CalendarEventPtr> getEventsByDate(const QDate &date) override;
    QList<Core::CalendarEventPtr> getEventsByDateRange(const QDate &start, const QDate &end) override;
    QList<Core::CalendarEventPtr> getEventsByCollection(const QString &collectionId) override;

    QList<QString> getCalendarIds() override;
    QString getCalendarName(const QString &id) override;
    bool createCalendar(const QString &id, const QString &name) override;
    bool deleteCalendar(const QString &id) override;

    bool sync() override;
    bool isOnline() const override;
    QString getLastSyncTime(const QString &collectionId) override;

private:
    // Akonadi 对象
    std::shared_ptr<::Akonadi::ETMCalendar> m_akonadiCalendar;
    std::shared_ptr<::Akonadi::IncidenceChanger> m_incidenceChanger;

    // 初始化
    bool initialize();

    // 错误处理
    QString m_lastError;
};

} // namespace PersonalCalendar::Akonadi

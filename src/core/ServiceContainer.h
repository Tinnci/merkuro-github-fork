// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "core/data/ICalendarStorage.h"
#include "core/operations/EventOperations.h"
#include <QString>
#include <memory>

namespace PersonalCalendar::Core
{

/**
 * @brief 服务容器 - 依赖注入容器
 *
 * 使用单例模式管理应用的核心服务。
 * 这允许应用在运行时切换不同的后端实现。
 */
class ServiceContainer
{
public:
    /**
     * @brief 获取单例实例
     */
    static ServiceContainer &instance();

    /**
     * @brief 注册日历存储实现
     * @param storage 实现了 ICalendarStorage 的后端
     */
    void registerCalendarStorage(ICalendarStoragePtr storage);

    /**
     * @brief 注册事件操作服务
     * @param operations EventOperations 实现
     */
    void registerEventOperations(std::shared_ptr<EventOperations> operations);

    /**
     * @brief 获取日历存储实现
     * @return 当前注册的 ICalendarStorage 实现
     */
    ICalendarStoragePtr getCalendarStorage() const;

    /**
     * @brief 获取事件操作服务
     * @return 当前注册的 EventOperations 实现
     */
    std::shared_ptr<EventOperations> getEventOperations() const;

    /**
     * @brief 检查是否已初始化
     * @return 已初始化返回 true
     */
    bool isInitialized() const;

    /**
     * @brief 重置所有服务（用于测试）
     */
    void reset();

private:
    ServiceContainer();

    // 单例实例
    static ServiceContainer *s_instance;

    // 注册的服务
    ICalendarStoragePtr m_calendarStorage;
    std::shared_ptr<EventOperations> m_eventOperations;
};

} // namespace PersonalCalendar::Core

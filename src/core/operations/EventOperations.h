// SPDX-FileCopyrightText: 2026 Personal Calendar Project
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "../models/CalendarEvent.h"
#include "../data/ICalendarStorage.h"
#include <functional>
#include <QString>
#include <QDate>

namespace PersonalCalendar::Core {

/**
 * @brief 事件操作的高级业务逻辑层
 * 
 * 这一层为 UI 提供高级别的事件操作，隐藏底层存储实现细节。
 * 使用回调风格来处理异步操作。
 */
class EventOperations {
public:
    // 回调类型定义
    using SuccessCallback = std::function<void(const CalendarEventPtr&)>;
    using ErrorCallback = std::function<void(const QString& error)>;
    using EventListCallback = std::function<void(const QList<CalendarEventPtr>&)>;
    
    /**
     * @brief 构造函数
     * @param storage 实现了 ICalendarStorage 接口的后端
     */
    explicit EventOperations(ICalendarStoragePtr storage);
    virtual ~EventOperations() = default;
    
    // ===== 高级事件操作 =====
    
    /**
     * @brief 创建新事件
     * @param event 新事件
     * @param onSuccess 成功回调
     * @param onError 错误回调
     */
    void createEvent(const CalendarEventPtr& event,
                     SuccessCallback onSuccess,
                     ErrorCallback onError);
    
    /**
     * @brief 更新现有事件
     * @param event 更新的事件
     * @param onSuccess 成功回调
     * @param onError 错误回调
     */
    void updateEvent(const CalendarEventPtr& event,
                     SuccessCallback onSuccess,
                     ErrorCallback onError);
    
    /**
     * @brief 删除事件
     * @param uid 事件 UID
     * @param onSuccess 成功回调
     * @param onError 错误回调
     */
    void deleteEvent(const QString& uid,
                     SuccessCallback onSuccess,
                     ErrorCallback onError);
    
    /**
     * @brief 获取单个事件
     * @param uid 事件 UID
     * @param onSuccess 成功回调
     * @param onError 错误回调
     */
    void getEvent(const QString& uid,
                  SuccessCallback onSuccess,
                  ErrorCallback onError);
    
    /**
     * @brief 获取特定日期的事件
     * @param date 查询日期
     * @param onSuccess 成功回调（列表）
     * @param onError 错误回调
     */
    void getEventsForDate(const QDate& date,
                          EventListCallback onSuccess,
                          ErrorCallback onError);
    
    /**
     * @brief 获取日期范围内的事件
     * @param start 开始日期
     * @param end 结束日期
     * @param onSuccess 成功回调（列表）
     * @param onError 错误回调
     */
    void getEventsForDateRange(const QDate& start,
                               const QDate& end,
                               EventListCallback onSuccess,
                               ErrorCallback onError);

protected:
    ICalendarStoragePtr m_storage;
    
private:
    void handleError(const QString& error, ErrorCallback callback);
};

} // namespace PersonalCalendar::Core

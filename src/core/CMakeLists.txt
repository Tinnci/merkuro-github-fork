# SPDX-FileCopyrightText: 2026 Personal Calendar Project
# SPDX-License-Identifier: LGPL-2.1-or-later

cmake_minimum_required(VERSION 3.16)

# 核心库 - 零 KDE 依赖
add_library(personalcalendar-core STATIC
    models/CalendarEvent.cpp
    models/CalendarEvent.h
    models/TodoItem.cpp
    models/TodoItem.h
    data/ICalendarStorage.h
    operations/EventOperations.cpp
    operations/EventOperations.h
    utils/RecurrenceCalculator.cpp
    utils/RecurrenceCalculator.h
    utils/DateTimeUtils.cpp
    utils/DateTimeUtils.h
    ServiceContainer.cpp
    ServiceContainer.h
)

target_include_directories(personalcalendar-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(personalcalendar-core PUBLIC
    Qt5::Core
)

# 不链接任何 KDE 库

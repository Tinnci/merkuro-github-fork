# SPDX-FileCopyrightText: 2026 Personal Calendar Project
# SPDX-License-Identifier: LGPL-2.1-or-later

# Akonadi 后端库 (可选，需要 KDE 依赖)
# 这个库可以条件性地编译，当 Akonadi 可用时启用

add_library(personalcalendar-akonadi-backend STATIC
    AkonadiCalendarBackend.cpp
    AkonadiCalendarBackend.h
    AkonadiDataConverter.cpp
    AkonadiDataConverter.h
)

target_include_directories(personalcalendar-akonadi-backend PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(personalcalendar-akonadi-backend PUBLIC
    personalcalendar-core
    Qt5::Core
    # 注意: 完整实现需要链接以下库
    # KF5::AkonadiCore
    # KF5::AkonadiCalendar
    # KF5::CalendarCore
)

# 这个后端是可选的，只有在需要 Akonadi 集成时才编译

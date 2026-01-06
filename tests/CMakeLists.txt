# SPDX-FileCopyrightText: 2026 Personal Calendar Project
# SPDX-License-Identifier: LGPL-2.1-or-later

cmake_minimum_required(VERSION 3.16)

# 启用测试
enable_testing()

# 添加 Google Test
find_package(GTest REQUIRED)

# 单元测试
add_executable(core-unit-tests
    unit/CalendarEventTest.cpp
    unit/TodoItemTest.cpp
    unit/RecurrenceAndDateTimeTest.cpp
    unit/ServiceContainerTest.cpp
)

target_link_libraries(core-unit-tests
    personalcalendar-core
    GTest::GTest
    GTest::Main
    Qt5::Core
)

add_test(NAME CoreUnitTests COMMAND core-unit-tests)

# 本地文件后端测试
add_executable(local-backend-tests
    unit/ICSFileBackendTest.cpp
)

target_link_libraries(local-backend-tests
    personalcalendar-core
    personalcalendar-local
    GTest::GTest
    GTest::Main
    Qt5::Core
)

add_test(NAME LocalBackendTests COMMAND local-backend-tests)

# 输出测试结果
add_custom_target(test-verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS core-unit-tests local-backend-tests
)

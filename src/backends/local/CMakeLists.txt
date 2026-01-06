# SPDX-FileCopyrightText: 2026 Personal Calendar Project
# SPDX-License-Identifier: LGPL-2.1-or-later

cmake_minimum_required(VERSION 3.16)

# Local ICS File Backend
add_library(personalcalendar-local
    ICSFileBackend.cpp
    ICSFileBackend.h
)

target_include_directories(personalcalendar-local PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(personalcalendar-local PUBLIC
    personalcalendar-core
    Qt5::Core
)

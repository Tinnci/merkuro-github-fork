# SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

project(kalendar VERSION 0.1)

set(QT_MIN_VERSION "6.6.0")
set(KF_MIN_VERSION "6.0.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to build only the core library without KDE dependencies
option(BUILD_CORE_ONLY "Build only the core library without KDE dependencies" OFF)

include(FeatureSummary)

################# set KDE specific information #################
if(NOT BUILD_CORE_ONLY)
    find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
else()
    set(ECM_FOUND FALSE)
endif()

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
if(ECM_FOUND)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

    include(KDEInstallDirs)
    include(KDECMakeSettings)
    include(KDECompilerSettings NO_POLICY_SCOPE)
    include(ECMSetupVersion)
    include(ECMGenerateHeaders)
    include(ECMPoQmTools)
    include(KDEGitCommitHooks)
    include(KDEClangFormat)
    file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
    kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

    ecm_setup_version(${PROJECT_VERSION}
        VARIABLE_PREFIX KALENDAR
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/config-kalendar.h"
    )
endif()

################# Find dependencies #################
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core)
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Gui Qml Quick)

if(NOT BUILD_CORE_ONLY)
    # Qt6 Dependencies
    find_package(Qt6 ${QT_MIN_VERSION} COMPONENTS Svg Location QuickControls2) # All optional

    # KDE Dependencies for Akonadi Backend
    find_package(KF6 ${KF_MIN_VERSION} COMPONENTS I18n CalendarCore CoreAddons)

    find_package(KPim6Akonadi CONFIG)
    find_package(KPim6AkonadiContact CONFIG)
    find_package(KPim6AkonadiCalendar CONFIG)

    # Check if we have enough to build the backend
    if(KPim6Akonadi_FOUND AND KF6CalendarCore_FOUND AND KPim6AkonadiCalendar_FOUND)
        set(AKONADI_BACKEND_AVAILABLE ON)
        message(STATUS "Akonadi found: Enabling Akonadi Backend")
    else()
        message(STATUS "Akonadi NOT found: Disabling Akonadi Backend")
    endif()
endif()

################# build and install #################

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII -DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DQT_USE_QSTRINGBUILDER)
add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)

add_subdirectory(src/core)
add_subdirectory(src/backends/local)
add_subdirectory(src/app)

if(AKONADI_BACKEND_AVAILABLE)
    add_subdirectory(src/backends/akonadi)
endif()

# Build core-only tests
find_package(GTest)
if(GTest_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()

if(NOT BUILD_CORE_ONLY)
    # add_subdirectory(src)

    if(NOT BUILD_STANDALONE)
        install(PROGRAMS org.kde.kalendar.desktop DESTINATION ${KDE_INSTALL_APPDIR})
        install(FILES org.kde.kalendar.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
        install(FILES org.kde.kalendar.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
    endif()

    kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

    feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()

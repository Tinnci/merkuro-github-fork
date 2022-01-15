# SPDX-FileCopyrightText: (C) 2022 Claudio Cambra <claudio.cambra@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause

enable_testing()

include_directories(${kalendar_CURRENT_BINARY_DIR})
include_directories(${kalendar_BINARY_DIR})
include_directories(${kalendar_BINARY_DIR}/src)

set(incidenceOccurrenceModelTest_SOURCES
    incidenceoccurrencemodeltest.cpp
)

ecm_add_test(${incidenceOccurrenceModelTest_SOURCES}
    TEST_NAME "incidenceOccurrenceModelTest"
    LINK_LIBRARIES
        Qt::Test
        Qt5::Core
        Qt5::Gui
        KF5::ConfigCore
        KF5::AkonadiCore
        KF5::AkonadiCalendar
)

target_include_directories(incidenceOccurrenceModelTest PRIVATE ${CMAKE_SOURCE_DIR}/src)


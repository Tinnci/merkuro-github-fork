# SPDX-FileCopyrightText: (C) 2022 Claudio Cambra <claudio.cambra@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause

enable_testing()

include_directories(${kalendar_CURRENT_BINARY_DIR})
include_directories(${kalendar_BINARY_DIR})
include_directories(${kalendar_BINARY_DIR}/src)

set(incidenceOccurrenceModelTest_SOURCES
    incidenceoccurrencemodeltest.cpp
    ../src/incidenceoccurrencemodel.cpp
)

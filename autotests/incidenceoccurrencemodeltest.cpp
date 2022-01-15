// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "models/incidenceoccurrencemodel.h"

#include <QTest>

class IncidenceOccurrenceModelTest : public QObject
{
    Q_OBJECT

public:
    IncidenceOccurrenceModelTest() = default;
    ~IncidenceOccurrenceModelTest() = default;

private:
};

QTEST_MAIN(IncidenceOccurrenceModelTest)
#include "incidenceoccurrencemodeltest.moc"

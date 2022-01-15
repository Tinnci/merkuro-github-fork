// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "../src/models/incidenceoccurrencemodel.h"

#include <Akonadi/Calendar/IncidenceChanger>
#include <KCalendarCore/Incidence>
#include <KCheckableProxyModel>
#include <QSignalSpy>
#include <QTest>
#include <akonadi/qtest_akonadi.h>

class IncidenceOccurrenceModelTest : public QObject
{
    Q_OBJECT

public:
    IncidenceOccurrenceModelTest() = default;
    ~IncidenceOccurrenceModelTest() = default;

    void checkAllItems(KCheckableProxyModel *model, const QModelIndex &parent = QModelIndex())
    {
        const int rowCount = model->rowCount(parent);
        for (int row = 0; row < rowCount; ++row) {
            QModelIndex index = model->index(row, 0, parent);
            model->setData(index, Qt::Checked, Qt::CheckStateRole);

            if (model->rowCount(index) > 0) {
                checkAllItems(model, index);
            }
        }
    }

signals:
    void calendarLoaded();

private:
    QScopedPointer<Akonadi::ETMCalendar> calendar;
    QTimer loadedCheckTimer;
    Akonadi::Collection collection;

private slots:
    void initTestCase()
    {
        AkonadiTest::checkTestIsIsolated();

        calendar.reset(new Akonadi::ETMCalendar);
        QSignalSpy collectionsAdded(calendar.data(), &Akonadi::ETMCalendar::collectionsAdded);
        QVERIFY(collectionsAdded.wait(5000));
        collection = collectionsAdded.takeFirst().at(0).value<Akonadi::Collection>();
        qDebug() << QStringLiteral("USING COLLECTION: ") << collection.name() << collection.id();

        loadedCheckTimer.setInterval(300);
        loadedCheckTimer.setSingleShot(true);
        connect(&loadedCheckTimer, &QTimer::timeout, this, [&]() {
            if (calendar->isLoaded()) {
                Q_EMIT calendarLoaded();
            } else {
                loadedCheckTimer.start();
            }
        });

        QSignalSpy loaded(this, &IncidenceOccurrenceModelTest::calendarLoaded);
        loaded.wait(10000);
        checkAllItems(calendar->checkableProxyModel());
    }

    void testAddCalendar()
    {
        QDateTime now = QDateTime::currentDateTime();

        QVERIFY(calendar->isLoaded());
        QVERIFY(calendar->items().count() > 0);

        IncidenceOccurrenceModel model;
        QSignalSpy fetchFinished(&model, &QAbstractItemModel::modelReset);

        model.setStart(now.date());
        model.setLength(7);
        model.setCalendar(calendar.data());
        model.updateQuery();

        fetchFinished.wait(10000);
        QCOMPARE(model.rowCount(), 7);
    }
};

QTEST_MAIN(IncidenceOccurrenceModelTest)
#include "incidenceoccurrencemodeltest.moc"

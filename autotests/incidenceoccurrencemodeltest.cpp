// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "../src/models/incidenceoccurrencemodel.h"

#include <Akonadi/Calendar/IncidenceChanger>
#include <KCalendarCore/Incidence>
#include <KCheckableProxyModel>
#include <KFormat>
#include <QAbstractItemModelTester>
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
    IncidenceOccurrenceModel model;
    QAbstractItemModelTester modelTester = QAbstractItemModelTester(&model);
    QTimer loadedCheckTimer;
    Akonadi::Collection collection;
    QDateTime now = QDateTime::currentDateTime();

private slots:
    void initTestCase()
    {
        AkonadiTest::checkTestIsIsolated();

        calendar.reset(new Akonadi::ETMCalendar);
        QSignalSpy collectionsAdded(calendar.data(), &Akonadi::ETMCalendar::collectionsAdded);
        QVERIFY(collectionsAdded.wait(10000));
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
        QVERIFY(calendar->isLoaded());
        QVERIFY(calendar->items().count() > 0);

        QSignalSpy fetchFinished(&model, &QAbstractItemModel::modelReset);

        model.setStart(now.date());
        QCOMPARE(model.start(), now.date());
        model.setLength(7);
        QCOMPARE(model.length(), 7);
        model.setCalendar(calendar.data());
        QCOMPARE(model.calendar()->id(), calendar->id());
        model.updateQuery();

        fetchFinished.wait(10000);
        // Our test calendar file has an event that recurs every day of the week.
        // Since we are checking for 7 days, we should have an instance of this event
        // 7 times.
        QCOMPARE(model.rowCount(), 7);
    }

    void testData()
    {
        // Check everything is still there from the previous test
        QCOMPARE(model.rowCount(), 7);

        // Test that the data function gives us the event info we have in our calendar file
        const auto index = model.index(0, 0);
        QCOMPARE(index.data(IncidenceOccurrenceModel::Summary).toString(), QStringLiteral("Test event"));
        QCOMPARE(index.data(IncidenceOccurrenceModel::Description).toString(), QStringLiteral("Big testing description"));
        QCOMPARE(index.data(IncidenceOccurrenceModel::Location).toString(), QStringLiteral("Testing land"));

        // It's an all day event
        QDateTime eventStartTimeToday(now.date().startOfDay());
        QCOMPARE(index.data(IncidenceOccurrenceModel::StartTime).toDateTime().toMSecsSinceEpoch(), eventStartTimeToday.toMSecsSinceEpoch());
        QCOMPARE(index.data(IncidenceOccurrenceModel::EndTime).toDateTime().toMSecsSinceEpoch(), eventStartTimeToday.addDays(1).toMSecsSinceEpoch());

        const auto duration = index.data(IncidenceOccurrenceModel::Duration).value<KCalendarCore::Duration>();
        QCOMPARE(duration.asDays(), 1);
        KFormat format;
        QCOMPARE(index.data(IncidenceOccurrenceModel::DurationString).toString(), format.formatSpelloutDuration(duration.asSeconds() * 1000));

        QVERIFY(index.data(IncidenceOccurrenceModel::Recurs).toBool());
        QVERIFY(index.data(IncidenceOccurrenceModel::HasReminders).toBool());
        QCOMPARE(index.data(IncidenceOccurrenceModel::Priority).toInt(), 0);
        QVERIFY(index.data(IncidenceOccurrenceModel::AllDay).toBool());

        // CalendarManager generates the colors for different collections so let's skip that check, since it will give invalid

        QCOMPARE(index.data(IncidenceOccurrenceModel::CollectionId).toLongLong(), 2);

        QVERIFY(!index.data(IncidenceOccurrenceModel::TodoCompleted).toBool()); // An event should always return false for this
        QVERIFY(!index.data(IncidenceOccurrenceModel::IsOverdue).toBool());
        QVERIFY(!index.data(IncidenceOccurrenceModel::IsReadOnly).toBool());

        QVERIFY(!index.data(IncidenceOccurrenceModel::IncidenceId).toString().isNull());
        QCOMPARE(index.data(IncidenceOccurrenceModel::IncidenceType).toInt(), KCalendarCore::Incidence::TypeEvent);
        QCOMPARE(index.data(IncidenceOccurrenceModel::IncidenceTypeStr).toString(), QStringLiteral("Event"));

        QVERIFY(index.data(IncidenceOccurrenceModel::IncidencePtr).canConvert<KCalendarCore::Incidence::Ptr>());
        QVERIFY(index.data(IncidenceOccurrenceModel::IncidenceOccurrence).canConvert<IncidenceOccurrenceModel::Occurrence>());
    }
};

QTEST_MAIN(IncidenceOccurrenceModelTest)
#include "incidenceoccurrencemodeltest.moc"

// SPDX-FileCopyrightText: 2002, 2003 Cornelius Schumacher <schumacher@kde.org>
// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0

#include "kalendaralarmclient.h"

#include "kalendaracadaptor.h"

#include <CalendarSupport/Utils>

#include <KCheckableProxyModel>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QApplication>
#include <QDBusConnection>

using namespace KCalendarCore;

KalendarAlarmClient::KalendarAlarmClient(QObject *parent)
    : QObject(parent)
{
    new KalendaracAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/ac"), this);

    // Check if Akonadi is already configured
    const QString akonadiConfigFile = Akonadi::ServerManager::serverConfigFilePath(Akonadi::ServerManager::ReadWrite);
    if (QFileInfo::exists(akonadiConfigFile)) {
        // Akonadi is configured, create ETM and friends, which will start Akonadi
        // if its not running yet
        setupAkonadi();
    } else {
        // Akonadi has not been set up yet, wait for someone else to start it,
        // so that we don't unnecessarily slow session start up
        connect(Akonadi::ServerManager::self(), &Akonadi::ServerManager::stateChanged, this, [this](Akonadi::ServerManager::State state) {
            if (state == Akonadi::ServerManager::Running) {
                setupAkonadi();
            }
        });
    }

    KConfigGroup alarmGroup(KSharedConfig::openConfig(), "Alarms");
    const int interval = alarmGroup.readEntry("Interval", 60);
    // qCDebug(KOALARMCLIENT_LOG) << "KOAlarmClient check interval:" << interval << "seconds.";
    mLastChecked = alarmGroup.readEntry("CalendarsLastChecked", QDateTime::currentDateTime().addDays(-9));

    mCheckTimer.start(1000 * interval); // interval in seconds
    connect(qApp, &QApplication::commitDataRequest, this, &KalendarAlarmClient::slotCommitData);
}

void KalendarAlarmClient::setupAkonadi()
{
    const QStringList mimeTypes{Event::eventMimeType(), Todo::todoMimeType()};
    mCalendar = Akonadi::ETMCalendar::Ptr(new Akonadi::ETMCalendar(mimeTypes));
    mCalendar->setObjectName(QStringLiteral("KalendarAC's calendar"));
    mETM = mCalendar->entityTreeModel();

    connect(&mCheckTimer, &QTimer::timeout, this, &KalendarAlarmClient::checkAlarms);
    connect(mETM, &Akonadi::EntityTreeModel::collectionPopulated, this, &KalendarAlarmClient::deferredInit);
    connect(mETM, &Akonadi::EntityTreeModel::collectionTreeFetched, this, &KalendarAlarmClient::deferredInit);

    checkAlarms();
}

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

void KalendarAlarmClient::deferredInit()
{
    if (!collectionsAvailable()) {
        return;
    }

    // qCDebug(KOALARMCLIENT_LOG) << "Performing delayed initialization.";

    // load reminders that were active when quitting
    KConfigGroup genGroup(KSharedConfig::openConfig(), "General");
    const int numReminders = genGroup.readEntry("Reminders", 0);

    for (int i = 1; i <= numReminders; ++i) {
        const QString group(QStringLiteral("Incidence-%1").arg(i));
        const KConfigGroup incGroup(KSharedConfig::openConfig(), group);

        const QUrl url(incGroup.readEntry("AkonadiUrl"));
        Akonadi::Item::Id akonadiItemId = -1;
        if (!url.isValid()) {
            // logic to migrate old KOrganizer incidence uid's to a Akonadi item.
            const QString uid = incGroup.readEntry("UID");
            if (!uid.isEmpty()) {
                akonadiItemId = mCalendar->item(uid).id();
            }
        } else {
            akonadiItemId = Akonadi::Item::fromUrl(url).id();
        }

        if (akonadiItemId >= 0) {
            const QDateTime dt = incGroup.readEntry("RemindAt", QDateTime());
            Akonadi::Item i = mCalendar->item(Akonadi::Item::fromUrl(url).id());
            if (CalendarSupport::hasIncidence(i) && !CalendarSupport::incidence(i)->alarms().isEmpty()) {
                createReminder(i, dt, QString());
            }
        }
    }

    KCheckableProxyModel *checkableModel = mCalendar->checkableProxyModel();
    checkAllItems(checkableModel);

    // Now that everything is set up, a first check for reminders can be performed.
    checkAlarms();
}

bool KalendarAlarmClient::collectionsAvailable() const
{
    // The list of collections must be available.
    if (!mETM->isCollectionTreeFetched()) {
        return false;
    }

    // All collections must be populated.
    const int rowCount = mETM->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        static const int column = 0;
        const QModelIndex index = mETM->index(row, column);
        const bool haveData = mETM->data(index, Akonadi::EntityTreeModel::IsPopulatedRole).toBool();
        if (!haveData) {
            return false;
        }
    }

    return true;
}

void KalendarAlarmClient::checkAlarms()
{
    KConfigGroup cfg(KSharedConfig::openConfig(), "General");

    if (!cfg.readEntry("Enabled", true)) {
        return;
    }

    // We do not want to miss any reminders, so don't perform check unless
    // the collections are available and populated.
    if (!collectionsAvailable()) {
        // qCDebug(KOALARMCLIENT_LOG) << "Collections are not available; aborting check.";
        return;
    }

    const QDateTime from = mLastChecked.addSecs(1);
    mLastChecked = QDateTime::currentDateTime();

    // qCDebug(KOALARMCLIENT_LOG) << "Check:" << from.toString() << " -" << mLastChecked.toString();

    const Alarm::List alarms = mCalendar->alarms(from, mLastChecked, true /* exclude blocked alarms */);

    for (const Alarm::Ptr &alarm : alarms) {
        const QString uid = alarm->customProperty("ETMCalendar", "parentUid");
        const Akonadi::Item::Id id = mCalendar->item(uid).id();
        const Akonadi::Item item = mCalendar->item(id);

        createReminder(item, mLastChecked, alarm->text());
    }
}

void KalendarAlarmClient::createReminder(const Akonadi::Item &aitem, const QDateTime &remindAtDate, const QString &displayText)
{
    if (!CalendarSupport::hasIncidence(aitem)) {
        return;
    }

    if (remindAtDate.addDays(10) < mLastChecked) {
        // ignore reminders more than 10 days old
        return;
    }

    // createDialog();

    // mDialog->addIncidence(aitem, remindAtDate, displayText);
    // mDialog->wakeUp();
    saveLastCheckTime();
}

void KalendarAlarmClient::slotQuit()
{
    Q_EMIT saveAllSignal();
    saveLastCheckTime();
    quit();
}

void KalendarAlarmClient::saveLastCheckTime()
{
    KConfigGroup cg(KSharedConfig::openConfig(), "Alarms");
    cg.writeEntry("CalendarsLastChecked", mLastChecked);
    KSharedConfig::openConfig()->sync();
}

void KalendarAlarmClient::quit()
{
    // qCDebug(KOALARMCLIENT_LOG);
    qApp->quit();
}

void KalendarAlarmClient::slotCommitData(QSessionManager &)
{
    Q_EMIT saveAllSignal();
    saveLastCheckTime();
}

void KalendarAlarmClient::forceAlarmCheck()
{
    checkAlarms();
    saveLastCheckTime();
}

QString KalendarAlarmClient::dumpDebug() const
{
    KConfigGroup cfg(KSharedConfig::openConfig(), "Alarms");
    const QDateTime lastChecked = cfg.readEntry("CalendarsLastChecked", QDateTime());
    const QString str = QStringLiteral("Last Check: %1").arg(lastChecked.toString());
    return str;
}

QStringList KalendarAlarmClient::dumpAlarms() const
{
    const QDateTime start = QDateTime(QDate::currentDate(), QTime(0, 0), Qt::LocalTime);
    const QDateTime end = start.addDays(1).addSecs(-1);

    QStringList lst;
    const Alarm::List alarms = mCalendar->alarms(start, end);
    lst.reserve(1 + (alarms.isEmpty() ? 1 : alarms.count()));
    // Don't translate, this is for debugging purposes.
    lst << QStringLiteral("dumpAlarms() from ") + start.toString() + QLatin1String(" to ") + end.toString();

    if (alarms.isEmpty()) {
        lst << QStringLiteral("No alarm found.");
    } else {
        for (const Alarm::Ptr &alarm : alarms) {
            const QString uid = alarm->customProperty("ETMCalendar", "parentUid");
            const Akonadi::Item::Id id = mCalendar->item(uid).id();
            const Akonadi::Item item = mCalendar->item(id);

            const Incidence::Ptr incidence = CalendarSupport::incidence(item);
            const QString summary = incidence->summary();

            const QDateTime time = incidence->dateTime(Incidence::RoleAlarm);
            lst << QStringLiteral("%1: \"%2\" (alarm text \"%3\")").arg(time.toString(Qt::ISODate), summary, alarm->text());
        }
    }

    return lst;
}

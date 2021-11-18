// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0

#include "kalendaralarmclient.h"
#include "alarmdockwindow.h"
#include "kalendaracadaptor.h"
#include "notificationhandler.h"

#include <CalendarSupport/Utils>

#include <KCheckableProxyModel>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QApplication>
#include <QDBusConnection>
#include <QDateTime>

using namespace KCalendarCore;

KalendarAlarmClient::KalendarAlarmClient(QObject *parent)
    : QObject(parent)
{
    new KalendaracAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/ac"), this);

    m_notificationHandler = new NotificationHandler(this);

    if (dockerEnabled()) {
        mDocker = new AlarmDockWindow;
        connect(this, &KalendarAlarmClient::reminderCount, mDocker, &AlarmDockWindow::slotUpdate);
        connect(mDocker, &AlarmDockWindow::quitSignal, this, &KalendarAlarmClient::slotQuit);
    }

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
    qDebug() << "KalendarAlarmClient check interval:" << interval << "seconds.";
    mLastChecked = alarmGroup.readEntry("CalendarsLastChecked", QDateTime::currentDateTime().addDays(-9));

    mCheckTimer.start(1000 * interval); // interval in seconds
    connect(qApp, &QApplication::commitDataRequest, this, &KalendarAlarmClient::slotCommitData);
}

KalendarAlarmClient::~KalendarAlarmClient()
{
    delete mDocker;
}

void KalendarAlarmClient::setupAkonadi()
{
    const QStringList mimeTypes{Event::eventMimeType(), Todo::todoMimeType()};
    mCalendar = Akonadi::ETMCalendar::Ptr(new Akonadi::ETMCalendar(mimeTypes));
    mCalendar->setObjectName(QStringLiteral("KalendarAC's calendar"));
    mETM = mCalendar->entityTreeModel();

    connect(&mCheckTimer, &QTimer::timeout, this, &KalendarAlarmClient::checkAlarms);
    // connect(m_notificationHandler, &NotificationHandler::scheduleAlarmCheck, this, &KalendarAlarmClient::scheduleAlarmCheck);
    connect(mETM, &Akonadi::EntityTreeModel::collectionPopulated, this, &KalendarAlarmClient::restoreSuspendedFromConfig);
    connect(mETM, &Akonadi::EntityTreeModel::collectionTreeFetched, this, &KalendarAlarmClient::restoreSuspendedFromConfig);

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

void KalendarAlarmClient::restoreSuspendedFromConfig()
{
    if (!collectionsAvailable()) {
        return;
    }

    if (!collectionsAvailable()) {
        return;
    }

    qDebug() << "Performing delayed initialization.";

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
                auto uid = CalendarSupport::incidence(i)->uid();
                m_notificationHandler->addSuspendedNotification(uid, alarmText(uid), dt);
            }
        }
    }

    KCheckableProxyModel *checkableModel = mCalendar->checkableProxyModel();
    checkAllItems(checkableModel);

    // Now that everything is set up, a first check for reminders can be performed.
    checkAlarms();
}

QString KalendarAlarmClient::alarmText(const QString &uid) const
{
    const Alarm::List alarms = mCalendar->alarms(QDateTime(), QDateTime::currentDateTime(), true /* exclude blocked alarms */);

    for (const auto &alarm : qAsConst(alarms)) {
        if (alarm->parentUid() == uid) {
            return alarm->text();
        }
    }

    return QString();
}

bool KalendarAlarmClient::dockerEnabled()
{
    KConfig kalendarConfig(QStandardPaths::locate(QStandardPaths::ConfigLocation, QStringLiteral("kalendarrc")));
    KConfigGroup generalGroup(&kalendarConfig, "System Tray");
    return generalGroup.readEntry("ShowReminderDaemon", true);
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
        qDebug() << "Collections are not available; aborting check.";
        return;
    }

    const QDateTime from = mLastChecked.addSecs(1);
    mLastChecked = QDateTime::currentDateTime();

    qDebug() << "Check:" << from.toString() << " -" << mLastChecked.toString();

    const Alarm::List alarms = mCalendar->alarms(from, mLastChecked, true /* exclude blocked alarms */);
    FilterPeriod fPeriod{.from = from, .to = mLastChecked};
    m_notificationHandler->setPeriod(fPeriod);

    qDebug() << alarms.length();

    for (const Alarm::Ptr &alarm : alarms) {
        qDebug() << alarm->time() << alarm->text();
        m_notificationHandler->addActiveNotification(alarm->parentUid(),
                                                     QLatin1String("%1\n%2").arg(alarm->time().toString(QLatin1String("hh:mm")), alarm->text()));
    }

    m_notificationHandler->sendNotifications();
    saveLastCheckTime();
}

void KalendarAlarmClient::slotQuit()
{
    qDebug() << "Quit";
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
    qDebug() << "Quit";
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

void KalendarAlarmClient::hide()
{
    delete mDocker;
    mDocker = nullptr;
}

void KalendarAlarmClient::show()
{
    if (!mDocker) {
        if (dockerEnabled()) {
            mDocker = new AlarmDockWindow;
            connect(this, &KalendarAlarmClient::reminderCount, mDocker, &AlarmDockWindow::slotUpdate);
            connect(mDocker, &AlarmDockWindow::quitSignal, this, &KalendarAlarmClient::slotQuit);
        }
    }
}

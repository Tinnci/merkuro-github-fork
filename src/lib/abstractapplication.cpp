// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "abstractapplication.h"
#include <KAboutData>
#include <KAuthorized>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QDebug>
#include <QGuiApplication>
#include <QMenu>

using namespace Qt::StringLiterals;

AbstractApplication::AbstractApplication(QObject *parent)
    : KirigamiAbstractApplication(parent)
    , m_shared(KSharedConfig::openConfig())
{
}

void AbstractApplication::setupActions()
{
    KirigamiAbstractApplication::setupActions();

    auto actionName = QLatin1StringView("options_configure");
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = KStandardActions::preferences(this, &AbstractApplication::openSettings, this);
        mainCollection()->addAction(action->objectName(), action);
    }

    actionName = QLatin1StringView("open_tag_manager");
    if (KAuthorized::authorizeAction(actionName)) {
        auto openTagManagerAction = mainCollection()->addAction(actionName, this, &AbstractApplication::openTagManager);
        openTagManagerAction->setText(i18n("Manage Tags…"));
        openTagManagerAction->setIcon(QIcon::fromTheme(QStringLiteral("action-rss_tag")));
    }

    actionName = QLatin1StringView("toggle_menubar");
    if (KAuthorized::authorizeAction(actionName)) {
        auto action = mainCollection()->addAction(actionName, this, &AbstractApplication::toggleMenubar);
        action->setText(i18n("Show Menubar"));
        action->setIcon(QIcon::fromTheme(QStringLiteral("show-menu")));
        action->setCheckable(true);
        KConfigGroup config(m_shared, u"General"_s);
        action->setChecked(config.readEntry(u"showMenubar"_s, true));
        mainCollection()->setDefaultShortcut(action, Qt::CTRL | Qt::Key_M);
    }
}

void AbstractApplication::toggleMenubar()
{
    KConfigGroup config(m_shared, u"General"_s);
    auto state = !config.readEntry(u"showMenubar"_s, true);
    config.writeEntry(u"showMenubar"_s, state);
    m_shared->sync();

    Q_EMIT menubarVisibleChanged();
}

bool AbstractApplication::menubarVisible() const
{
    KConfigGroup config(m_shared, u"General"_s);
    return config.readEntry(u"showMenubar"_s, true);
}

#include "moc_abstractapplication.cpp"

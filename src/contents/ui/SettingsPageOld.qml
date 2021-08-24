// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

// TODO: Remove this on release of KF5.86

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0
import org.kde.kalendar 1.0

Kirigami.Page {
    title: i18n("Settings")
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    onBackRequested: {
        if (pageSettingStack.depth > 1 && !pageSettingStack.wideMode && pageSettingStack.currentIndex !== 0) {
            event.accepted = true;
            pageSettingStack.pop();
        }
    }
    Kirigami.PageRow {
        id: pageSettingStack
        anchors.fill: parent
        initialPage: Kirigami.ScrollablePage {
            bottomPadding: 0
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            ListView {
                property list<Kirigami.Action> actions: [
                    Kirigami.Action {
                        text: i18n("General")
                        icon.name: "korganizer"
                        onTriggered: pageSettingStack.push("qrc:/GeneralSettingsPage.qml")
                    },
                    Kirigami.Action {
                        text: i18n("Views")
                        icon.name: "view-choose"
                        onTriggered: pageSettingStack.push("qrc:/ViewSettingsPage.qml")
                    },
                    Kirigami.Action {
                        text: i18n("Calendar sources")
                        icon.name: "preferences-system-users"
                        onTriggered: pageSettingStack.push("qrc:/SourceSettingsPage.qml")
                    },
                    Kirigami.Action {
                        text: i18n("Calendars")
                        icon.name: "korganizer"
                        onTriggered: pageSettingStack.push("qrc:/CalendarSettingsPage.qml")
                    },
                    Kirigami.Action {
                        text: i18n("About Kalendar")
                        icon.name: "help-about"
                        onTriggered: pageSettingStack.push("qrc:/AboutPage.qml")
                    }
                ]
                model: actions
                Component.onCompleted: if(!Kirigami.Settings.isMobile) { actions[0].trigger(); }
                delegate: Kirigami.BasicListItem {
                    action: modelData
                }
            }
        }
    }
}


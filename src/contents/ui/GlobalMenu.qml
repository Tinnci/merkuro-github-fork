// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import Qt.labs.platform 1.1 as Labs

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Layouts 1.10
import org.kde.kirigami 2.15 as Kirigami

Labs.MenuBar {
    Labs.Menu {
        title: i18nc("@action:menu", "Kalendar")

        Labs.MenuItem {
            enabled: pageStack.layers.currentItem.title !== i18n("Settings")
            text: i18nc("@action:menu", "Preferences…")

            shortcut: StandardKey.Preferences
            onTriggered: pushReplaceLayer("qrc:/SettingsPage.qml")
        }
        Labs.MenuItem {
            text: i18nc("@action:menu", "Quit Kalendar")

            shortcut: StandardKey.Quit
            onTriggered: Qt.quit()
        }
    }
    Labs.Menu {
        title: i18nc("@action:menu", "File")

        Labs.MenuItem {
            text: i18nc("@action:menu", "Import calendar") + " (todo)"
        }

        NativeMenuItemFromAction {
            kalendarAction: 'create_event'
        }

        NativeMenuItemFromAction {
            kalendarAction: 'create_todo'
        }
    }
    EditMenu {
        title: i18nc("@action:menu", "Edit")
        field: (root.activeFocusItem instanceof TextEdit || root.activeFocusItem instanceof TextInput) ? root.activeFocusItem : null
    }
    Labs.Menu {
        title: i18nc("@action:menu", "View")

        NativeMenuItemFromAction {
            kalendarAction: 'open_month_view'
        }

        NativeMenuItemFromAction {
            kalendarAction: 'open_schedule_view'
        }

        NativeMenuItemFromAction {
            kalendarAction: 'open_todo_view'
        }
    }
    Labs.Menu {
        title: i18nc("@action:menu", "Window")

        Labs.MenuItem {
            text: root.visibility === Window.FullScreen ? i18nc("@action:menu", "Exit Full Screen") : i18nc("@action:menu", "Enter Full Screen")
            onTriggered: root.visibility === Window.FullScreen ? root.showNormal() : root.showFullScreen()
        }
    }
    Labs.Menu {
        title: i18nc("@action:menu", "Help")

        Labs.MenuItem {
            text: i18nc("@action:menu", "Kalendar Handbook") + " (todo help wanted)"
        }
    }
}


// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Layouts 1.10
import org.kde.kirigami 2.15 as Kirigami
import QtQuick.Window 2.15
import org.kde.kalendar 1.0

QQC2.MenuBar {
    id: bar
    property var parentWindow: null
    property bool todoMode: false

    QQC2.Menu {
        title: i18nc("@action:menu", "File")

        QQC2.MenuItem {
            text: i18nc("@action:menu", "Import calendar") // todo
            visible: false
        }

        QQC2.MenuItem {
            text: i18nc("@action:menu", "Quit Kalendar")
            onTriggered: Qt.quit()
        }
    }
    QQC2.Menu {
        id: editMenu
        title: i18nc("@action:menu", "Edit")
        Connections {
            target: parentWindow
            onActiveFocusItemChanged: {
                if (parentWindow.activeFocusItem instanceof TextEdit || parentWindow.activeFocusItem instanceof TextInput) {
                    editMenu.field = parentWindow.activeFocusItem;
                }
            }
        }
        field: null

        required property Item field

        Kirigami.Action {
            icon.name: KalendarApplication.iconName(undoAction.icon)
            text: undoAction.text
            shortcut: undoAction.shortcut
            enabled: undoAction.enabled
            onTriggered: undoAction.trigger()
        }

        Kirigami.Action {
            icon.name: KalendarApplication.iconName(redoAction.icon)
            text: redoAction.text
            shortcut: redoAction.shortcut
            enabled: redoAction.enabled
            onTriggered: redoAction.trigger()
        }

        QQC2.MenuSeparator {
        }

        QQC2.MenuItem {
            enabled: editMenu.field !== null && editMenu.field.canUndo
            text: i18nc("text editing menu action", "Undo text")
            onTriggered: {
                editMenu.field.undo()
                editMenu.close()
            }
        }

        QQC2.MenuItem {
            enabled: editMenu.field !== null && editMenu.field.canRedo
            text: i18nc("text editing menu action", "Redo text")
            onTriggered: {
                editMenu.field.undo()
                editMenu.close()
            }
        }

        QQC2.MenuSeparator {
        }

        QQC2.MenuItem {
            enabled: editMenu.field !== null && editMenu.field.selectedText
            text: i18nc("text editing menu action", "Cut")
            onTriggered: {
                editMenu.field.cut()
                editMenu.close()
            }
        }

        QQC2.MenuItem {
            enabled: editMenu.field !== null && editMenu.field.selectedText
            text: i18nc("text editing menu action", "Copy")
            onTriggered: {
                editMenu.field.copy()
                editMenu.close()
            }
        }

        QQC2.MenuItem {
            enabled: editMenu.field !== null && editMenu.field.canPaste
            text: i18nc("text editing menu action", "Paste")
            onTriggered: {
                editMenu.field.paste()
                editMenu.close()
            }
        }

        QQC2.MenuItem {
            enabled: editMenu.field !== null && editMenu.field.selectedText !== ""
            text: i18nc("text editing menu action", "Delete")
            onTriggered: {
                editMenu.field.remove(editMenu.field.selectionStart, editMenu.field.selectionEnd)
                editMenu.close()
            }
        }

        QQC2.MenuSeparator {
        }

        QQC2.MenuItem {
            enabled: editMenu.field !== null
            text: i18nc("text editing menu action", "Select All")
            onTriggered: {
                editMenu.field.selectAll()
                editMenu.close()
            }
        }
    }
    QQC2.Menu {
        title: i18nc("@action:menu", "View")

        Kirigami.Action {
            icon.name: KalendarApplication.iconName(monthViewAction.icon)
            text: monthViewAction.text
            shortcut: monthViewAction.shortcut
            checkable: true
            checked: monthViewAction.checked
            onTriggered: monthViewAction.trigger()
        }
        Kirigami.Action {
            icon.name: KalendarApplication.iconName(scheduleViewAction.icon)
            text: scheduleViewAction.text
            shortcut: scheduleViewAction.shortcut
            checkable: true
            checked: scheduleViewAction.checked
            onTriggered: scheduleViewAction.trigger()
        }
        Kirigami.Action {
            icon.name: KalendarApplication.iconName(todoViewAction.icon)
            text: todoViewAction.text
            shortcut: todoViewAction.shortcut
            checkable: true
            checked: todoViewAction.checked
            onTriggered: todoViewAction.trigger()
        }

        QQC2.MenuSeparator {
        }

        QQC2.Menu {
            title: i18n("Sort todos")
            enabled: bar.todoMode

            Kirigami.Action {
                icon.name: KalendarApplication.iconName(todoViewSortByDueDateAction.icon)
                text: todoViewSortByDueDateAction.text
                shortcut: todoViewSortByDueDateAction.shortcut
                checkable: todoViewSortByDueDateAction.checkable
                checked: todoViewSortByDueDateAction.checked
                onTriggered: todoViewSortByDueDateAction.trigger()
            }
            Kirigami.Action {
                icon.name: KalendarApplication.iconName(todoViewSortByPriorityAction.icon)
                text: todoViewSortByPriorityAction.text
                shortcut: todoViewSortByPriorityAction.shortcut
                checkable: todoViewSortByPriorityAction.checkable
                checked: todoViewSortByPriorityAction.checked
                onTriggered: todoViewSortByPriorityAction.trigger()
            }
            Kirigami.Action {
                icon.name: KalendarApplication.iconName(todoViewSortAlphabeticallyAction.icon)
                text: todoViewSortAlphabeticallyAction.text
                shortcut: todoViewSortAlphabeticallyAction.shortcut
                checkable: todoViewSortAlphabeticallyAction.checkable
                checked: todoViewSortAlphabeticallyAction.checked
                onTriggered: todoViewSortAlphabeticallyAction.trigger()
            }

            QQC2.MenuSeparator {
            }

            Kirigami.Action {
                icon.name: KalendarApplication.iconName(todoViewOrderAscendingAction.icon)
                text: todoViewOrderAscendingAction.text
                shortcut: todoViewOrderAscendingAction.shortcut
                checkable: todoViewOrderAscendingAction.checkable
                checked: todoViewOrderAscendingAction.checked
                onTriggered: todoViewOrderAscendingAction.trigger()
            }
            Kirigami.Action {
                icon.name: KalendarApplication.iconName(todoViewOrderDescendingAction.icon)
                text: todoViewOrderDescendingAction.text
                shortcut: todoViewOrderDescendingAction.shortcut
                checkable: todoViewOrderDescendingAction.checkable
                checked: todoViewOrderDescendingAction.checked
                onTriggered: todoViewOrderDescendingAction.trigger()
            }
        }

    }
    QQC2.Menu {
        title: i18nc("@action:menu", "Actions")

        Kirigami.Action {
            icon.name: KalendarApplication.iconName(createEventAction.icon)
            text: createEventAction.text
            shortcut: createEventAction.shortcut
            onTriggered: createEventAction.trigger()
        }
        Kirigami.Action {
            icon.name: KalendarApplication.iconName(createTodoAction.icon)
            text: createTodoAction.text
            shortcut: createTodoAction.shortcut
            onTriggered: createTodoAction.trigger()
        }
    }
    QQC2.Menu {
        title: i18nc("@action:menu", "Window")

        QQC2.MenuItem {
            text: root.visibility === Window.FullScreen ? i18nc("@action:menu", "Exit Full Screen") : i18nc("@action:menu", "Enter Full Screen")
            onTriggered: root.visibility === Window.FullScreen ? root.showNormal() : root.showFullScreen()
        }
    }
    QQC2.Menu {
        title: i18nc("@action:menu", "Settings")
        Kirigami.Action {
            text: i18n("Settings")
            icon.name: KalendarApplication.iconName(configureAction.icon)
            onTriggered: configureAction.trigger()
            shortcut: configureAction.shortcut
        }
    }
    QQC2.Menu {
        title: i18nc("@action:menu", "Help")

        QQC2.MenuItem {
            text: i18nc("@action:menu", "Kalendar Handbook") // todo
            visible: false
        }
    }
}



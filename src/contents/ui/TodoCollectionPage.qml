// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>

// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar

Kirigami.PageRow {
    id: todoPageRow

    signal viewTodo(var todoData, var collectionData)

    globalToolBar.style: Kirigami.ApplicationHeaderStyle.Auto
    globalToolBar.canContainHandles: true
    globalToolBar.showNavigationButtons: true

    Connections {
        target: todoPageRow.currentItem
        function onViewTodo(todoData, collectionData) {
            todoPageRow.viewTodo(todoData, collectionData)
        }
    }

    initialPage: Kirigami.ScrollablePage {
        title: i18n("Todo calendars")

        actions.main: Kirigami.Action {
            text: i18n("View all")
            onTriggered: todoPageRow.push("qrc:/TodoPage.qml")
        }
        Component.onCompleted: todoPageRow.push("qrc:/TodoPage.qml")

        ListView {
            model: Kalendar.CalendarManager.todoCollections
            delegate: Kirigami.BasicListItem {
                property int itemCollectionId: collectionId
                leading: QQC2.CheckBox {
                    visible: model.checkState != null
                    checked: model.checkState == 2
                    onClicked: model.checkState = (checked ? 2 : 0)
                }
                trailing: Rectangle {
                    Layout.fillHeight: true
                    width: height
                    radius: 5
                    color: collectionColor
                    visible: collectionColor !== undefined
                }
                label: display
                icon: decoration
                onClicked: todoPageRow.push("qrc:/TodoPage.qml", {filterCollectionId: collectionId})
            }
        }
    }
}

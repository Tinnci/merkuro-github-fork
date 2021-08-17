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

    initialPage: Kirigami.ScrollablePage {
        title: i18n("Calendars")

        actions.main: Kirigami.Action {
            text: i18n("View all")
            onTriggered: pageStack.push(allTodosPage)
        }
        Component.onCompleted: todoPageRow.push(allTodosPage)

        Component {
            id: allTodosPage
            TodoView {
                onViewTodo: todoPageRow.viewTodo(todoData, collectionData)
            }
        }

        ListView {
            model: Kalendar.CalendarManager.todoCollections
            delegate: Kirigami.BasicListItem {
                property int itemCollectionId: collectionId
                leftPadding: ((Kirigami.Units.gridUnit * 2) * (kDescendantLevel - 1)) + Kirigami.Units.largeSpacing
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
            }
        }
    }
}

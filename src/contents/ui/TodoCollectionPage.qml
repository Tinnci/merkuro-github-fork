// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>

// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar

Kirigami.PageRow {
    id: todoPageRow

    signal addTodo()
    signal viewTodo(var todoData, var collectionData)
    signal editTodo(var todoPtr, var collectionData)
    signal deleteTodo(var todoPtr, date deleteDate)

    globalToolBar.style: Kirigami.ApplicationHeaderStyle.Auto
    globalToolBar.canContainHandles: true
    globalToolBar.showNavigationButtons: true

    Connections {
        target: todoPageRow.currentItem
        function onAddTodo() {
            todoPageRow.addTodo();
        }
        function onViewTodo(todoData, collectionData) {
            todoPageRow.viewTodo(todoData, collectionData);
        }
        function onEditTodo(todoPtr, collectionData) {
            todoPageRow.editTodo(todoPtr, collectionData);
        }
        function onDeleteTodo(todoPtr, deleteDate) {
            todoPageRow.deleteTodo(todoPtr, deleteDate);
        }
    }

    initialPage: Kirigami.ScrollablePage {
        id: listPage
        title: i18n("Todo lists")

        Loader {
            id: allTodosPageLoader
            active: true
            asynchronous: true
            sourceComponent: TodoPage {}
            visible: false
            onLoaded: if (loadingPage.visible) {
                todoPageRow.pop(listPage);
                todoPageRow.push(allTodosPageLoader.item);
            }
        }

        Kirigami.Page {
            id: loadingPage
            QQC2.BusyIndicator {
                anchors.centerIn: parent
                running: true
            }
        }

        Component.onCompleted: todoPageRow.push(loadingPage)

        ListView {
            currentIndex: -1
            header: ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                QQC2.ToolButton {
                    Layout.fillWidth: true
                    icon.name: "view-process-all"
                    text: i18n("View all todos")
                    onClicked: todoPageRow.push(allTodosPageLoader.item)
                }
            }

            model: Kalendar.CalendarManager.todoCollections
            delegate: Kirigami.BasicListItem {
                property int itemCollectionId: collectionId

                leftPadding: ((Kirigami.Units.gridUnit * 2) * (kDescendantLevel - 1)) + Kirigami.Units.largeSpacing
                enabled: model.checkState != null
                trailing: QQC2.CheckBox {
                    id: collectionCheckbox

                    indicator: Rectangle {
                        height: parent.height * 0.8
                        width: height
                        x: collectionCheckbox.leftPadding
                        y: parent.height / 2 - height / 2
                        radius: 3
                        border.color: model.collectionColor
                        color: Qt.rgba(0,0,0,0)

                        Rectangle {
                            width: parent.width * 0.66
                            height: parent.width * 0.66
                            anchors.centerIn: parent
                            color: model.collectionColor
                            visible: model.checkState == 2
                        }
                    }
                    checked: model.checkState == 2
                    onClicked: model.checkState = model.checkState === 0 ? 2 : 0
                }

                label: display

                Loader {
                    id: todoPageLoader
                    active: true
                    asynchronous: true
                    sourceComponent: TodoPage {
                        filterCollectionId: collectionId
                    }
                    visible: false
                }

                onClicked: if(model.checkState != null) {
                    model.checkState = 2;
                    todoPageRow.push(todoPageLoader.item);
                }
            }
        }
    }
}

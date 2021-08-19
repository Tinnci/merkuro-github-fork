// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>

// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils
import "labelutils.js" as LabelUtils

QQC1.TreeView {
    id: root

    signal viewTodo(var todoData, var collectionData)
    signal editTodo(var todoPtr, var collectionData)
    signal deleteTodo(var todoPtr, date deleteDate)

    property date currentDate: new Date()
    property int filterCollectionId
    property int showCompleted: Kalendar.TodoSortFilterProxyModel.ShowAll
    property int sortBy: Kalendar.TodoSortFilterProxyModel.EndTimeColumn
    onSortByChanged: todoModel.sortTodoModel(sortBy, ascendingOrder)
    property bool ascendingOrder: false
    onAscendingOrderChanged: todoModel.sortTodoModel(sortBy, ascendingOrder)

    QQC1.TableViewColumn {
        Layout.fillWidth: true
        title: "Summary"
        role: "summary"
    }
    headerVisible: false
    flickableItem.interactive: Kirigami.Settings.isMobile
    model: Kalendar.TodoSortFilterProxyModel {
        id: todoModel
        calendar: Kalendar.CalendarManager.calendar
        incidenceChanger: Kalendar.CalendarManager.incidenceChanger
        filterCollectionId: root.filterCollectionId ? root.filterCollectionId : -1
        showCompleted: root.showCompleted
    }
    rowDelegate: RowLayout {
        height: listItem.height
        Kirigami.BasicListItem {
            id: listItem

            property bool isOverdue: root.currentDate > model.endTime

            highlighted: ListView.isCurrentItem
            label: model.text
            labelItem.font.strikeout: model.checked
            subtitle: !isNaN(model.endTime.getTime()) ? LabelUtils.todoDateTimeLabel(model.endTime) : null
            subtitleItem.color: isOverdue ? Kirigami.Theme.negativeTextColor : Kirigami.Theme.textColor
            Layout.leftMargin: Kirigami.Units.smallSpacing + (Kirigami.Units.gridUnit * (model.treeDepth + 1))

            leading: QQC2.CheckBox {
                id: todoCheckbox

                indicator: Rectangle {
                    implicitWidth: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit : Kirigami.Units.gridUnit * 0.75
                    implicitHeight: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit : Kirigami.Units.gridUnit * 0.75
                    x: todoCheckbox.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 100
                    border.color: model.color
                    color: Qt.rgba(0,0,0,0)

                    Rectangle {
                        width: parent.width * 0.66
                        height: parent.width * 0.66
                        anchors.centerIn: parent
                        radius: 100
                        color: model.color
                        visible: todoCheckbox.checked
                    }
                }
                checked: model.checked
                onClicked: model.checked = model.checked === 0 ? 2 : 0
            }
            onClicked: root.viewTodo(model, Kalendar.CalendarManager.getCollectionDetails(model.collectionId))

            IncidenceMouseArea {
                anchors.fill: parent
                incidenceData: model
                collectionDetails: Kalendar.CalendarManager.getCollectionDetails(model.collectionId)

                onViewClicked: root.viewTodo(model, Kalendar.CalendarManager.getCollectionDetails(model.collectionId))
                onEditClicked: root.editTodo(model.incidencePtr, Kalendar.CalendarManager.getCollectionDetails(model.collectionId))
                onDeleteClicked: root.deleteTodo(model.incidencePtr, model.endTime ? model.endTime : model.startTime ? model.startTime : null)
                onTodoCompletedClicked: model.checked = model.checked === 0 ? 2 : 0
            }
        }
    }
}

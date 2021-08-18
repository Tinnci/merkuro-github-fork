// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
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

Kirigami.Page {
    id: root
    title: i18n("Todos")

    signal viewTodo(var todoData, var collectionData)

    property int filterCollectionId
    property var filterCollectionDetails: filterCollectionId ? todoModel.getCollectionDetails(filterCollectionId) : null
    property int sortBy: Kalendar.TodoSortFilterProxyModel.EndTimeColumn
    onSortByChanged: todoModel.sortTodoModel(sortBy, ascendingOrder)
    property bool ascendingOrder: headerButtonGroup.checkedButton.ascending
    onAscendingOrderChanged: todoModel.sortTodoModel(sortBy, ascendingOrder)
    readonly property color standardTextColor: Kirigami.Theme.textColor
    readonly property bool isDark: LabelUtils.isDarkColor(Kirigami.Theme.backgroundColor)

    actions {
        main: Kirigami.Action {
            text: i18n("Add todo")
            icon.name: "list-add"
            onTriggered: root.setUpAdd(Kalendar.IncidenceWrapper.TypeTodo);
        }
        left: Kirigami.Action {
            text: i18n("Sort...")
            icon.name: "view-sort"

            Kirigami.Action {
                text: i18n("By due date")
                onTriggered: root.sortBy = Kalendar.TodoSortFilterProxyModel.EndTimeColumn
            }
            Kirigami.Action {
                text: i18n("By priority")
                onTriggered: root.sortBy = Kalendar.TodoSortFilterProxyModel.PriorityIntColumn
            }
            Kirigami.Action {
                text: i18n("Alphabetically")
                onTriggered: root.sortBy = Kalendar.TodoSortFilterProxyModel.SummaryColumn
            }
        }

    }

    ColumnLayout {
        anchors.fill: parent
        QQC2.ButtonGroup {
            id: headerButtonGroup
            exclusive: true
        }
            RowLayout {
                id: headerLayout
                Kirigami.Heading {
                    Layout.fillWidth: true
                    text: root.filterCollectionDetails ? root.filterCollectionDetails.displayName : i18n("All todos")
                    color: root.filterCollectionDetails ?
                        LabelUtils.getIncidenceLabelColor(root.filterCollectionDetails.color, root.isDark) : Kirigami.Theme.textColor
                }
                Column {
                    QQC2.RadioButton {
                        readonly property bool ascending: true
                        text: i18n("Ascending")
                        QQC2.ButtonGroup.group: headerButtonGroup
                    }
                    QQC2.RadioButton {
                        readonly property bool ascending: false
                        text: i18n("Descending")
                        checked: true
                        QQC2.ButtonGroup.group: headerButtonGroup
                    }
                }
            }
        QQC1.TreeView {
            Layout.fillWidth: true
            Layout.fillHeight: true

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
            }
            rowDelegate: RowLayout {
                height: listItem.height
                Kirigami.BasicListItem {
                    id: listItem
                    highlighted: ListView.isCurrentItem
                    label: model.text
                    labelItem.font.strikeout: model.checked
                    subtitle: model.endTime
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
                    onClicked: viewTodo(model, todoModel.getCollectionDetails(model.collectionId))
                }
            }
        }
    }
}

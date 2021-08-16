// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>

// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils
import "labelutils.js" as LabelUtils

Kirigami.ScrollablePage {
    id: root
    title: i18n("Todos")

    signal viewTodo(var todoData, var collectionData)

    property int sortBy: Kalendar.TodoSortFilterProxyModel.SortDueDate
    onSortByChanged: Kalendar.CalendarManager.todoModel.sortTodoModel(sortBy, ascendingOrder)
    property bool ascendingOrder: headerButtonGroup.checkedButton.ascending
    onAscendingOrderChanged: Kalendar.CalendarManager.todoModel.sortTodoModel(sortBy, ascendingOrder)
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
                onTriggered: root.sortBy = Kalendar.TodoSortFilterProxyModel.SortDueDate
            }
            Kirigami.Action {
                text: i18n("By priority")
                onTriggered: root.sortBy = Kalendar.TodoSortFilterProxyModel.SortPriority
            }
            Kirigami.Action {
                text: i18n("Alphabetically")
                onTriggered: root.sortBy = Kalendar.TodoSortFilterProxyModel.SortName
            }
        }

    }

    ListView {
        QQC2.ButtonGroup {
            id: headerButtonGroup
            exclusive: true
        }
        header: RowLayout {
            id: headerLayout
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
        model: Kalendar.CalendarManager.todoModel
        delegate: Kirigami.BasicListItem {
            highlighted: ListView.isCurrentItem
            label: model.summary
            labelItem.font.strikeout: model.checked
            subtitle: model.dueDate
            leftPadding: ((Kirigami.Units.gridUnit * 1.5) * (kDescendantLevel - 1)) + Kirigami.Units.largeSpacing

            leading: QQC2.CheckBox {
                id: todoCheckbox

                indicator: Rectangle {
                    implicitWidth: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit * 1.25 : Kirigami.Units.gridUnit * 0.75
                    implicitHeight: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit * 1.25 : Kirigami.Units.gridUnit * 0.75
                    x: todoCheckbox.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 100
                    border.color: model.color
                    color: {
                        let color = LabelUtils.getIncidenceBackgroundColor(model.color, root.isDark);
                        color.a = 0.3;
                        return color;
                    }

                    Rectangle {
                        width: parent.width * 0.66
                        height: parent.width * 0.66
                        anchors.centerIn: parent
                        radius: 100
                        color: LabelUtils.getIncidenceLabelColor(model.color, root.isDark)
                        visible: todoCheckbox.checked
                    }
                }
                checked: model.checked
            }
            onClicked: viewTodo(Kalendar.CalendarManager.todoModel.getTodoData(index), Kalendar.CalendarManager.todoModel.getCollectionDetails(index))
        }
    }
}

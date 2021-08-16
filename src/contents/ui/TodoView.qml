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

    readonly property color standardTextColor: Kirigami.Theme.textColor
    readonly property bool isDark: LabelUtils.isDarkColor(Kirigami.Theme.backgroundColor)

    ListView {
        model: Kalendar.CalendarManager.todoModel
        delegate: Kirigami.BasicListItem {
            property var todoData: Kalendar.CalendarManager.getTodoData(index)

            highlighted: ListView.isCurrentItem
            label: model.summary
            subtitle: model.dueDate
            leftPadding: ((Kirigami.Units.gridUnit * 1.5) * (kDescendantLevel - 1)) + Kirigami.Units.largeSpacing
            leading: QQC2.CheckBox {
                id: todoCheckbox

                indicator: Rectangle {
                    implicitWidth: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit : Kirigami.Units.gridUnit * 0.75
                    implicitHeight: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit : Kirigami.Units.gridUnit * 0.75
                    x: todoCheckbox.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 3
                    border.color: todoData.color
                    color: {
                        let color = LabelUtils.getIncidenceBackgroundColor(todoData.color, root.isDark);
                        color.a = 0.3;
                        return color;
                    }

                    Rectangle {
                        width: parent.width * 0.66
                        height: parent.width * 0.66
                        anchors.centerIn: parent
                        radius: 2
                        color: LabelUtils.getIncidenceLabelColor(todoData.color, root.isDark)
                        visible: todoCheckbox.checked
                    }
                }
                checked: model.checked
            }
            onClicked: viewTodo(todoData, Kalendar.CalendarManager.getTodoModelCollectionDetails(index))
        }
    }
}

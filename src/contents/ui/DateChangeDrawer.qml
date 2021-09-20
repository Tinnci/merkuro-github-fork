// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>

// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import "dateutils.js" as DateUtils

QQC2.Popup {
    id: root

    property date date: new Date()
    property bool showDay: true

    GridLayout {
        rows: 2
        columns: 3

        RowLayout {
            id: dayNormalLayout
            visible: root.showDay

            QQC2.ToolButton {
                icon.name: "go-previous"
                onClicked: root.date = DateUtils.addDaysToDate(root.date, -1)
            }
            QQC2.ToolButton {
                id: dayButton
                text: root.date.getDate()
                onClicked: {
                    dayEditLayout.visible = true;
                    dayNormalLayout.visible = false;
                }
            }
            QQC2.ToolButton {
                icon.name: "go-next"
                onClicked: root.date = DateUtils.addDaysToDate(root.date, 1)
            }
        }
        RowLayout {
            id: dayEditLayout
            visible: false

            QQC2.TextField {
                text: root.date.getDate()
                visible: root.showDay
            }
            QQC2.ToolButton {
                icon.name: "gtk-yes"
            }
            QQC2.ToolButton {
                icon.name: "gtk-cancel"
                onClicked: {
                    dayEditLayout.visible = false
                    dayNormalLayout.visible = true
                }
            }
        }

        RowLayout {
            id: monthNormalLayout

            QQC2.ToolButton {
                icon.name: "go-previous"
                onClicked: root.date = DateUtils.previousMonth(root.date)
            }
            QQC2.ToolButton {
                id: monthButton
                text: Qt.locale().monthName(root.date.getMonth())
                onClicked: {
                    monthEditLayout.visible = true;
                    monthNormalLayout.visible = false;
                }
            }
            QQC2.ToolButton {
                icon.name: "go-next"
                onClicked: root.date = DateUtils.nextMonth(root.date)
            }
        }
        RowLayout {
            id: monthEditLayout
            visible: false

            QQC2.TextField {
                text: root.date.getMonth()
            }
            QQC2.ToolButton {
                icon.name: "gtk-yes"
            }
            QQC2.ToolButton {
                icon.name: "gtk-cancel"
                onClicked: {
                    monthEditLayout.visible = false;
                    monthNormalLayout.visible = true;
                }
            }
        }

        RowLayout {
            id: yearNormalLayout

            QQC2.ToolButton {
                icon.name: "go-previous"
                onClicked: root.date = new Date(root.date.getFullYear() - 1, root.date.getMonth(), root.date.getDate())
            }
            QQC2.ToolButton {
                id: yearButton
                text: root.date.getFullYear()
                onClicked: {
                    yearEditLayout.visible = true;
                    yearNormalLayout.visible = false;
                }
            }
            QQC2.ToolButton {
                icon.name: "go-next"
                onClicked: root.date = new Date(root.date.getFullYear() + 1, root.date.getMonth(), root.date.getDate())
            }
        }
        RowLayout {
            id: yearEditLayout
            visible: false

            QQC2.TextField {
                id: yearField
                text: root.date.getFullYear()
            }
            QQC2.ToolButton {
                icon.name: "gtk-yes"
            }
            QQC2.ToolButton {
                icon.name: "gtk-cancel"
                onClicked: {
                    yearEditLayout.visible = false;
                    yearNormalLayout.visible = true;
                }
            }
        }
    }
}

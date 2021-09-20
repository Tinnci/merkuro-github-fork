// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>

// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import "dateutils.js" as DateUtils

QQC2.Popup {
    id: root

    signal dateSelected(date date)

    property date date: new Date()
    property bool showDay: true

    ColumnLayout {
        RowLayout {
            id: dateChangingLayout
            RowLayout {
                id: dayNormalLayout
                Layout.fillWidth: true
                visible: root.showDay

                QQC2.ToolButton {
                    icon.name: "go-previous"
                    onClicked: {
                        root.date = DateUtils.addDaysToDate(root.date, -1);
                        root.dateSelected(root.date);
                    }
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
                    onClicked: {
                        root.date = DateUtils.addDaysToDate(root.date, 1);
                        root.dateSelected(root.date);
                    }
                }
            }
            RowLayout {
                id: dayEditLayout
                Layout.fillWidth: true
                visible: false

                QQC2.SpinBox {
                    value: root.date.getDate()
                    onValueChanged: if(visible) {
                        root.date = new Date (root.date.getFullYear(), root.date.getMonth(), value);
                        root.dateSelected(root.date);
                    }
                    visible: root.showDay
                }
                QQC2.ToolButton {
                    icon.name: "answer"
                    onClicked: {
                        dayEditLayout.visible = false
                        dayNormalLayout.visible = true
                    }
                }
            }

            RowLayout {
                id: monthNormalLayout
                Layout.fillWidth: true

                QQC2.ToolButton {
                    icon.name: "go-previous"
                    onClicked: {
                        root.date = DateUtils.previousMonth(root.date);
                        root.dateSelected(root.date);
                    }
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
                    onClicked: {
                        root.date = DateUtils.nextMonth(root.date);
                        root.dateSelected(root.date);
                    }
                }
            }
            RowLayout {
                id: monthEditLayout
                Layout.fillWidth: true
                visible: false

                QQC2.SpinBox {
                    value: root.date.getMonth() + 1
                    onValueChanged: if(visible) {
                        root.date = new Date (root.date.getFullYear(), value - 1, root.date.getDate());
                        root.dateSelected(root.date);
                    }
                }
                QQC2.ToolButton {
                    icon.name: "answer"
                    onClicked: {
                        monthEditLayout.visible = false;
                        monthNormalLayout.visible = true;
                    }
                }
            }

            RowLayout {
                id: yearNormalLayout
                Layout.fillWidth: true

                QQC2.ToolButton {
                    icon.name: "go-previous"
                    onClicked: {
                        root.date = new Date(root.date.getFullYear() - 1, root.date.getMonth(), root.date.getDate());
                        root.dateSelected(root.date);
                    }
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
                    onClicked: {
                        root.date = new Date(root.date.getFullYear() + 1, root.date.getMonth(), root.date.getDate());
                        root.dateSelected(root.date);
                    }
                }
            }
            RowLayout {
                id: yearEditLayout
                visible: false

                QQC2.SpinBox {
                    id: yearField
                    value: root.date.getFullYear()
                    onValueChanged: if(visible) {
                        root.date = new Date (value, root.date.getMonth(), root.date.getDate());
                        root.dateSelected(root.date);
                    }
                    from: -10000
                    to: 10000
                }
                QQC2.ToolButton {
                    icon.name: "answer"
                    onClicked: {
                        yearEditLayout.visible = false;
                        yearNormalLayout.visible = true;
                    }
                }
            }
        }

        RowLayout {
            id: bottomButtonsLayout
            Layout.alignment: Qt.AlignRight

            QQC2.Button {
                Layout.row: 1
                Layout.column: 1
                icon.name: "go-jump-today"
                text: i18n("Today")
                onClicked: {
                    root.date = new Date();
                    root.dateSelected(root.date);
                }
            }
            QQC2.Button {
                Layout.row: 1
                Layout.column: 2
                icon.name: "answer"
                text: i18n("Done")
                onClicked: root.close()
            }
        }
    }
}

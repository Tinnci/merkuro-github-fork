// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils

Item {
    id: datepicker

    signal datePicked(date pickedDate)

    property date selectedDate: new Date() // Decides calendar span
    property date highlightDate: new Date() // Highlights a date, e.g. event date
    property date clickedDate: new Date() // User's chosen date
    property date today: new Date()
    property int year: selectedDate.getFullYear()
    property int month: selectedDate.getMonth()
    property int day: selectedDate.getDate()
    property bool showDays: true
    onShowDaysChanged: if (!showDays) pickerView.currentIndex = 1;

    function prevMonth() {
        selectedDate = new Date(selectedDate.getFullYear(), selectedDate.getMonth() - 1, selectedDate.getDate())
    }

    function nextMonth() {
        selectedDate = new Date(selectedDate.getFullYear(), selectedDate.getMonth() + 1, selectedDate.getDate())
    }

    function prevYear() {
        selectedDate = new Date(selectedDate.getFullYear() - 1, selectedDate.getMonth(), selectedDate.getDate())
    }

    function nextYear() {
        selectedDate = new Date(selectedDate.getFullYear() + 1, selectedDate.getMonth(), selectedDate.getDate())
    }

    function prevDecade() {
        selectedDate = new Date(selectedDate.getFullYear() - 10, selectedDate.getMonth(), selectedDate.getDate())
    }

    function nextDecade() {
        selectedDate = new Date(selectedDate.getFullYear() + 10, selectedDate.getMonth(), selectedDate.getDate())
    }

    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            id: headingRow
            Layout.fillWidth: true

            Kirigami.Heading {
                id: monthLabel
                Layout.fillWidth: true
                text: i18nc("%1 is month name, %2 is year", "%1 %2", Qt.locale().standaloneMonthName(selectedDate.getMonth()), String(selectedDate.getFullYear()))
                level: 1
            }
            QQC2.ToolButton {
                icon.name: 'go-previous-view'
                onClicked: {
                    if (pickerView.currentIndex == 1) { // monthGrid index
                        prevYear()
                    } else if (pickerView.currentIndex == 2) { // yearGrid index
                        prevDecade()
                    } else { // dayGrid index
                        prevMonth()
                    }
                }
            }
            QQC2.ToolButton {
                icon.name: 'go-jump-today'
                onClicked: selectedDate = new Date()
            }
            QQC2.ToolButton {
                icon.name: 'go-next-view'
                onClicked: {
                    if (pickerView.currentIndex == 1) { // monthGrid index
                        nextYear()
                    } else if (pickerView.currentIndex == 2) { // yearGrid index
                        nextDecade()
                    } else { // dayGrid index
                        nextMonth()
                    }
                }
            }
        }

        QQC2.TabBar {
            id: rangeBar
            currentIndex: pickerView.currentIndex
            Layout.fillWidth: true

            QQC2.TabButton {
                id: daysViewCheck
                Layout.fillWidth: true
                text: i18n("Days")
                onClicked: pickerView.currentIndex = 0 // dayGrid is first item in pickerView
                visible: datepicker.showDays
                width: visible ? implicitWidth : 0
            }
            QQC2.TabButton {
                id: monthsViewCheck
                Layout.fillWidth: true
                text: i18n("Months")
                onClicked: pickerView.currentIndex = 1
            }
            QQC2.TabButton {
                id: yearsViewCheck
                Layout.fillWidth: true
                text: i18n("Years")
                onClicked: pickerView.currentIndex = 2
            }
        }

        QQC2.SwipeView {
            id: pickerView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            interactive: false

            QQC2.ButtonGroup {
                buttons: dayGrid.children
            }
            PathView {
                id: pathView

                Layout.fillWidth: true
                Layout.fillHeight: true
                flickDeceleration: Kirigami.Units.longDuration
                preferredHighlightBegin: 0.5
                preferredHighlightEnd: 0.5
                snapMode: PathView.SnapToItem
                focus: true
                interactive: true//Kirigami.Settings.tabletMode
                clip: true

                path: Path {
                    startX: - pathView.width * pathView.count / 2 + pathView.width / 2
                    startY: pathView.height / 2
                    PathLine {
                        x: pathView.width * pathView.count / 2 + pathView.width / 2
                        y: pathView.height / 2
                    }
                }

                model: Kalendar.InfiniteCalendarViewModel {
                    scale: Kalendar.InfiniteCalendarViewModel.MonthScale
                }

                property int startIndex
                Component.onCompleted: {
                    startIndex = count / 2;
                    currentIndex = startIndex;
                }
                onCurrentIndexChanged: {
                    datepicker.month = currentItem.model.month;
                    datepicker.year = currentItem.model.year;

                    if(currentIndex >= count - 2) {
                        model.addDates(true);
                    } else if (currentIndex <= 1) {
                        model.addDates(false);
                        startIndex += model.datesToAdd;
                    }
                }

                delegate: GridLayout {
                    id: dayGrid
                    columns: 7
                    rows: 6
                    width: pathView.width
                    height: pathView.height
                    Layout.topMargin: Kirigami.Units.smallSpacing
                    //visible: datepicker.showDays

                    property var model: Kalendar.MonthModel {
                        year: firstDay.getFullYear()
                        month: firstDay.getMonth() + 1 // From pathview model
                    }

                    Repeater {
                        model: dayGrid.model.weekDays
                        delegate: QQC2.Label {
                            Layout.fillWidth: true
                            height: dayGrid / dayGrid.rows
                            horizontalAlignment: Text.AlignHCenter
                            opacity: 0.7
                            text: modelData
                        }
                    }

                    Repeater {
                        model: dayGrid.model

                        delegate: QQC2.Button {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            flat: true
                            highlighted: model.isToday
                            checkable: true
                            checked: date.getDate() === clickedDate.getDate() &&
                                date.getMonth() === clickedDate.getMonth() &&
                                date.getFullYear() === clickedDate.getFullYear()
                            opacity: sameMonth ? 1 : 0.7
                            text: model.dayNumber
                            onClicked: datePicked(model.date), clickedDate = model.date

                            DragHandler {
                                target: pathView
                            }
                        }
                    }
                }
            }

            GridLayout {
                id: monthGrid
                columns: 3
                rows: 4
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.smallSpacing

                Repeater {
                    model: monthGrid.columns * monthGrid.rows
                    delegate: QQC2.Button {
                        property int monthToUse: index
                        property date date: new Date(year, monthToUse)
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        flat: true
                        text: Qt.locale().monthName(date.getMonth())
                        onClicked: {
                            selectedDate = new Date(date);
                            datepicker.datePicked(date);
                            if(datepicker.showDays) pickerView.currentIndex = 0;
                        }
                    }
                }
            }

            GridLayout {
                id: yearGrid
                columns: 3
                rows: 4
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.smallSpacing

                Repeater {
                    model: yearGrid.columns * yearGrid.rows
                    delegate: QQC2.Button {
                        property int yearToUse: index - 1 + (Math.floor(year/10)*10) // Display a decade, e.g. 2019 - 2030
                        property date date: new Date(yearToUse, 0)
                        property bool sameDecade: Math.floor(yearToUse / 10) == Math.floor(year / 10)
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        flat: true
                        opacity: sameDecade ? 1 : 0.7
                        text: date.getFullYear()
                        onClicked: {
                            selectedDate = new Date(date);
                            datepicker.datePicked(date);
                            pickerView.currentIndex = 1;
                        }
                    }
                }
            }
        }
    }
}




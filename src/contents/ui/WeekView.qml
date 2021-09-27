// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils
import "labelutils.js" as LabelUtils

Kirigami.Page {
    id: root

    signal addIncidence(int type, date addDate)
    signal viewIncidence(var modelData, var collectionData)
    signal editIncidence(var incidencePtr, var collectionId)
    signal deleteIncidence(var incidencePtr, date deleteDate)
    signal completeTodo(var incidencePtr)
    signal addSubTodo(var parentWrapper)

    property var openOccurrence
    property date selectedDate: new Date()
    property date startDate: DateUtils.getFirstDayOfMonth(selectedDate)
    property int day: selectedDate.getDate()
    property int month: selectedDate.getMonth()
    property int year: selectedDate.getFullYear()
    property bool initialMonth: true
    readonly property bool isLarge: width > Kirigami.Units.gridUnit * 30
    readonly property bool isDark: LabelUtils.isDarkColor(Kirigami.Theme.backgroundColor)

    property real dayWidth: root.width / 7

    background: Rectangle {
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
    }

    /*function setToDate(date, isInitialMonth = false) {
        root.initialMonth = isInitialMonth;
        let monthDiff = date.getMonth() - pathView.currentItem.firstDayOfMonth.getMonth() + (12 * (date.getFullYear() - pathView.currentItem.firstDayOfMonth.getFullYear()))
        let newIndex = pathView.currentIndex + monthDiff;

        let firstItemDate = pathView.model.data(pathView.model.index(1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);
        let lastItemDate = pathView.model.data(pathView.model.index(pathView.model.rowCount() - 1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);

        while(firstItemDate >= date) {
            pathView.model.addDates(false)
            firstItemDate = pathView.model.data(pathView.model.index(1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);
            newIndex = 0;
        }
        if(firstItemDate < date && newIndex === 0) {
            newIndex = date.getMonth() - firstItemDate.getMonth() + (12 * (date.getFullYear() - firstItemDate.getFullYear())) + 1;
        }

        while(lastItemDate <= date) {
            pathView.model.addDates(true)
            lastItemDate = pathView.model.data(pathView.model.index(pathView.model.rowCount() - 1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);
        }
        pathView.currentIndex = newIndex;
        selectedDate = date;
    }*/

    actions {
        left: Kirigami.Action {
            icon.name: "go-previous"
            text: i18n("Previous Week")
            shortcut: "Left"
            //onTriggered: setToDate(DateUtils.addMonthsToDate(pathView.currentItem.firstDayOfMonth, -1))
            displayHint: Kirigami.DisplayHint.IconOnly
        }
        right: Kirigami.Action {
            icon.name: "go-next"
            text: i18n("Next Week")
            shortcut: "Right"
            //onTriggered: setToDate(DateUtils.addMonthsToDate(pathView.currentItem.firstDayOfMonth, 1))
            displayHint: Kirigami.DisplayHint.IconOnly
        }
        main: Kirigami.Action {
            icon.name: "go-jump-today"
            text: i18n("Today")
            onTriggered: setToDate(new Date())
        }
    }

    padding: 0
    bottomPadding: Kirigami.Settings.isMobile ? Kirigami.Units.largeSpacing * 2 : Kirigami.Units.largeSpacing

    PathView {
        id: pathView

        anchors.fill: parent
        flickDeceleration: Kirigami.Units.longDuration
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        snapMode: PathView.SnapToItem
        focus: true
        interactive: true //Kirigami.Settings.tabletMode

        path: Path {
            startX: - pathView.width * pathView.count / 2 + pathView.width / 2
            startY: pathView.height / 2
            PathLine {
                x: pathView.width * pathView.count / 2 + pathView.width / 2
                y: pathView.height / 2
            }
        }

        model: Kalendar.WeekViewModel {}

        property date dateToUse
        property int startIndex
        Component.onCompleted: {
            startIndex = count / 2;
            currentIndex = startIndex;
        }
        onCurrentIndexChanged: {
            root.startDate = currentItem.startDate;
            root.month = currentItem.month;
            root.year = currentItem.year;
            //root.initialMonth = false;

            if(currentIndex >= count - 2) {
                model.addDates(true);
            } else if (currentIndex <= 1) {
                model.addDates(false);
                startIndex += model.weeksToAdd;
            }
        }

        delegate: Loader {
            id: viewLoader

            property date startDate: model.startDate
            property int month: model.selectedMonth - 1 // Convert QDateTime month to JS month
            property int year: model.selectedYear

            property int index: model.index
            property bool isCurrentItem: PathView.isCurrentItem
            property bool isNextOrCurrentItem: index >= pathView.currentIndex -1 && index <= pathView.currentIndex + 1

            active: isNextOrCurrentItem
            //asynchronous: true
            sourceComponent: QQC2.ScrollView {
                width: pathView.width
                height: pathView.height
                contentWidth: availableWidth
                QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

                Row {
                    anchors.fill: parent

                    Repeater {
                        id: daysList

                        property int periodsPerHour: 60 / hourlyModel.periodLength
                        property int daySections: (60 * 24) / hourlyModel.periodLength
                        property real dayHeight: daySections * Kirigami.Units.gridUnit

                        model: Kalendar.HourlyIncidenceModel {
                            id: hourlyModel
                            model: Kalendar.IncidenceOccurrenceModel {
                                id: occurrenceModel
                                objectName: "incidenceOccurrenceModel"
                                start: viewLoader.startDate
                                length: 7
                                filter: root.filter ? root.filter : {}
                                calendar: Kalendar.CalendarManager.calendar
                            }
                        }

                        delegate: Item {
                            id: dayColumn
                            property int index: model.index
                            width: dayWidth
                            height: daysList.dayHeight

                            Repeater {
                                model: daysList.daySections
                                delegate: Kirigami.Separator {
                                    width: root.dayWidth
                                    y: index * Kirigami.Units.gridUnit
                                    height: 1
                                    visible: index % daysList.periodsPerHour === 0
                                }
                            }
                            /*Kirigami.Heading {
                                width: parent.width
                                text: DateUtils.addDaysToDate(viewLoader.startDate, index).toLocaleDateString(Qt.locale())
                                wrapMode: Text.Wrap
                            }*/

                            Repeater {
                                id: incidencesRepeater
                                model: incidences
                                delegate: Rectangle {
                                    y: modelData.starts * Kirigami.Units.gridUnit
                                    width: root.dayWidth //* modelData.widthShare
                                    height: modelData.duration * Kirigami.Units.gridUnit
                                    color: Qt.rgba(0,0,0,0)
                                    property int rectRadius: 5
                                    Component.onCompleted: console.log(modelData.starts, modelData.widthShare, modelData.duration)

                                    property bool isOpenOccurrence: root.openOccurrence ?
                                        root.openOccurrence.incidenceId === modelData.incidenceId : false

                                    Rectangle {
                                        id: incidenceBackground
                                        anchors.fill: parent
                                        color: isOpenOccurrence ? modelData.color :
                                            LabelUtils.getIncidenceBackgroundColor(modelData.color, root.isDark)
                                        radius: parent.rectRadius
                                    }

                                    RowLayout {
                                        id: incidenceContents

                                        property color textColor: LabelUtils.getIncidenceLabelColor(modelData.color, root.isDark)

                                        function otherMonthTextColor(color) {
                                            if(root.isDark) {
                                                if(LabelUtils.getDarkness(color) >= 0.5) {
                                                    return Qt.lighter(color, 2);
                                                }
                                                return Qt.lighter(color, 1.5);
                                            }
                                            return Qt.darker(color, 3);
                                        }

                                        anchors {
                                            fill: parent
                                            leftMargin: Kirigami.Units.smallSpacing
                                            rightMargin: Kirigami.Units.smallSpacing
                                        }

                                        Kirigami.Icon {
                                            Layout.maximumHeight: parent.height
                                            Layout.maximumWidth: height

                                            source: modelData.incidenceTypeIcon
                                            isMask: true
                                            color: isOpenOccurrence ? (LabelUtils.isDarkColor(modelData.color) ? "white" : "black") :
                                                incidenceBackground.visible ? incidenceContents.textColor :
                                                incidenceContents.otherMonthTextColor(modelData.color)
                                        }

                                        QQC2.Label {
                                            Layout.fillWidth: true
                                            text: modelData.text
                                            elide: Text.ElideRight
                                            font.weight: Font.Medium
                                            color: isOpenOccurrence ? (LabelUtils.isDarkColor(modelData.color) ? "white" : "black") :
                                                incidenceBackground.visible ? incidenceContents.textColor :
                                                incidenceContents.otherMonthTextColor(modelData.color)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// Copyright (C) 2018 Michael Bohlender, <bohlender@kolabsys.com>
// Copyright (C) 2018 Christian Mollekopf, <mollekopf@kolabsys.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils

Item {
    id: root
    property int daysToShow
    property int daysPerRow: daysToShow
    property double weekHeaderWidth: 0
    property double dayWidth: (width - weekHeaderWidth) / daysPerRow
    property date currentDate
    property date startDate
    property var calendarFilter
    property bool paintGrid: false
    property bool showDayIndicator: false
    property var filter
    property alias dayHeaderDelegate: dayLabels.delegate
    property Component weekHeaderDelegate
    property int month

    //Internal
    property int numberOfLinesShown: 0
    property int numberOfRows: (daysToShow / daysPerRow)
    property var dayHeight: (height - dayLabels.height) / numberOfRows

    implicitHeight: (numberOfRows > 1 ? Kirigami.Units.gridUnit * 10 * numberOfRows: numberOfLinesShown * Kirigami.Units.gridUnit) + dayLabels.height

    height: implicitHeight

    Column {
        anchors {
            fill: parent
        }

        DayLabels {
            id: dayLabels
            startDate: root.startDate
            dayWidth: root.dayWidth
            daysToShow: root.daysPerRow
        }

        //Weeks
        Repeater {
            model: Kalendar.MultiDayEventModel {
                model: Kalendar.EventOccurrenceModel {
                    objectName: "eventOccurrenceModel"
                    start: root.startDate
                    length: root.daysToShow
                    filter: root.filter ? root.filter : {}
                    calendar: Kalendar.CalendarManager.calendar
                }
                // daysPerRow: root.daysPerRow //Hardcoded to 7
            }
            //One row => one week
            Item {
                width: parent.width
                height: root.dayHeight
                clip: true
                Row {
                    width: parent.width
                    height: parent.height
                    Loader {
                        id: weekHeader
                        height: parent.height
                        sourceComponent: root.weekHeaderDelegate
                        property var startDate: weekStartDate
                        onStatusChanged: if (weekHeader.status == Loader.Ready) root.weekHeaderWidth = item.width
                    }
                    Item {
                        id: dayDelegate
                        height: root.dayHeight
                        width: parent.width - weekHeader.width
                        property var startDate: weekStartDate
                        //Grid
                        Row {
                            height: parent.height
                            Repeater {
                                id: gridRepeater
                                model: root.daysPerRow
                                Item {
                                    id: gridItem
                                    height: parent.height
                                    width: root.dayWidth
                                    property var date: DateUtils.addDaysToDate(dayDelegate.startDate, modelData)
                                    property bool isInPast: DateUtils.roundToDay(date) < DateUtils.roundToDay(root.currentDate)
                                    property bool isToday: DateUtils.sameDay(root.currentDate, date)
                                    property bool isCurrentMonth: date.getMonth() == root.month

                                    //Grid
                                    Rectangle {
                                        anchors.fill: parent
                                        visible: root.paintGrid
                                        color: Kirigami.Theme.backgroundColor
                                        Kirigami.Theme.colorSet: Kirigami.Theme.View
                                        border.width: 1
                                        border.color: Kirigami.Theme.disabledTextColor

                                        //Dimm days in the past
                                        Rectangle {
                                            anchors.fill: parent
                                            anchors.margins: 1
                                            color: Kirigami.Theme.backgroundColor
                                            Kirigami.Theme.colorSet: Kirigami.Theme.Button
                                            opacity: 0.2
                                            visible: gridItem.isInPast
                                        }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        //onClicked: Kube.Fabric.postMessage(Kube.Messages.eventEditor, {"start": date, "allDay": true})
                                    }

                                    //Day number
                                    QQC2.Label {
                                        visible: root.showDayIndicator
                                        anchors {
                                            top: parent.top
                                            left: parent.left
                                            topMargin: Kirigami.Units.smallSpacing
                                            leftMargin: Kirigami.Units.smallSpacing
                                        }
                                        //We add the month abbrevation to the first of each month
                                        text: gridItem.date.toLocaleDateString(Qt.locale(), gridItem.date.getDate() == 1 ? "d MMM" : "d")
                                        font.bold: true
                                        color: gridItem.isToday ? Kirigami.Theme.highlightColor : (!gridItem.isCurrentMonth ? Kirigami.Theme.disabledTextColor : Kirigami.Theme.textColor)
                                    }
                                }
                            }
                        }

                        Column {
                            anchors {
                                fill: parent
                                //Offset for date
                                topMargin: root.showDayIndicator ? Kirigami.Units.gridUnit + Kirigami.Units.smallSpacing : 0
                            }
                            Repeater {
                                id: linesRepeater
                                model: events
                                onCountChanged: {
                                    root.numberOfLinesShown = count
                                }
                                Item {
                                    id: line
                                    height: Kirigami.Units.gridUnit
                                    width: parent.width

                                    //Events
                                    Repeater {
                                        id: eventsRepeater
                                        model: modelData
                                        Rectangle {
                                            x: root.dayWidth * modelData.starts
                                            y: 0
                                            width: root.dayWidth * modelData.duration
                                            height: parent.height

                                            radius: 2

                                            Rectangle {
                                                anchors.fill: parent
                                                color: modelData.color
                                                radius: 2
                                                border.width: 1
                                                border.color: Kirigami.Theme.alternateBackgroundColor
                                                opacity: 0.6
                                            }

                                            QQC2.Label {
                                                anchors {
                                                    fill: parent
                                                    leftMargin: Kirigami.Units.smallSpacing
                                                    rightMargin: Kirigami.Units.smallSpacing
                                                }
                                                text: modelData.text
                                                elide: Text.ElideRight
                                            }

                                            MouseArea {
                                                id: mouseArea
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                onClicked: eventDetails.createObject(root, {}).open()
                                                Component {
                                                    id: eventDetails
                                                    QQC2.Popup {
                                                        id: popup
                                                        parent: ApplicationWindow.overlay
                                                        x: Math.round((parent.width - width) / 2)
                                                        y: Math.round((parent.height - height) / 2)
                                                        width: eventView.width
                                                        height: eventView.height
                                                        padding: 0
                                                        /*EventView {
                                                            id: eventView
                                                            controller: Kube.EventController {
                                                                eventOccurrence: model.modelData.eventOccurrence
                                                            }
                                                            onDone: popup.close()
                                                        }*/
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
        }
    }
}

// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils

Kirigami.ScrollablePage {
    id: root

    signal addEvent
    signal viewEvent(var modelData, var collectionData)
    signal editEvent(var eventPtr, var collectionId)
    signal deleteEvent(var eventPtr, date deleteDate)

    property date currentDate: new Date()
    property date startDate: DateUtils.getFirstDayOfMonth(currentDate)
    property int month: startDate.getMonth()
    property int daysInMonth: new Date(currentDate.getFullYear(), currentDate.getMonth(), 0).getDate()
    readonly property bool isLarge: width > Kirigami.Units.gridUnit * 30

    background: Rectangle {
        Kirigami.Theme.colorSet: root.isLarge ? Kirigami.Theme.Header : Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
    }

    actions {
        left: Kirigami.Action {
            text: i18n("Previous")
            onTriggered: startDate = DateUtils.previousMonth(startDate)
        }
        right: Kirigami.Action {
            text: i18n("Next")
            onTriggered: startDate = DateUtils.nextMonth(startDate)
        }
    }

    padding: 0

    ListView {
        id: scheduleListView

        property var dateIndexes: []

        anchors.fill: parent

        headerPositioning: ListView.OverlayHeader
        header: Kirigami.ItemViewHeader {
            //backgroundImage.source: "../banner.jpg"
            title: Qt.locale().monthName(root.month)
        }

        model: Kalendar.DailyEventsModel {
            model: Kalendar.EventOccurrenceModel {
                id: occurrenceModel
                objectName: "eventOccurrenceModel"
                start: root.startDate
                length: root.daysInMonth
                filter: root.filter ? root.filter : {}
                calendar: Kalendar.CalendarManager.calendar
            }
        }

        delegate: ColumnLayout {
            id: dayColumn

            width: scheduleListView.width

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true

                text: {
                    let dayName = Qt.locale().dayName(date.getDay());
                    let dayDate = date.getDate();
                    let dayMonth = Qt.locale().monthName(date.getMonth())

                    let nextDay = DateUtils.getLastDayOfWeek( DateUtils.nextWeek(date) );
                    if (nextDay.getMonth() !== date.getMonth()) {
                        nextDay = new Date(date.getFullYear(), date.getMonth(), 0);
                    }

                    let nextDayName = Qt.locale().dayName(nextDay.getDay());
                    let nextDayDate = nextDay.getDate();
                    return `${dayName} ${dayDate} - ${nextDayName} ${nextDayDate} ${dayMonth}`
                }
                visible: date.getDay() == Qt.locale().firstDayOfWeek || index == 0
            }

            // Day + events
            GridLayout {
                id: dayGrid
                visible: events.length || new Date(date).setHours(0,0,0,0) == new Date().setHours(0,0,0,0)

                columns: 2
                rows: 2

                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing

                Kirigami.Separator {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                }

                Kirigami.Heading {
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 3
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 3
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.fillHeight: true

                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignTop

                    text: Qt.locale().dayName(date.getDay(), Locale.NarrowFormat) + ", \n" + date.getDate()
                    visible: events.length || new Date(date).setHours(0,0,0,0) == new Date().setHours(0,0,0,0)
                }

                ColumnLayout {
                    Layout.fillWidth: true

                    Kirigami.AbstractCard {
                        Layout.fillWidth: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Units.largeSpacing

                        showClickFeedback: true

                        visible: !events.length && new Date(date).setHours(0,0,0,0) == new Date().setHours(0,0,0,0)

                        contentItem: QQC2.Label {
                            text: Kirigami.Settings.isMobile ?  i18n("Nothing on the books today. Tap to add something.") :
                                                                i18n("Nothing on the books today. Click to add something.")
                        }

                        onClicked: root.addEvent()
                    }

                    Repeater {
                        model: events
                        Repeater {
                            id: eventsRepeater
                            model: modelData

                            Kirigami.AbstractCard {
                                id: eventCard

                                Layout.fillWidth: true

                                Kirigami.Theme.inherit: false
                                // NOTE: regardless of the color set used, it is recommended to replace
                                // all available colors in Theme, to avoid badly contrasting colors
                                Kirigami.Theme.colorSet: Kirigami.Theme.View
                                Kirigami.Theme.backgroundColor: Qt.rgba(modelData.color.r, modelData.color.g, modelData.color.b, 0.8)
                                Kirigami.Theme.highlightColor: Qt.darker(modelData.color, 2.5)

                                showClickFeedback: true

                                function isDarkColor(background) {
                                    var temp = Qt.darker(background, 1)
                                    var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
                                    return temp.a > 0 && a >= 0.5
                                }

                                contentItem: GridLayout {
                                    columns: root.isLarge ? 2 : 1
                                    rows: root.isLarge ? 1 : 2

                                    QQC2.Label {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        Layout.column: 0
                                        Layout.row: 0

                                        color: eventCard.isDarkColor(modelData.color) ? "white" : "black"
                                        text: modelData.text
                                        elide: Text.ElideRight
                                    }

                                    QQC2.Label {
                                        Layout.fillHeight: true
                                        Layout.column: root.isLarge ? 1 : 0
                                        Layout.row: root.isLarge ? 0 : 1
                                        visible: !modelData.allDay

                                        color: eventCard.isDarkColor(modelData.color) ? "white" : "black"
                                        text: {
                                            if(modelData.startTime.toTimeString() != modelData.endTime.toTimeString()) {
                                                modelData.startTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat) + " - " + modelData.endTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                                            } else if (modelData.startTime.toTimeString() == modelData.endTime.toTimeString()) {
                                                modelData.startTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                                            }
                                        }
                                    }
                                }

                                IncidenceMouseArea {
                                    id: eventMouseArea

                                    eventData: modelData
                                    collectionDetails: events.length && Kalendar.CalendarManager.getCollectionDetails(modelData.collectionId)

                                    onViewClicked: root.viewEvent(modelData, collectionData)
                                    onEditClicked: root.editEvent(eventPtr, collectionId)
                                    onDeleteClicked: root.deleteEvent(eventPtr, deleteDate)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

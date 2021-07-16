// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
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
    property int year: startDate.getFullYear()
    property int daysInMonth: new Date(currentDate.getFullYear(), currentDate.getMonth(), 0).getDate()
    readonly property bool isLarge: width > Kirigami.Units.gridUnit * 30

    function setToDate(date) {
        currentDate = date

        startDate = DateUtils.getFirstDayOfMonth(date);
        month = startDate.getMonth();
    }

    background: Rectangle {
        Kirigami.Theme.colorSet: root.isLarge ? Kirigami.Theme.Header : Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
    }

    actions {
        left: Kirigami.Action {
            text: i18n("Previous")
            onTriggered: setToDate(DateUtils.previousMonth(startDate))
        }
        right: Kirigami.Action {
            text: i18n("Next")
            onTriggered: setToDate(DateUtils.nextMonth(startDate))
        }
    }

    padding: 0

    ListView {
        id: scheduleListView

        highlightRangeMode: ListView.ApplyRange
        onCountChanged: positionViewAtIndex(root.currentDate.getDate() - 1, ListView.Beginning);

        header: Kirigami.ItemViewHeader {
            //backgroundImage.source: "../banner.jpg"
            title: Qt.locale().monthName(root.month)
        }

        model: Kalendar.MultiDayEventModel {
            periodLength: 1

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
            // Tip: do NOT hide an entire delegate.
            // This will very much screw up use of positionViewAtIndex.

            id: dayColumn

            width: scheduleListView.width

            // Independent from the event daygrid
            ColumnLayout {
                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing

                Kirigami.Separator {
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                }

                Kirigami.Heading {
                    id: weekHeading
                    Layout.fillWidth: true

                    level: 2
                    text: {
                        let nextDay = DateUtils.getLastDayOfWeek( DateUtils.nextWeek(periodStartDate) );
                        if (nextDay.getMonth() !== periodStartDate.getMonth()) {
                            nextDay = new Date(nextDay.getFullYear(), nextDay.getMonth(), 0);
                        }

                        return periodStartDate.toLocaleDateString(Qt.locale(), "dddd dd") + " - " + nextDay.toLocaleDateString(Qt.locale(), "dddd dd MMMM");
                    }
                    visible: periodStartDate !== undefined &&
                        (periodStartDate.getDay() == Qt.locale().firstDayOfWeek || index == 0)
                }
                QQC2.Label {
                    visible: !events.length
                    text: periodStartDate.toLocaleDateString(Qt.locale(), "ddd dd")
                }
            }

            // Day + events
            GridLayout {
                id: dayGrid

                columns: 2
                rows: 2

                Layout.leftMargin: Kirigami.Units.largeSpacing
                Layout.rightMargin: Kirigami.Units.largeSpacing
                Layout.bottomMargin: Kirigami.Units.largeSpacing

                property bool isToday: new Date(periodStartDate).setHours(0,0,0,0) == new Date().setHours(0,0,0,0)
                visible: events.length || isToday

                Kirigami.Heading {
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 3
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 3
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.fillHeight: true

                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignTop

                    level: dayGrid.isToday ? 2 : 3

                    text: periodStartDate.toLocaleDateString(Qt.locale(), "ddd\ndd")
                    visible: events.length || dayGrid.isToday
                }

                ColumnLayout {
                    Layout.fillWidth: true

                    Kirigami.AbstractCard {
                        id: suggestCard

                        Layout.fillWidth: true

                        showClickFeedback: true
                        visible: !events.length && dayGrid.isToday

                        contentItem: QQC2.Label {
                            property string selectMethod: Kirigami.Settings.isMobile ? i18n("Tap") : i18n("Click")
                            text: i18n("Nothing on the books today. %1 to add something.", selectMethod)
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

                                property bool multiday: modelData.startTime.getDate() !== modelData.endTime.getDate()
                                property int eventDays: DateUtils.fullDaysBetweenDates(modelData.startTime, modelData.endTime)
                                property int dayOfMultidayEvent: DateUtils.fullDaysBetweenDates(modelData.startTime, periodStartDate)

                                function isDarkColor(background) {
                                    var temp = Qt.darker(background, 1);
                                    var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
                                    return temp.a > 0 && a >= 0.5;
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
                                        text: {
                                            if(eventCard.multiday) {
                                                return i18n("%1 (Day %2 of %3)", modelData.text, eventCard.dayOfMultidayEvent, eventCard.eventDays);
                                            } else {
                                                return modelData.text;
                                            }
                                        }
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

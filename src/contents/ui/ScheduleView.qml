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

    ListView {
        id: scheduleListView

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
            anchors.left: parent.left
            anchors.right: parent.right

            Kirigami.ListSectionHeader {
                Layout.fillWidth: true

                property date listDate: date
                text: listDate.toLocaleDateString(Qt.locale())
                visible: events.length
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.bottomMargin: Kirigami.Units.largeSpacing

                visible: events.length

                Repeater {
                    model: events
                    Repeater {
                        id: eventsRepeater
                        model: modelData

                        Kirigami.AbstractCard {
                            id: eventCard

                            Layout.fillWidth: true
                            Layout.leftMargin: Kirigami.Units.largeSpacing
                            Layout.rightMargin: Kirigami.Units.largeSpacing

                            showClickFeedback: true

                            contentItem: QQC2.Label {
                                function isDarkColor(background) {
                                    var temp = Qt.darker(background, 1)
                                    var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
                                    return temp.a > 0 && a >= 0.5
                                }

                                color: isDarkColor(modelData.color) ? "white" : "black"
                                text: modelData.text
                            }

                            background: Rectangle {
                                anchors.fill: parent
                                color: modelData.color
                                radius: 2
                                border.width: 1
                                border.color: Kirigami.Theme.alternateBackgroundColor
                                opacity: eventMouseArea.pressed ? 0.8 : 0.6
                            }

                            IncidenceMouseArea {
                                id: eventMouseArea

                                eventData: modelData
                                collectionDetails: Kalendar.CalendarManager.getCollectionDetails(modelData.collectionId)

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

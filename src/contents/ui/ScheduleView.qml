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

    property date currentDate: new Date()
    property date startDate: DateUtils.getFirstDayOfMonth(currentDate)
    property int month: startDate.getMonth()
    property int daysInMonth: new Date(currentDate.getFullYear(), currentDate.getMonth(), 0).getDate()

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

                delegate: Column {
                    id: dayColumn
                    // Header

                    Kirigami.ListSectionHeader {
                        property date listDate: date
                        text: listDate.toLocaleDateString(Qt.locale())
                        visible: events.length
                    }

                    ColumnLayout {
                        Repeater {
                            model: events
                            Repeater {
                                id: eventsRepeater
                                model: modelData

                                QQC2.Label {
                                    text: modelData.text
                                }
                            }
                        }
                    }
                }
            }


}

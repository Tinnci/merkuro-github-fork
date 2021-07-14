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
    property int month: currentDate.getMonth()
    property int daysInMonth: new Date(currentDate.getFullYear(), currentDate.getMonth(), 0).getDate()

    actions {
        left: Kirigami.Action {
            text: i18n("Previous")
            onTriggered: {
                month - 1 < 0 ? month = 11 : month -= 1
                let newDate = DateUtils.getFirstDayOfWeek(DateUtils.previousMonth(startDate))

                // Handling adding and subtracting months in Javascript can get *really* messy.
                newDate = DateUtils.addDaysToDate(newDate, 7)

                if (newDate.getMonth() === month) {
                    newDate = DateUtils.addDaysToDate(newDate, - 7)
                }
                if (newDate.getDate() < 14) {
                    newDate = DateUtils.addDaysToDate(newDate, - 7)
                }

                console.log(month)
                startDate = newDate
            }
        }
        right: Kirigami.Action {
            text: i18n("Next")
            onTriggered: {
                month = (month + 1) % 12
                let newDate = DateUtils.getFirstDayOfWeek(DateUtils.nextMonth(startDate))
                newDate = DateUtils.addDaysToDate(newDate, 7)

                if (newDate.getMonth() === month) {
                    newDate = DateUtils.addDaysToDate(newDate, - 7)
                }
                if (newDate.getDate() < 14) {
                    newDate = DateUtils.addDaysToDate(newDate, - 7)
                }

                startDate = newDate
            }
        }
    }

    ListView {
        Column {
            Repeater {
                model: Kalendar.MultiDayEventModel {
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
                    // Header

                    Repeater {
                        model: events
                        Repeater {
                            model: modelData
                            QQC2.Label {
                                Component.onCompleted: console.log(Object.keys(modelData))
                                text: modelData.text
                            }
                        }
                    }
                }
            }
        }
    }

}

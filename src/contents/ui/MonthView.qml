// Copyright (C) 2018 Michael Bohlender, <bohlender@kolabsys.com>
// Copyright (C) 2018 Christian Mollekopf, <mollekopf@kolabsys.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import "dateutils.js" as DateUtils

FocusScope {
    id: root

    property alias startDate: dayView.startDate
    property alias currentDate: dayView.currentDate
    property alias calendarFilter: dayView.calendarFilter
    property alias month: dayView.month

    MultiDayView {
        id: dayView
        objectName: "monthView"
        anchors.fill: parent
        daysToShow: daysPerRow * 6
        daysPerRow: 7
        paintGrid: true
        showDayIndicator: true
        dayHeaderDelegate: Item {
            height: Kirigami.Units.gridUnit + Kirigami.Units.smallSpacing * 3
            Column {
                anchors.centerIn: parent
                QQC2.Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.bold: true
                    text: day.toLocaleString(Qt.locale(), "dddd")
                }
            }
        }
        weekHeaderDelegate: Item {
            width: Kirigami.Units.gridUnit
            QQC2.Label {
                anchors.centerIn: parent
                font.bold: true
                text: DateUtils.getWeek(startDate, Qt.locale().firstDayOfWeek)
                color: Kirigami.Theme.disabledTextColor
            }
        }
    }
}


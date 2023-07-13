// SPDX-FileCopyrightText: 2022 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami
import org.kde.merkuro.calendar 1.0 as Merkuro

DayGridView {
    id: dayView

    readonly property bool isLarge: width > Kirigami.Units.gridUnit * 40
    readonly property bool isTiny: width < Kirigami.Units.gridUnit * 18

    objectName: "monthView"

    dayHeaderDelegate: QQC2.Control {
        Layout.maximumHeight: Kirigami.Units.gridUnit * 2
        contentItem: Kirigami.Heading {
            text: {
                const longText = day.toLocaleString(Qt.locale(), "dddd");
                const midText = day.toLocaleString(Qt.locale(), "ddd");
                const shortText = midText.slice(0,1);
                switch(Merkuro.Config.weekdayLabelLength) {
                    case Merkuro.Config.Full:
                        let chosenFormat = "dddd"
                        return dayView.isLarge ? longText : dayView.isTiny ? shortText : midText;
                    case Merkuro.Config.Abbreviated:
                        return dayView.isTiny ? shortText : midText;
                    case Merkuro.Config.Letter:
                    default:
                        return shortText;
                }
            }
            level: 2
            leftPadding: Kirigami.Units.smallSpacing
            rightPadding: Kirigami.Units.smallSpacing
            horizontalAlignment: {
                switch(Merkuro.Config.weekdayLabelAlignment) {
                    case Merkuro.Config.Left:
                        return Text.AlignLeft;
                    case Merkuro.Config.Center:
                        return Text.AlignHCenter;
                    case Merkuro.Config.Right:
                        return Text.AlignRight;
                    default:
                        return Text.AlignHCenter;
                }
            }
        }
    }

    weekHeaderDelegate: QQC2.Label {
        padding: Kirigami.Units.smallSpacing
        verticalAlignment: Qt.AlignTop
        horizontalAlignment: Qt.AlignHCenter
        text: Merkuro.Utils.weekNumber(startDate)
        background: Rectangle {
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            color: Kirigami.Theme.backgroundColor
        }
    }
}

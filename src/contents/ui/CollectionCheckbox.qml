// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kalendar 1.0

QQC2.CheckBox {
    id: calendarCheckbox

    indicator: Rectangle {
        height: parent.height * 0.8
        width: height
        x: calendarCheckbox.leftPadding
        y: parent.height / 2 - height / 2
        radius: 3
        border.color: model.collectionColor
        color: Qt.rgba(0,0,0,0)

        Rectangle {
            anchors.margins: parent.height * 0.2
            anchors.fill: parent
            radius: 1
            color: model.collectionColor
            visible: model.checkState === 2
        }
    }
    checked: model.checkState === 2
    onClicked: model.checkState = model.checkState === 0 ? 2 : 0
}


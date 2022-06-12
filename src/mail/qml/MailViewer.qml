// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import org.kde.kalendar 1.0
import org.kde.kalendar.mail 1.0
import org.kde.kitemmodels 1.0 as KItemModels
import './mailpartview'

QQC2.Page {
    id: root
    property var item
    property string subject
    property date dateTime

    Layout.fillWidth: true
    header: QQC2.ToolBar {
        contentItem: QQC2.Label {
            text: subject
        }
    }
    contentItem: MailPartView {
        item: root.item
    }
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        property color borderColor: Kirigami.Theme.textColor
        border.color: Qt.rgba(borderColor.r, borderColor.g, borderColor.b, 0.3)
        radius: 4
    }
}

// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kalendar 1.0

Kirigami.Page {
    id: deleteSheet

    signal changeAll
    signal changeThis
    signal changeThisAndFuture
    signal cancel

    // For incidence deletion
    property var incidenceWrapper
    property bool isMove: false
    property int startOffset: 0
    property int endOffset: 0
    property date occurrenceDate
    property Item caughtDelegate

    padding: Kirigami.Units.largeSpacing

    title: i18n("Change incidence date")

    QQC2.Action {
        id: changeAction
        enabled: incidenceWrapper !== undefined
        shortcut: "Return"
        onTriggered: changeThis()
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            QQC2.Label {
                Layout.fillWidth: true
                text: i18n("The item you are trying to change is a recurring item. Should the changes be applied only to this single occurrence, also to future items, or to all items in the recurrence?")
                wrapMode: Text.WordWrap
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                icon.name: "deletecell"
                text: i18n("Only This Item")
                onClicked: changeThis()
            }

            QQC2.Button {
                icon.name: "edit-table-delete-row"
                text: i18n("Also Future Items")
                onClicked: changeThisAndFuture()
            }

            QQC2.Button {
                icon.name: "group-delete"
                text: i18n("All Occurrences")
                onClicked: changeAll()
            }

            QQC2.Button {
                icon.name: "dialog-cancel"
                text: i18n("Cancel")
                onClicked: cancel()
            }
        }
    }
}


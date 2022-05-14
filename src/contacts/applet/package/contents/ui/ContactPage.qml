// SPDX-FileCopyrightText: 2022 Carl Schwan <car@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick 2.15
import QtQml 2.15
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.kalendar.contact 1.0

PlasmaComponents3.ScrollView {
    id: root

    property int itemId
    topPadding: 0
    leftPadding: PlasmaCore.Units.smallSpacing
    rightPadding: PlasmaCore.Units.smallSpacing

    property AddresseeWrapper addressee: AddresseeWrapper {
        addresseeItem: ContactManager.getItem(root.itemId)
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Escape) {
            stack.pop()
            event.accepted = true;
        }
    }

    property var header: PlasmaExtras.PlasmoidHeading {
        RowLayout {
            width: parent.width

            PlasmaComponents3.Button {
                icon.name: "go-previous-view"
                text: i18n("Return to Contact List")
                onClicked: stack.pop()
            }

            PlasmaExtras.Heading {
                text: addressee.name
                level: 2
                Layout.fillWidth: true
            }

            PlasmaComponents3.ToolButton {
                icon.name: 'view-barcode-qr'
                onClicked: stack.push(Qt.resolvedUrl('./QrCodePage.qml'), {
                    qrCodeData: addressee.qrCodeData(),
                })
            }
        }
    }

    ColumnLayout {
        PlasmaComponents3.Label {
            visible: text !== i18n('Birthday:') + ' '
            text: if (addressee.birthday.getFullYear() === 0) {
                return i18n('Birthday:') + ' ' + Qt.formatDate(addressee.birthday, "dd.MM.")
            } else {
                return i18n('Birthday:') + ' ' + addressee.birthday.toLocaleDateString()
            }
        }

        PlasmaExtras.Heading {
            Layout.topMargin: PlasmaCore.Units.smallSpacing
            visible: addressesRepeater.count > 0
            text: i18np("Address", "Addresses", addressesRepeater.count)
            level: 4
        }

        Repeater {
            id: addressesRepeater
            model: addressee.addressesModel
            PlasmaComponents3.Label {
                visible: text !== ""
                text: (model.typeLabel ? i18nc("%1 is the type of the address, e.g. home, work, ...", "%1:", model.typeLabel) : '') + ' ' + model.formattedAddress
            }
        }

        PlasmaExtras.Heading {
            Layout.topMargin: PlasmaCore.Units.smallSpacing
            visible: emailRepeater.count > 0
            text: i18np("Email Address", "Email Addresses", emailRepeater.count)
            level: 4
        }

        Repeater {
            id: emailRepeater
            model: addressee.emailModel
            PlasmaComponents3.Label {
                visible: text !== ""
                text: `${model.type} <a href="mailto:${model.display}">${model.display}</a>`
                onLinkActivated: Qt.openUrlExternally(link)
                MouseArea {
                    id: area
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: Qt.openUrlExternally(`mailto:${model.dispaly}`)
                    onPressed: Qt.openUrlExternally(`mailto:${model.dispaly}`)
                }
            }
        }
    }
}

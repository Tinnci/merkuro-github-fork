/*
 * SPDX-FileCopyrightText: 2019 Fabian Riethmayer <fabian@web2.0-apps.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

Item {
    id: root

    required property var source
    required property string name
    property alias actions: toolbar.actions

    height: 170

    Item {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            color: avatar.color
            opacity: 0.2

        }
        Kirigami.Icon {
            visible: source
            scale: 1.8
            anchors.fill: parent

            source: root.source

            implicitWidth: 512
            implicitHeight: 512
        }

        layer.enabled: true
        layer.effect: HueSaturation {
            cached: true

            saturation: 1.9

            layer {
                enabled: true
                effect: FastBlur {
                    cached: true
                    radius: 100
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: -1.0; color: "transparent" }
            GradientStop { position: 1.0; color: Kirigami.Theme.backgroundColor }
        }
    }

    RowLayout {
        anchors.fill: parent
        RowLayout {
            Layout.maximumWidth: Kirigami.Units.gridUnit * 30
            Layout.alignment: Qt.AlignHCenter


            Kirigami.ShadowedRectangle {
                color: Kirigami.Theme.backgroundColor
                Layout.margins: 30
                width: 120
                height: width
                radius: width/2

                shadow {
                    size: 15
                    xOffset: 5
                    yOffset: 5
                    color: Qt.rgba(0, 0, 0, 0.2)
                }

                Components.Avatar {
                    id: avatar

                    height: parent.height
                    width: height

                    visible: !imageIcon.visible
                    name: root.name
                    imageMode: Components.Avatar.ImageMode.AdaptiveImageOrInitals
                }

                Kirigami.Icon {
                    id: imageIcon

                    height: parent.height
                    width: height

                    source: root.source
                    visible: source

                    layer {
                        enabled: imageIcon.visible
                        effect: OpacityMask {
                            maskSource: Rectangle {
                                width: imageIcon.width
                                height: imageIcon.width
                                radius: imageIcon.width
                                color: "black"
                                visible: false
                            }
                        }
                    }
                }

            }
            ColumnLayout {
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.fillWidth: true

                QQC2.Label {
                    Layout.fillWidth: true
                    text: root.name
                    font.bold: true
                    font.pixelSize: 22
                    maximumLineCount: 2
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight

                }

                Kirigami.ActionToolBar {
                    id: toolbar
                }
            }
        }
    }
}

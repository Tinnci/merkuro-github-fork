// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

Map {
    id: map
    anchors.fill: parent

    property alias pluginComponent: mapPlugin

    //center: QtPositioning.coordinate(0,0)
    //zoomLevel: 15
    gesture.enabled: true
    plugin: Plugin {
        id: mapPlugin
        name: "osm"
    }
    onCopyrightLinkActivated: {
        Qt.openUrlExternally(link)
    }
    Component.onCompleted: console.log(errorString)
}


// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15
import org.kde.kirigami 2.15 as Kirigami

Map {
    id: map
    anchors.fill: parent

    property alias pluginComponent: mapPlugin
    property string query
    property bool queryHasResults: geocodeModel.count > 0
    property int queryStatus: geocodeModel.status
    property bool containsLocation: visibleRegion.contains(geocodeModel.get(0).coordinate)
    property bool selectMode: false

    function goToLocation() {
        fitViewportToGeoShape(geocodeModel.get(0).boundingBox, 0);
        if (map.zoomLevel > 18.0) {
            map.zoomLevel = 18.0;
        }
    }

    gesture.enabled: true
    plugin: Plugin {
        id: mapPlugin
        name: "osm"
    }
    onCopyrightLinkActivated: {
        Qt.openUrlExternally(link)
    }

    Button {
        anchors.right: parent.right
        text: i18n("Return to location")
        visible: !map.containsLocation && map.query
        onClicked: map.goToLocation()
        z: 10
    }

    MapItemView {
        MouseArea {
            anchors.fill: parent
            enabled: map.selectMode
            onClicked: {
                var coords = map.toCoordinate(Qt.point(mouseX, mouseY), false)
                geocodeModel.query = coords
            }
        }

        model: GeocodeModel {
            id: geocodeModel
            plugin: map.pluginComponent
            query: map.query
            autoUpdate: true
            limit: 1
            onLocationsChanged: {
                goToLocation()
                map.query = geocodeModel.get(0).address.text
                query = map.query
            }
        }

        delegate: MapCircle {
            id: point
            radius: locationData.boundingBox.center.distanceTo(locationData.boundingBox.topRight) // map.zoomLevel
            color: Kirigami.Theme.highlightColor
            border.color: Kirigami.Theme.linkColor
            border.width: Kirigami.Units.devicePixelRatio * 2
            smooth: true
            opacity: 0.25
            center: locationData.coordinate
        }
    }
}

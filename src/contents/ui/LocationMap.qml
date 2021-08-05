import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

import org.kde.kirigami 2.15 as Kirigami

Map {
        id: map
        anchors.fill: parent
        //center: QtPositioning.coordinate(0,0)
        //zoomLevel: 15
        gesture.enabled: true
        plugin: Plugin {
            name: "osm"
            /*PluginParameter {
                name: "osm.mapping.providersrepository.disabled"
                value: "true"
            }*/
        }
        onCopyrightLinkActivated: {
            Qt.openUrlExternally(link)
        }
        Component.onCompleted: console.log(errorString)
    }


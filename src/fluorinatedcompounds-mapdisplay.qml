import QtQuick 2.15
import QtQuick.Controls 2.15 
import QtLocation 6
import QtPositioning 6

Rectangle {
    width: 800
    height: 600

    Plugin {
        id: openSrtMp
        name: "osm" // Use OpenStreetMap plugin
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: openSrtMp

        center {
            latitude: 35.6895
            longitude: 139.6917
        }
        zoomLevel: 14
    }
}

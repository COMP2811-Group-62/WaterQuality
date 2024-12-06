import QtQuick 2.15
import QtQuick.Controls 2.15 
import QtLocation 6
import QtPositioning 6

Rectangle {
    width: 800
    height: 600

    Map {
        id: map
        anchors.fill: parent
        plugin: Plugin { name: "osm" }

        zoomLevel: 14
        center {
            latitude: 53.80908899159547
            longitude: -1.5539251847199977
        }
        
        DragHandler {
            id: drag
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }
        WheelHandler {
            id: wheel
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            rotationScale: 1/256
            property: "zoomLevel"
        }


        MapCircle {
            center {
                latitude: 53.80908899159547
                longitude: -1.5639251847199977
            }
            radius: 2500.0
            color: 'green'
            opacity: 0.5
        }

    }
}

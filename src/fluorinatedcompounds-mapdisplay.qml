import QtQuick 2.15
import QtQuick.Controls 2.15 
import QtLocation 6
import QtPositioning 6

Rectangle {
    width: 800
    height: 600
    border.color: "#1a1a1a"
    border.width: 2
    radius: 8.0

    Plugin { 
        id: mapPlugin
        name: "osm" 
        PluginParameter {
            name: "osm.mapping.custom.host" 
            value: "https://tile.openstreetmap.org/" 
        } 
    }


    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        activeMapType: map.supportedMapTypes[map.supportedMapTypes.length - 1]
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
    function test(msg: string) {
        console.log("Got message:", msg)
    }
}

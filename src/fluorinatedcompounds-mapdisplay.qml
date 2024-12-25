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
            radius: 100000
            color: 'green'
            opacity: 0.5
        }
    }

    function addCircle(colour, dataURL) { 
        var requestURL = "https://environment.data.gov.uk/water-quality/id/sampling-point/" + dataURL
        console.log(requestURL)
        console.log("Adding circle")
        // use the dataURL from the CSV to get the lat + long to use to add to the map rather than easting and norting

        var httpRequest = new XMLHttpRequest();  
        httpRequest.onreadystatechange = function() { 
        httpRequest.responseType = XMLHttpRequest.JSON;
            
            // request will run async

            if (httpRequest.readyState === XMLHttpRequest.DONE) { 
                console.log("Request completed")
                if (httpRequest.status === 200) { 
                    
                    // once request complete and 200

                    var jsonResponse = JSON.parse(httpRequest.responseText);
                    var longitude = jsonResponse.items[0].long;
                    var latitude = jsonResponse.items[0].lat;
                    console.log(latitude, longitude)

                    // create the circle now we have all required data
                    createCircle(latitude, longitude, colour)
                } 
                else { 
                    console.log("Error: " + httpRequest.status); 
                } 
            } 
        } 
        httpRequest.open("GET", requestURL); 
        httpRequest.send(); 
        console.log("Request sent to:", dataURL)
    }
    function createCircle(lat, lon, colour) { 
        var circle = Qt.createQmlObject('
            
            import QtLocation; 
            MapCircle {
                radius: 2500
                opacity: 0.5
            }
            
            ', map)
        
        circle.center.latitude = lat
        circle.center.longitude = lon
        circle.color = colour
        map.addMapItem(circle)

        console.log("cirle:", circle)
        console.log("Centre: ", circle.center)
        console.log("Radius: ", circle.radius)
        
        map.fitViewportToMapItems()

    }
    
    function clearMap() {
        map.clearMapItems()
    }
}

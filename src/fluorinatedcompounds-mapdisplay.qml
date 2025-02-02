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
    // take in data from C++ and perform a request to the environment API, then either make a circle or set the center

    function addCircle(colour, dataURL, moveView, locationName, pollutantName, date, resultValue, resultUnits) { 
        var requestURL = "https://environment.data.gov.uk/water-quality/id/sampling-point/" + dataURL

        // use the dataURL from the CSV to get the lat + long to use to add to the map rather than easting and norting

        var httpRequest = new XMLHttpRequest();  
        httpRequest.onreadystatechange = function() { 
        httpRequest.responseType = XMLHttpRequest.JSON;
            
            // request will run async

            if (httpRequest.readyState === XMLHttpRequest.DONE) { 
                if (httpRequest.status === 200) { 
                    
                    // once request complete and 200

                    var jsonResponse = JSON.parse(httpRequest.responseText);
                    var longitude = jsonResponse.items[0].long;
                    var latitude = jsonResponse.items[0].lat;

                    // create the circle now we have all required data
                    if (moveView) {
                        moveViewport(latitude, longitude)
                    }
                    else {
                        createCircle(latitude, longitude, colour, locationName, pollutantName, date, resultValue, resultUnits)
                    }
                   
                } 
                else { 
                    console.log("Error: " + httpRequest.status); 
                } 
            } 
        } 
        httpRequest.open("GET", requestURL); 
        httpRequest.send(); 
    }
    function createCircle(lat, lon, colour, locationName, pollutantName, date, resultValue, resultUnits) { 
        
        // create a new QML MapCircle object and assign values, then pass to map

        var circle = Qt.createQmlObject('
            
            import QtLocation; 
            import QtQuick;
            import QtQuick.Controls;

            MapCircle {
                radius: 2500
                opacity: 0.5
            
            property alias toolTipText: toolTip.text

            MouseArea { 
                    id: mouseArea 
                    anchors.fill: parent 
                    hoverEnabled: true 
                    onEntered: toolTip.visible = true 
                    onExited: toolTip.visible = false 
                }      

            ToolTip { 
                    id: toolTip 
                    visible: false 
                    text: "<>"
                    delay: 500 
                }
            }
            ', map)
        
        circle.center.latitude = lat
        circle.center.longitude = lon
        circle.color = colour

        circle.toolTipText = locationName + " on " + date + " with pollutant " + pollutantName + " at level " + resultValue + resultUnits 

        map.addMapItem(circle)

        map.fitViewportToMapItems()
    }
    
    function clearMap() {
        map.clearMapItems()
    }
    // change location based off user selection
    function moveViewport(lat, lon) {
        map.center.latitude  = lat
        map.center.longitude = lon
        map.zoomLevel = 12
    }
}

import QtQuick 2.0
import QtGraphicalEffects 1.0
Item {

    property int speed: 0
    id: innerRings
    height: 275 //335 //TODO: Groesse skalierbar machen
    width: height
    x: (parent.width / 2) - (width / 2)
    y: (parent.height / 2) - (height / 2)

    Image {
         id: innerRingRect
         height: parent.height
         width: parent.width
         source: "/pics/Tacho_Mitte.png"


         Text {
             id: speeddigit
             text: speed
             font.pixelSize: 86
             font.bold: true
             font.family: "Eurostile"
             y: 40
             color: "white"
             anchors.horizontalCenter: parent.horizontalCenter
         }

         DropShadow {
                 anchors.fill: speeddigit
                 horizontalOffset: 0
                 verticalOffset: 8
                 radius: 4.0
                 samples: 16
                 color: "black"
                 source: speeddigit
             }

         Text {
             text: "kmh"
             font.pixelSize: 16
             font.bold: true
             font.family: "Eurostile"
             y: 132
             color: "white"
             anchors.horizontalCenter: parent.horizontalCenter
         }

         Image {
             id: batteryimage
             source: "/pics/b100.png"
            anchors.horizontalCenter: parent.horizontalCenter
            y: 170 //SA parent.height /2 + 7
         }

         Text {
             id: batteryPercent
             text: "%100"
             font.pixelSize: 28
             font.bold: true
             font.family: "Eurostile"
             y: 200 // SA 220
             color: "white"
             anchors.horizontalCenter: parent.horizontalCenter
         }
         Image {
             source: "/pics/ae2y.png"
             scale: 0.6
            anchors.horizontalCenter: parent.horizontalCenter
            y: 250 //SA parent.height /2 + 7
         }


    }
    function batteryStatus(batterys)
    {
        if(batterys>48){
            batteryimage.source="/pics/b100.png"
            batteryPercent.text="%100"}
        else if(batterys>47.9 && batterys<=48){
            batteryimage.source="/pics/b90.png"
            batteryPercent.text="%90"}
        else if(batterys>47.8 && batterys<=47.9){
            batteryimage.source="/pics/b80.png"
            batteryPercent.text="%80"}
        else if(batterys>47.7 && batterys<=47.8){
            batteryimage.source="/pics/b70.png"
            batteryPercent.text="%70"}
        else if(batterys>47.6 && batterys<=47.7){
            batteryimage.source="/pics/b60.png"
            batteryPercent.text="%60"}
        else if(batterys>47.5 && batterys<=47.6){
            batteryimage.source="/pics/b50.png"
                batteryPercent.text="%50"}
        else if(batterys>47.4 && batterys<=47.5){
            batteryimage.source="/pics/b40.png"
            batteryPercent.text="%40"}
        else if(batterys>47.3 && batterys<=47.4){
            batteryimage.source="/pics/b30.png"
            batteryPercent.text="%30"}
        else if(batterys>47.2 && batterys<=47.3){
            batteryimage.source="/pics/b20.png"
            batteryPercent.text="%20"}
        else if(batterys>47.1 && batterys<=47.2){
            batteryimage.source="/pics/b10.png"
            batteryPercent.text="%10"}
        else if(batterys>47 && batterys<=47.1){
            batteryimage.source="/pics/b0.png"
            batteryPercent.text="%0"}
    }
}

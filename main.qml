import QtQuick 2.9
import QtQuick.Window 2.2

import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    title: qsTr("Ae2 Speedometer")
    width: 800
    height: 480
    visible: true
    id:screen
    color: "black"

    MouseArea {
            anchors.fill: parent;
            onClicked: {


                // Ekranda her hangi bir yere tıklayınca


            }
        }

       QtObject {
           property var locale: Qt.locale()
           property date currentDate: new Date()
           property string dateString
           property string timeString

           Component.onCompleted: {
               dateString = currentDate.toLocaleDateString();
               timeString = currentDate.toLocaleTimeString();
           }
       }


       Image {
           width: parent.width
           height: parent.height
           source: "/pics/Background.png"
       }


    Rectangle {
    y: parent.height - 90
    height: 200
    width: parent.width
    color: "transparent"

            RowLayout {
                   y: 40
                   x: 170
                   Text {
                       id:mil

                       font.pixelSize: 20
                       font.bold: true
                       color: "white"
                   }
                   Text {
                       text: ""
                       font.pixelSize: 20
                       font.bold: false
                       color: "darkgray"
                   }
               }

            RowLayout {
                   y: 40
                   x: 100
                   Text {
                       id:temp
                       text: ""
                       font.pixelSize: 26
                       font.bold: true
                       color: "white"
                   }
                   Text {
                       text: ""
                       font.pixelSize: 26
                       font.bold: false
                       color: "darkgray"
                   }
               }


            RowLayout {
                y: 40
                x: 600
                Text {
                    //text: new Date().toLocaleDateString(Qt.locale(),"ddd") //ddd MMM d //h:mm AP
                    text : "AFD"
                    font.pixelSize: 20
                    font.bold: true
                    color: "white"
                }
                Text {
                  //  text: new Date().toLocaleDateString(Qt.locale(),"MMM d") //ddd MMM d //h:mm AP
                    font.pixelSize: 20
                    font.bold: false
                    color: "darkgray"
                }
            }

            RowLayout {
                y: 40
                x: 720
                Text {
                    id : pot
                 //   text: new Date().toLocaleTimeString(Qt.locale(),"h:mm") //ddd MMM d //h:mm AP
                    font.pixelSize: 20
                    font.bold: false
                    color: "darkgray"
                }

            }
        }

//Tacho
    Image {
           height: parent.height
           width: height
           x: (parent.width / 2) - (width / 2)
           scale: 1.14
           source: "/pics/Tacho.png"
           fillMode: Image.PreserveAspectFit
    }

    Speedometer {
       height: 525
       width: height
       x: (parent.width / 2) - (width / 2)
       y: (parent.height / 2) - (height / 2)
    }
}



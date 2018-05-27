import QtQuick 2.4
import QtGraphicalEffects 1.0

Rectangle {
    color: "transparent"
    property int j : 0
    property int seconds
    property int tenseconds
    property int minutes
    property int tenminutes
    property int hours
    property int loga;
    property double limitBatteryHeat : 50.0;
    property double limitMosfetHeat : 150.0;
    property double limitCurrent : 20.0;


    Connections{
        target:benimHaberciReferansi
        onUpdateScreen:{


            if(isMotorRun)
                engStatus.source = "/pics/engine_run.png";
            else
                engStatus.source = "/pics/engine_run_not.png";

            breakStatus(isBreak); // Fren basılı bilgi durumu
            deadSwitchStatus(isDeadSwitch); // Deadman switch bilgi durumu
            drive(isDeadSwitch); // Ekran göstergesindeki N ve D
            innerring.speed=speedWheel; // ortadaki decimal hız
            speedoNeedle.value=speedNeedleValue(speedWheel); // hız ibresini ayarlıyor
            wattText.text=parseFloat(batteryVoltage*batteryCurrent).toFixed(1);
            kWNeedle.value=kWNeedleValue(batteryVoltage*batteryCurrent); // Watt ibresi
            batteryVoltageText.text=parseFloat(batteryVoltage).toFixed(1) + " V";

            batteryCurrentText.text=parseFloat(batteryCurrent).toFixed(1) + " A";
            if(batteryCurrent > limitCurrent)
                batteryCurrentText.color = "red";
            else
                batteryCurrentText.color = "gold";

            batteryHeatText.text=parseFloat(batteryHeat).toFixed(1) + "ºC";
            if(batteryHeat > limitBatteryHeat)
                batteryHeatText.color = "red";
            else
                batteryHeatText.color = "gold";

            mosfetHeatText.text=parseFloat(mosfetHeat).toFixed(1) + "ºC";
            if(mosfetHeat > limitMosfetHeat)
                mosfetHeatText.color = "red";
            else
                mosfetHeatText.color = "gold";

            innerring.batteryStatus(batteryVoltage);
            totalW.text=parseFloat(totalWatt).toFixed(1) + " W";
            totalDistance.text=parseFloat(distance).toFixed(1) + " m";
            pot.text = parseFloat(speedValue).toFixed(1);
            errStatus(isEngineActive,isBatteryActive);

        }
    }
   /* MouseArea {
        anchors.fill: parent
        onClicked: {
            breakStatus(1)
            j++
            innerring.speed=j
            wH.text= j
            kWNeedle.value = kWNeedleValue(j)
            speedoNeedle.value=speedNeedleValue(j)

//benimHaberciReferansı.haberAl("Merhaba Türkiye")
        }
    }*/

          SpeedNeedle {
              id: speedoNeedle
                   anchors.verticalCenterOffset: 0
                   anchors.centerIn: parent
                   focus: true
              }

              KWNeedle {
                  id: kWNeedle
                  anchors.verticalCenterOffset: 0
                  anchors.centerIn: parent
                  value: 100

              }
              InnerRing    {
                id: innerring
                speed: 0//(Math.round(speedoNeedle.currentValue, 0) + 360) * 0.68

              }

              //Letter:  N D
              Grid {
                  anchors.horizontalCenter: parent.horizontalCenter
                  y: parent.height - 65
                  columns: 2

                  Rectangle { color: "transparent"; width: 25; height: 25
                          Text {
                              id:letterN
                              text: " N "
                               font.family: "Eurostile"; font.pixelSize: 18
                              color: "darkgray"
                              anchors.centerIn: parent
                          }}
                    Rectangle { color: "transparent"; width: 25; height: 25
                      Text {
                          id: letterD
                          text: " D "
                          font.family: "Eurostile"; font.pixelSize: 18
                          color: "darkgray"
                          anchors.centerIn: parent
                      }}
              }
              Text {
                  id: wattText
                  text: "0"
                  font.pixelSize: 30
                  font.bold: true
                  font.family: "Eurostile"
                  x: 400
                  y: 280
                  color: "white"

              }
              Text {

                  text: "Wh"
                  font.pixelSize: 18
                  font.bold: true
                  font.family: "Eurostile"
                  x: 400
                  y: 320
                  color: "white"
              }

              Grid{

                  columns: 3
                  x : -130
                  y : 50
                  columnSpacing: 8

                  Image {

                          id: errEngine
                          width: 40
                          height: 40
                          source: "pics/err_engine_deactive.png"
                      }


                  Image {
                      id: errBattery
                      width: 40
                      height: 40
                       source: "pics/err_battery_deactive.png"
                  }

                  Image {
                      id: engStatus
                      width: 40
                      height: 40
                       source: "pics/engine_run_not.png"
                  }


              }

              Rectangle {
                  width: 150
                  height: 60
                  x: -130
                  y: 110
                  radius: 10
                  gradient: Gradient {
                      GradientStop { position: 0.0; color: "gray" }
                      GradientStop { position: 1.0; color: "black" }
                  }
                  Text {
                      text: "Gerilim"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:5
                      font.bold: true
                      font.family: "Eurostile"
                      color: "black"
                  }
                  Text {
                      id: batteryVoltageText
                      text: 0 + " Volt"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:30
                      font.family: "Eurostile"
                      color: "gold"
                      style: Text.Raised; styleColor: "black"
                      }
              }
              Rectangle {
                  width: 150
                  height: 60
                  x: -130
                  y: 180
                  radius: 10
                  gradient: Gradient {
                      GradientStop { position: 0.0; color: "gray" }
                      GradientStop { position: 1.0; color: "black" }
                  }
                  Text {
                      text: "Akım"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:5
                      font.bold: true
                      font.family: "Eurostile"
                      color: "black"
                  }
                  Text {
                      id: batteryCurrentText
                      text: 0 + " Amper"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:30
                      font.family: "Eurostile"
                      color: "gold"
                      style: Text.Raised; styleColor: "black"
                      }
              }
              Rectangle {
                  width: 150
                  height: 60
                  x: -130
                  y: 250
                  radius: 10
                  gradient: Gradient {
                      GradientStop { position: 0.0; color: "gray" }
                      GradientStop { position: 1.0; color: "black" }
                  }
                  Text {
                      text: "B.Sıcaklık"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:5
                      font.bold: true
                      font.family: "Eurostile"
                      color: "black"
                  }
                  Text {
                      id: batteryHeatText
                      text: 0 + "º C"
                      font.pixelSize: 22
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:30
                      font.family: "Eurostile"
                      color: "gold"
                      style: Text.Raised; styleColor: "black"
                      }
              }
              Rectangle {
                  width: 150
                  height: 60
                  x: -130
                  y: 320
                  radius: 10
                  gradient: Gradient {
                      GradientStop { position: 0.0; color: "gray" }
                      GradientStop { position: 1.0; color: "black" }
                  }
                  Text {
                      text: "M.Sıcaklık"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:5
                      font.bold: true
                      font.family: "Eurostile"
                      color: "black"
                      }
                  Text {
                      id: mosfetHeatText
                      text: 0 + "º C"
                      font.pixelSize: 22
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:30
                      font.family: "Eurostile"
                      color: "gold"
                      style: Text.Raised; styleColor: "black"
                      }
              }

              Rectangle {
                  width: 160
                  height: 60
                  x: 500
                  y: 250
                  radius: 10
                  gradient: Gradient {
                      GradientStop { position: 0.0; color: "gray" }
                      GradientStop { position: 1.0; color: "black" }
                  }
                  Text {
                      text: "Harcanan Güç"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:5
                      font.bold: true
                      font.family: "Eurostile"
                      color: "black"
                  }
                  Text {
                      id: totalW
                      text: 0 + " W"
                      font.pixelSize: 22
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:30
                      font.family: "Eurostile"
                      color: "white"
                      style: Text.Raised; styleColor: "black"
                      }
              }
              Rectangle {
                  width: 160
                  height: 60
                  x: 500
                  y: 320
                  radius: 10
                  gradient: Gradient {
                      GradientStop { position: 0.0; color: "gray" }
                      GradientStop { position: 1.0; color: "black" }
                  }
                  Text {
                      text: "Alınan Yol"
                      font.pixelSize: 20
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:5
                      font.bold: true
                      font.family: "Eurostile"
                      color: "black"
                      }
                  Text {
                      id: totalDistance
                      text: 0 + " m"
                      font.pixelSize: 22
                      anchors.horizontalCenter: parent.horizontalCenter
                      y:30
                      font.family: "Eurostile"
                      color: "white"
                      style: Text.Raised; styleColor: "black"
                      }
                  Text {
                      id:wTime
                      y: 55;
                      anchors.horizontalCenter: parent.horizontalCenter

                      font.family: "Eurostile";
                      color: "white";
                      font.pixelSize: 20
                      text: hours + ":" + tenminutes + minutes + ":" + tenseconds + seconds
                  }
              }



              Rectangle{

                  anchors.horizontalCenter: parent.right;
                  anchors.verticalCenter: parent.top;
                  anchors.verticalCenterOffset: 50;


                  Image {
                      id: deadSwitch
                      fillMode: Image.PreserveAspectFit
                      width: 75
                      height: 75
                      source: "/pics/dead-switch-icon.png"
                      visible: false

                  }

                  Image {
                      id: stop
                      fillMode: Image.PreserveAspectFit
                      width: 75
                      height: 75
                      y:deadSwitch.height
                      source: "/pics/warning-icon.png"
                      visible: false

                  }


              }


              Timer {
                  //update Calltime, calculate 60 seconds into 1 minute etc.
                     interval: 1000; running: true; repeat: true
                     onTriggered: {seconds++;

                     if(seconds == 10){
                         tenseconds += 1
                         seconds = 0
                     }
                     if(seconds == 0 && tenseconds==6){
                         minutes += 1
                         seconds = 0
                         tenseconds = 0
                     }
                     if(minutes==10){
                         tenminutes += 1
                         minutes = 0
                      }
                      if(minutes == 0 && tenminutes==6){
                          hours += 1
                          tenminutes = 0
                      }

                     }
                 }

              function errStatus(isEngineActive,isBatteryActive){

                  if(isEngineActive)
                      errEngine.source = "/pics/err_engine_deactive.png"
                  else
                      errEngine.source = "/pics/err_engine_active.png"


                  if(isBatteryActive)
                      errBattery.source = "/pics/err_battery_deactive.png"
                  else
                      errBattery.source = "/pics/err_battery_active.png"



              }


              function deadSwitchStatus(isDeadSwitch){

                  if(isDeadSwitch)
                      deadSwitch.visible = false;
                  else
                      deadSwitch.visible = true;
              }

              function breakStatus(breaks){

                  if(breaks)
                      stop.visible=true
                  else
                      stop.visible=false

              }
              function kWNeedleValue(watt)
              {
                  if(watt == 1)
                      return 51
                  else if(watt>1 && watt<=5)
                      return 50
                  else if(watt>5 && watt<=10)
                      return 49
                  else if(watt>10 && watt<=15)
                      return 48
                  else if(watt>15 && watt<=20)
                      return 47
                  else if(watt>20 && watt<=25)
                      return 46
                  else if(watt>25 && watt<=30)
                      return 45
                  else if(watt>30 && watt<=35)
                      return 44
                  else if(watt>35 && watt<=37)
                      return 43
                  else if(watt>37 && watt<=40)
                      return 42
                  else if(watt>40 && watt<=55)
                      return 41
                  else if(watt>55 && watt<=60)
                      return 40
                  else if(watt>60 && watt<=75)
                      return 39
                  else if(watt>75 && watt<=80)
                      return 38
                  else if(watt>80 && watt<=100)
                      return 37
                  else if(watt>100 && watt<=120)
                      return 36
                  else if(watt>120 && watt<=140)
                      return 35
                  else if(watt>140 && watt<=160)
                      return 34
                  else if(watt>16 && watt<=200)
                      return 33
                  else if(watt>200 && watt<=240)
                      return 32
                  else if(watt>240 && watt<=300)
                      return 31
                  else if(watt>300 && watt<=320)
                      return 30
                  else if(watt>320)
                      return 0
                  else
                      return 52

              }
              function speedNeedleValue(speed) {

                    if(speed>0 && speed<=2)
                        return 1
                    else if(speed>2 && speed<=4)
                        return 2
                    else if(speed>4 && speed<=7)
                        return 3
                    else if(speed>7 && speed<=9)
                        return 4
                    else if(speed>9 && speed<=12)
                        return 5
                    else if(speed>12 && speed<=14)
                        return 6
                    else if(speed>14 && speed<=17)
                        return 7
                    else if(speed>14 && speed<=19)
                        return 8
                    else if(speed>19 && speed<=21)
                        return 9
                    else if(speed>21 && speed<=23)
                        return 10
                    else if(speed>23 && speed<=26)
                        return 11
                    else if(speed>26 && speed<=28)
                        return 12
                    else if(speed>28 && speed<=31)
                        return 13
                    else if(speed>31 && speed<=33)
                        return 14
                    else if(speed>33 && speed<=36)
                        return 15
                    else if(speed>36 && speed<=38)
                        return 16
                    else if(speed>38 && speed<=40)
                        return 17
                    else if(speed>40 && speed<=42)
                        return 18
                    else if(speed>42 && speed<=45)
                        return 19
                    else if(speed>45 && speed<=47)
                        return 20
                    else if(speed>47 && speed<=50)
                        return 21
                    else if(speed>50 && speed<=52)
                        return 22
                    else if(speed>52 && speed<=55)
                        return 23
                    else if(speed>55 && speed<=57)
                        return 24
                    else if(speed>57 && speed<=59)
                        return 25
                    else if(speed>59 && speed<=61)
                        return 26
                    else if(speed>61 && speed<=64)
                        return 27
                    else if(speed>64 && speed<=66)
                        return 28
                    else if(speed>66 && speed<=69)
                        return 29
                    else if(speed>69 && speed<=71)
                        return 30
                    else if(speed>71 && speed<=74)
                        return 31
                    else if(speed>74 && speed<=76)
                        return 32
                    else if(speed>76 && speed<=78)
                        return 33
                    else if(speed>78)
                        return 34
                    else
                        return 0

              }
              function drive(isDeadSwitch) {

                  if(!isDeadSwitch){

                      letterN.font.bold = true
                      letterN.color = "white"
                      letterN.font.pixelSize = 36
                      letterD.font.bold = false
                      letterD.color = "darkgray"
                      letterD.font.pixelSize = 18

                  }else{

                      letterD.font.bold = true
                      letterD.color = "white"
                      letterD.font.pixelSize = 36
                      letterN.font.bold = false
                      letterN.color = "darkgray"
                      letterN.font.pixelSize = 18

                  }


              }


 }




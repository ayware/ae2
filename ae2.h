#ifndef AE2_H
#define AE2_H

#include <iostream>
#include <QIODevice>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickView>
#include <QQuickItem>
#include <QTimer>
#include <QtDebug>
#include <QtSerialPort>
#include <QQmlContext>
class Ae2 : public QObject
{
        Q_OBJECT

    public:
        Ae2();
        QTimer *timer;
        int i=0;
        int j=0;
        int k=0;
        QSerialPort serial,serialGPS;
        int UartCRC(QByteArray crcarray, int crcleght);
        void UartWrite(QByteArray uartarray);
        QByteArray sendData;
        QByteArray received_Register;
        QByteArray Register;
        bool speedCommand=false;
        bool hornCommand=false;
        bool modCommand=false;
        bool stopCommand=false;
        float R=0.1; // 10 cm
        float wheelR = 20.0; //"20 cm"
        unsigned int breakStats=0;
        float encoderSpd=0;
        float encoderSpdCount=0;
        float mosfetHt=0;
        float MDCHt=0;        
        float Spd=0;
        float wheelSpd=0;
        float wheelSpdCountPrevios=0;
        float wheelSpdCount=0;
        float batteryCrnt=0;
        float batteryVltg=0;
        float batteryHt=0;
        float distancee=0;
        float totalWatt=0;
        float previoustotalW=0;
        float potValue = 0;
        int isDeadSwitch = 0;
        int isBreak = 0;
        bool isEngineActive = true;
        bool isBatteryActive = true;
        bool isWheelActive = true;


      signals:
        void haberYollaint(float encoderSpeed,
                           float mosfetHeats,
                           float MDCHeats,
                           float wheelSpeed,
                           float batteryCurrents,
                           float batteryVoltages,
                           float batteryHeats,
                           float distance,
                           float totalWatt,
                           int isDeadSwitch,
                           int isBreak,
                           float potValue,
                           bool isEngineActive,
                           bool isBatteryActive,
                           bool isWheelActive);

    public slots:
        void TimerTick();
        void haberAl(int s);

};

#endif // AE2_H

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
#include <QDataStream>

#include "myudp.h"

#define KNOTS_TO_KMH    0.5399568034557235
#define ACK 0x55

class Ae2 : public QObject
{
        Q_OBJECT



    enum COMMAND{

        COMMAND_DATA_CHECKER


    };

    enum COMMAND_TYPE{

        COMMAND_TYPE_DATA_REQUEST,
        COMMAND_TYPE_DATA_RESPONSE

    };


    public:
        Ae2();
        QTimer *timer;
        QSerialPort *serialTelemetry;
        QSerialPort *serialGps;
        QSerialPort *serialAT;

        MyUDP client;

        QByteArray sendData;
        QByteArray rcvGpsData;
        QByteArray frame;

        int timeToRequestData = 100;

        int isEngineActive = 0;
        int isBatteryActive = 0;
        int isDeadSwitch = 0;
        int isBreak = 0;
        int isMotorRun = 0;

        float mosfetHeat = 0;
        float motorControllerHeat = 0;
        float speedWheel = 0;
        float speedMotor = 0;
        float batteryCurrent = 0;
        float batteryVoltage = 0;
        float batteryHeat = 0;
        float distance = 0;
        float totalWatt = 0;
        float previousTotalWatt = 0;
        float speedValue = 0;
        float curWatt = 0;

        float latitude = 0;
        float longitude = 0;
        float gpsSpeed = 0;

    private:
        int crcCalc(QByteArray crcArray, int crcLenght);
        void uartWrite(QByteArray uartArray);
        void updateData(QByteArray data);



      signals:
        void updateScreen(float speedWheel,
                           float mosfetHeat,
                           float motorControllerHeat,
                           float batteryCurrent,
                           float batteryVoltage,
                           float batteryHeat,
                           float distance,
                           float curWatt,
                           float totalWatt,
                           int isDeadSwitch,
                           int isBreak,
                           float speedValue,
                           int isEngineActive,
                           int isBatteryActive,
                           int isMotorRun);

    public slots:
        void timerTick();
        void telemetryDataReceived( void );
        void gpsDataReceived( void );

};

#endif // AE2_H

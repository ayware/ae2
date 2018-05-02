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

class Ae2 : public QObject
{
        Q_OBJECT

    public:
        Ae2();
        QTimer *timer;
        QSerialPort *serialTelemetry;
        QSerialPort *serialGps;

        MyUDP client;

        QByteArray sendData;
        QByteArray received_Register;
        QByteArray Register;

        QByteArray received_GpsData;


        int timeToTimer = 100;

        int isEngineActive = 0;
        int isBatteryActive = 0;
        int isDeadSwitch = 0;
        int isBreak = 0;

        float mosfetHeat = 0;
        float motorControllerHeat = 0;
        float speedEncoder = 0;
        float speedMotor = 0;
        float batteryCurrent = 0;
        float batteryVoltage = 0;
        float batteryHeat = 0;
        float distance = 0;
        float totalWatt = 0;
        float previousTotalWatt = 0;
        float speedValue = 0;

        float latitude = 0;
        float longitude = 0;
        float gpsSpeed = 0;

    private:
        int UartCRC(QByteArray crcArray, int crcLenght);
        void UartWrite(QByteArray uartArray);


      signals:
        void haberYollaint(float speedEncoder,
                           float mosfetHeat,
                           float motorControllerHeat,
                           float batteryCurrent,
                           float batteryVoltage,
                           float batteryHeat,
                           float distance,
                           float totalWatt,
                           int isDeadSwitch,
                           int isBreak,
                           float speedValue,
                           int isEngineActive,
                           int isBatteryActive);

    public slots:
        void timerTick();
        void telemetryDataReceived( void );
        void gpsDataReceived( void );

};

#endif // AE2_H

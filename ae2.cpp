#include "ae2.h"

Ae2::Ae2()
{
    serialTelemetry = new QSerialPort(this);
    serialGps = new QSerialPort(this);

    connect(serialTelemetry, SIGNAL(readyRead()), this, SLOT(telemetryDataReceived()));
    connect(serialGps, SIGNAL(readyRead()), this, SLOT(gpsDataReceived()));

    serialTelemetry->setPortName("/dev/ttyS0");
    serialTelemetry->setBaudRate(QSerialPort::Baud115200);
    serialTelemetry->setParity(QSerialPort::NoParity);
    serialTelemetry->setStopBits(QSerialPort::OneStop);
    serialTelemetry->setFlowControl(QSerialPort::NoFlowControl);

    serialGps->setPortName("/dev/ttyUSB1");
    serialGps->setBaudRate(QSerialPort::Baud9600);
    serialGps->setParity(QSerialPort::NoParity);
    serialGps->setStopBits(QSerialPort::OneStop);
    serialGps->setFlowControl(QSerialPort::NoFlowControl);


    if(!serialTelemetry->isOpen()){
       serialTelemetry->open(QSerialPort::ReadWrite);
    }

    if(!serialGps->isOpen()){
       serialGps->open(QSerialPort::ReadWrite);
    }


    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    timer->start(timeToTimer);


    qDebug()<<"serialTelemetry IsOpenUart      :"<<serialTelemetry->isOpen()<<endl;
    qDebug()<<"serialTelemetry ReadableUart    :"<<serialTelemetry->isReadable()<<endl;
    qDebug()<<"serialTelemetry WritableUart    :"<<serialTelemetry->isWritable()<<endl<<endl;

    qDebug()<<"serialGps IsOpenUart      :"<<serialGps->isOpen()<<endl;
    qDebug()<<"serialGps ReadableUart    :"<<serialGps->isReadable()<<endl;
    qDebug()<<"serialGps WritableUart    :"<<serialGps->isWritable()<<endl<<endl;

}

void Ae2::telemetryDataReceived( void ){

    //qDebug()<<serialTelemetry->bytesAvailable();

    if(serialTelemetry->bytesAvailable()>=20){

        received_Register = serialTelemetry->read(20);
        serialTelemetry->readAll(); //Bufferda hatadan dolayı fazladan veri geldiyse silelim bosalsin.

        if( (UartCRC(received_Register,19) == (unsigned char)received_Register[19]) && (0x55 == received_Register[0]) )
        {

           //qDebug()<<"Uart Data Received"<<endl;

            Register=received_Register;

            speedMotor=Register[1]+((Register[2]<<8)&0xFF00);
            // Motora takılı encoder bilgisi

            speedEncoder = Register[3];
            // Tekerdeki encoder bilgisi

            motorControllerHeat=Register[4]+((Register[5]<<8)&0xFF00);
            motorControllerHeat=(147.5-(0.06*motorControllerHeat));
            // Motor haberleşme kartı controller heat

            mosfetHeat=Register[6]+((Register[7]<<8)&0xFF00);
            mosfetHeat=(147.5-(0.06*mosfetHeat));
            // Motor sürücü mosfet heat

            batteryCurrent=Register[8]+((Register[9]<<8)&0xFF00);
            batteryCurrent=batteryCurrent*0.0049;


            batteryVoltage=Register[10];

            batteryHeat=Register[12]+((Register[13]<<8)&0xFF00);
            batteryHeat=batteryHeat*0.0147;

            isBreak = Register[14];
            isDeadSwitch = Register[15];

            speedValue = Register[16];
            // Speed butonları ile sabitlenen hız sabiti

            isEngineActive = Register[17];
            isBatteryActive = Register[18];
            // Motor ve batarya haberleşme kartı kontrolü


            totalWatt=(((batteryCurrent*batteryVoltage))/(3600*(1000/timeToTimer)))+previousTotalWatt;
            previousTotalWatt=totalWatt;

            emit haberYollaint(speedEncoder,
                               mosfetHeat,
                               motorControllerHeat,
                               batteryCurrent,
                               batteryVoltage,
                               batteryHeat,
                               distance,
                               totalWatt,
                               isDeadSwitch,
                               isBreak,
                               speedValue,
                               isEngineActive,
                               isBatteryActive);

            //Gelen dataların sonuna gps bilgilerini ekleyip, tekrar crc olusturarak UDP uzerinden server'a gonderiyoruz.

            received_Register[19]=latitude;
            received_Register[20]=longitude;
            received_Register[21]=gpsSpeed;
            received_Register[22]=UartCRC(received_Register,22);

            client.sendData(received_Register);



         }

        received_Register.clear();

    }else{

        //qDebug()<<serialTelemetry->bytesAvailable();
    }
}

void Ae2::gpsDataReceived( void ){

    qDebug()<<serialGps->bytesAvailable();

    if(serialGps->canReadLine()){
        received_GpsData = serialGps->readLine();

        if( received_GpsData.startsWith("$GPRMC") ){

            qDebug()<<received_GpsData;

            QStringList list = QString(received_GpsData).split(",");

            qDebug()<<list;

            int latDegree = (((QString)list[3]).toDouble())/100;
            float latMin = (((QString)list[3]).toDouble())-latDegree*100;
            //qDebug()<<QString::number(latDegree)<<" - "<<QString::number(latMin)<<" - "<<QString::number(latDegree + latMin/60);
            latitude = latDegree + latMin/60;

            int lonDegree = (((QString)list[5]).toDouble())/100;
            float lonMin = (((QString)list[5]).toDouble())-lonDegree*100;
            //qDebug()<<QString::number(lonDegree)<<" - "<<QString::number(lonMin)<<" - "<<QString::number(lonDegree + lonMin/60);
            longitude = lonDegree + lonMin/60;

            float gpsSpeed = (((QString)list[7]).toDouble())/KNOTS_TO_KMH;

            qDebug()<<QString::number(latitude)<<" - "<<QString::number(longitude)<<" - "<<gpsSpeed;

        }else{

            received_GpsData.clear();
        }
    }

}


void Ae2::timerTick()
{
    if (serialTelemetry->isOpen() && serialTelemetry->isWritable())
    {
        sendData[0]=0x5A;
        sendData[1]=0x02;
        sendData[2]=0x02;
        sendData[3]=UartCRC(sendData,3);
        UartWrite(sendData);

    }else{

        serialTelemetry->open(QSerialPort::ReadWrite);
    }
}



int Ae2::UartCRC(QByteArray crcArray, int crcLenght)
{
    int calc=0,j=0;

    for(j=0;j<crcLenght;j++)
    {
        calc=calc+crcArray[j];
    }

    calc=calc%256;

    return calc;
}

void Ae2::UartWrite(QByteArray uartArray)
{
    serialTelemetry->write(uartArray);
    serialTelemetry->flush();
    serialTelemetry->waitForBytesWritten(timeToTimer);

}


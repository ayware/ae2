#include "ae2.h"

Ae2::Ae2()
{
    serialTelemetry = new QSerialPort(this);
    serialGps = new QSerialPort(this);

    connect(serialTelemetry, SIGNAL(readyRead()), this, SLOT(telemetryDataReceived()));
    connect(serialGps, SIGNAL(readyRead()), this, SLOT(gpsDataReceived()));

    serialTelemetry->setPortName("/dev/serial0");
    serialTelemetry->setBaudRate(QSerialPort::Baud115200);
    serialTelemetry->setParity(QSerialPort::NoParity);
    serialTelemetry->setStopBits(QSerialPort::OneStop);
    serialTelemetry->setFlowControl(QSerialPort::NoFlowControl);

    serialGps->setPortName("COM1");
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
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTick()));
    timer->start(timeToTimer);


    qDebug()<<"serialTelemetry IsOpenUart      :"<<serialTelemetry->isOpen()<<endl;
    qDebug()<<"serialTelemetry ReadableUart    :"<<serialTelemetry->isReadable()<<endl;
    qDebug()<<"serialTelemetry WritableUart    :"<<serialTelemetry->isWritable()<<endl<<endl;

    qDebug()<<"serialGps IsOpenUart      :"<<serialGps->isOpen()<<endl;
    qDebug()<<"serialGps ReadableUart    :"<<serialGps->isReadable()<<endl;
    qDebug()<<"serialGps WritableUart    :"<<serialGps->isWritable()<<endl<<endl;

}

void Ae2::telemetryDataReceived( void ){

    qDebug()<<serialTelemetry->bytesAvailable();

    if(serialTelemetry->bytesAvailable()>=24){

        received_Register = serialTelemetry->read(24);
        serialTelemetry->readAll(); //Bufferda hatadan dolayı fazladan veri geldiyse silelim bosalsin.

        if( (UartCRC(received_Register,23) == (unsigned char)received_Register[23]) && (0x55 == received_Register[0]) )
        {
            timerToSecond++;

            qDebug()<<"Uart Data Received"<<endl;

            Register=received_Register;

            breakStats=Register[1];
           // qDebug()<<"Break Status       :"<<breakStats<<endl;

            encoderSpdCount=Register[2]+((Register[3]<<8)&0xFF00);
            //qDebug()<<"Encoder Count      :"<<encoderSpdCount<<endl;

            mosfetHt=Register[4]+((Register[5]<<8)&0xFF00);

            mosfetHt=(147.5-(0.06*mosfetHt));
            //qDebug()<<"Mosfet Heat        :"<<mosfetHt<<endl;

            MDCHt=Register[6]+((Register[7]<<8)&0xFF00);
            MDCHt=(147.5-(0.06*MDCHt));
            //qDebug()<<"MDC Heat           :"<<MDCHt<<endl;

            wheelSpdCount=Register[8]+((Register[9]<<8)&0xFF00);
            //qDebug()<<"Wheel Speed Count  :"<<wheelSpdCount<<endl;

            Spd=Register[10];
            //qDebug()<<"Speed              :"<<Spd<<endl;

            batteryCrnt=Register[11]+((Register[12]<<8)&0xFF00);
            batteryCrnt=batteryCrnt*0.0049;
           // qDebug()<<"Battery Current    :"<<batteryCrnt<<endl;

            batteryVltg=Register[13]+((Register[14]<<8)&0xFF00);
            batteryVltg=batteryVltg*0.0117;
           // qDebug()<<"Battery Voltage    :"<<batteryVltg<<endl;

            batteryHt=Register[15]+((Register[16]<<8)&0xFF00);
            batteryHt=batteryHt*0.0147;
            //qDebug()<<"Battery Heat       :"<<batteryHt<<endl;

            isBreak = Register[17];
            isDeadSwitch = Register[18];
            potValue = Register[19];

            isEngineActive = ((int)Register[20]) == 1;
            isWheelActive = ((int)Register[21]) == 1;
            isBatteryActive = ((int)Register[22]) == 1;


            //qDebug()<<"isDeadSwitch    :"<<isDeadSwitch<<endl;
            //qDebug()<<"Distance            :"<<distancee<<endl;

            if(timerToSecond == 1000/(timeToRefresh)){

                timerToSecond = 0;

                wheelSpd = wheelSpdCount - wheelSpdCountPrevios;
                Spd = ((wheelSpd*2*3.14*(wheelR/2)/100000))/(3600);
                distancee=wheelSpdCount*2*3.14*(wheelR/2)/1000;
                qDebug()<<"Speed            :"<<Spd<<endl;

                wheelSpdCountPrevios = wheelSpdCount;

            }

            //qDebug()<<"Speed            :"<<wheelSpd<<endl;

            totalWatt=(((batteryCrnt*batteryVltg))/(3600*(1000/timeToRefresh)))+previoustotalW;
            //qDebug()<<"Total Watt          :"<<totalWatt<<endl;
            previoustotalW=totalWatt;


            emit haberYollaint(encoderSpd,
                               MDCHt,
                               MDCHt,
                               Spd,
                               batteryCrnt,
                               batteryVltg,
                               batteryHt,
                               distancee,
                               totalWatt,
                               isDeadSwitch,
                               isBreak,
                               potValue,
                               isEngineActive,
                               isBatteryActive,
                               isWheelActive);



         }else{

            timerToSecond = 0;

            emit haberYollaint(encoderSpd,
                               MDCHt,
                               MDCHt,
                               Spd,
                               batteryCrnt,
                               batteryVltg,
                               batteryHt,
                               distancee,
                               totalWatt,
                               isDeadSwitch,
                               isBreak,
                               potValue,
                               false,
                               false,
                               false);

        }

        received_Register.clear();

    }else{

        qDebug()<<serialTelemetry->bytesAvailable();
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


void Ae2::haberAl(int s)
{

    qDebug() << "Haber :" << s;
    //emit haberYollaint(30,50);
}

void Ae2::TimerTick()
{
    if (serialTelemetry->isOpen() && serialTelemetry->isWritable())
    {
        sendData[0]=0x5A;
        sendData[1]=0x06;
        sendData[2]=0x06;
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
    serialTelemetry->waitForBytesWritten(timeToWaitingRequest);
}


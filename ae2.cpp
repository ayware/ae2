#include "ae2.h"

Ae2::Ae2()
{
    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(dataReceived()));

    serial->setPortName("COM1");
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);


    if(!serial->isOpen()){
       serial->open(QSerialPort::ReadWrite);
    }


    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTick()));
    timer->start(timeToTimer);


    qDebug()<<"IsOpenUart      :"<< serial->isOpen()<<endl;
    qDebug()<<"ReadableUart    :"<<serial->isReadable()<<endl;
    qDebug()<<"WritableUart    :"<<serial->isWritable()<<endl<<endl;

}

void Ae2::dataReceived( void ){

    qDebug()<<serial->bytesAvailable();

    if(serial->bytesAvailable()>=24){

        received_Register = serial->read(24);

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
        serial->readAll();

    }else{

        qDebug()<<serial->bytesAvailable();
    }
}


void Ae2::haberAl(int s)
{

    qDebug() << "Haber :" << s;
    //emit haberYollaint(30,50);
}

void Ae2::TimerTick()
{
    if (serial->isOpen() && serial->isWritable())
    {
        sendData[0]=0x5A;
        sendData[1]=0x06;
        sendData[2]=0x06;
        sendData[3]=UartCRC(sendData,3);
        UartWrite(sendData);

    }else{

        serial->open(QSerialPort::ReadWrite);
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
    serial->write(uartArray);
    serial->flush();
    serial->waitForBytesWritten(timeToWaitingRequest);
}


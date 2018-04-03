#include "ae2.h"

int timeToRefresh = 100; // Ms
int timeToWaitingRequest = 50; // Ms
int timeToTimer = timeToRefresh - timeToWaitingRequest;

Ae2::Ae2()
{
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTick()));
    timer->start(timeToTimer);

    serial.close();
    serialGPS.close();

    serialGPS.setPortName("/dev/ttyUSB0");
    serialGPS.setBaudRate(QSerialPort::Baud9600);
    serialGPS.open(QSerialPort::ReadOnly);

    serial.setPortName("/dev/ttyS0");
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.open(QSerialPort::ReadWrite);


    qDebug()<<"IsOpenUart      :"<< serial.isOpen()<<endl;
    qDebug()<<"ReadableUart    :"<<serial.isReadable()<<endl;
    qDebug()<<"WritableUart    :"<<serial.isWritable()<<endl<<endl;
    qDebug()<<"IsOpenGPS       :"<<serialGPS.isOpen()<<endl;
    qDebug()<<"ReadableUart    :"<<serialGPS.isReadable()<<endl;

    serial.close();
    serialGPS.close();

}
void Ae2::haberAl(int s)
{

    qDebug() << "Haber :" << s;
    //emit haberYollaint(30,50);
}

int timerToSecond = 0;

void Ae2::TimerTick()
{
    //emit haberYollaint(30,40,100.1,4,32.1,23.2,9.2,55.5,1);


    serial.open(QSerialPort::ReadWrite);


    if (serial.isOpen() && serial.isWritable())
    {
        sendData[0]=0x5A;
        sendData[1]=0x06;
        sendData[2]=0x06;
        sendData[3]=UartCRC(sendData,3);
        UartWrite(sendData);
        received_Register = serial.read(24);

        if(UartCRC(received_Register,23)==received_Register[23] && 0x55==received_Register[0])
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

    //        qDebug() << "DeadSwitch : "<<isDeadSwitch<<endl;




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

        /*if(speedCommand==true)
        {
            sendData[0]=0x5A;
            sendData[1]=0x01;
            sendData[2]=0x00; // hız bilgisi
            sendData[3]=UartCRC(sendData,3);
            UartWrite(sendData);
            received_Register = serial.read(1);
            if(0x55==received_Register[0])
                speedCommand=false;

        }
        if(modCommand==true)
        {
            sendData[0]=0x5A;
            sendData[1]=0x02;
            sendData[2]=0x00; // hız bilgisi
            sendData[3]=UartCRC(sendData,3);
            UartWrite(sendData);
            received_Register = serial.read(1);
            if(0x55==received_Register[0])
                speedCommand=false;

        }
        if(hornCommand==true)
        {
            sendData[0]=0x5A;
            sendData[1]=0x04;
            sendData[2]=0x00; // hız bilgisi
            sendData[3]=UartCRC(sendData,3);
            UartWrite(sendData);
            received_Register = serial.read(1);
            if(0x55==received_Register[0])
                speedCommand=false;

        }
        if(stopCommand==true)
        {
            sendData[0]=0x5A;
            sendData[1]=0x00;
            sendData[2]=0x00; // hız bilgisi
            sendData[3]=UartCRC(sendData,3);
            UartWrite(sendData);
            received_Register = serial.read(1);
            if(0x55==received_Register[0])
                speedCommand=false;

        }

        */



       serial.close();
    }

    serialGPS.open(QSerialPort::ReadOnly);

    if(serialGPS.isOpen() && serialGPS.isReadable()){

        QByteArray gpsData = serialGPS.readAll();
        qDebug()<<"DataSize:"<<gpsData.length()<<endl;

        if(gpsData.length() != 0)
        {

            qDebug()<<"Data0:"<<gpsData.at(0)<<endl;

        }



        serialGPS.close();
    }


}



int Ae2::UartCRC(QByteArray crcarray, int crcleght)
{
    int calc=0,j=0;

        for(j=0;j<crcleght;j++)
        {
            calc=calc+crcarray[j];
        }

        calc=calc%256;

    return calc;
}

void Ae2::UartWrite(QByteArray uartarray)
{
    serial.write(uartarray);
    serial.flush();
    serial.waitForBytesWritten(timeToWaitingRequest);
}


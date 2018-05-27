#include "ae2.h"

Ae2::Ae2()
{
    serialTelemetry = new QSerialPort(this);
    serialGps = new QSerialPort(this);
    serialAT = new QSerialPort(this);

    connect(serialTelemetry, SIGNAL(readyRead()), this, SLOT(telemetryDataReceived()));
    connect(serialGps, SIGNAL(readyRead()), this, SLOT(gpsDataReceived()));

    serialTelemetry->setPortName("/dev/ttyS0");
    serialTelemetry->setBaudRate(QSerialPort::Baud115200);
    serialTelemetry->setParity(QSerialPort::NoParity);
    serialTelemetry->setStopBits(QSerialPort::OneStop);
    serialTelemetry->setDataBits(QSerialPort::Data8);
    serialTelemetry->setFlowControl(QSerialPort::NoFlowControl);

    serialGps->setPortName("/dev/ttyUSB1");
    serialGps->setBaudRate(QSerialPort::Baud9600);
    serialGps->setParity(QSerialPort::NoParity);
    serialGps->setStopBits(QSerialPort::OneStop);
    serialGps->setFlowControl(QSerialPort::NoFlowControl);

    serialAT->setPortName("/dev/ttyUSB2");
    serialAT->setBaudRate(QSerialPort::Baud9600);
    serialAT->setFlowControl(QSerialPort::NoFlowControl);

    if(!serialTelemetry->isOpen()){
       serialTelemetry->open(QSerialPort::ReadWrite);
    }

    if(!serialGps->isOpen()){
       serialGps->open(QSerialPort::ReadWrite);
    }

    uint32_t counter = 0;
    while(!serialAT->isOpen()){

        serialAT->open(QSerialPort::ReadWrite);
        counter++;
        if(counter == 1000)
            break;

    }


    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTick()));
    timer->start(timeToRequestData);


    qDebug()<<"serialTelemetry IsOpenUart      :"<<serialTelemetry->isOpen()<<endl;
    qDebug()<<"serialTelemetry ReadableUart    :"<<serialTelemetry->isReadable()<<endl;
    qDebug()<<"serialTelemetry WritableUart    :"<<serialTelemetry->isWritable()<<endl<<endl;

    qDebug()<<"serialGps IsOpenUart      :"<<serialGps->isOpen()<<endl;
    qDebug()<<"serialGps ReadableUart    :"<<serialGps->isReadable()<<endl;
    qDebug()<<"serialGps WritableUart    :"<<serialGps->isWritable()<<endl<<endl;

    qDebug()<<"serialAT IsOpenUart      :"<<serialAT->isOpen()<<endl;
    qDebug()<<"serialAT ReadableUart    :"<<serialAT->isReadable()<<endl;
    qDebug()<<"serialAT WritableUart    :"<<serialAT->isWritable()<<endl<<endl;

    // Gps modulünü aktif ediyoruz
    serialAT->write("AT-QGPS=1\r");
    serialAT->flush();

}

bool isPassedAck = false;
bool isPassedCtr = false;

uint8_t command = 0;
uint8_t commandType = 0;
uint8_t dataLength = 0;


void clearUart(void){

    // Cleared parameters to get new datas

    isPassedAck = false;
    isPassedCtr = false;
    command = 0;
    commandType = 0;
    dataLength = 0;

}



void Ae2::updateData(QByteArray data){

    if(data.length() == 0)
        return;

    speedMotor = data[0];

    speedWheel = data[1];

    motorControllerHeat =( ((data[2]<<8)&0xFF00) + ((data[3]<<0)&0xFF) ) / 10.0;

    mosfetHeat = ( ((data[4]<<8)&0xFF00) + ((data[5]<<0)&0xFF) ) / 10.0;

    isMotorRun = data[6];

    isEngineActive = data[8];

    batteryCurrent = ( ((data[9]<<8)&0xFF00) + ((data[10]<<0)&0xFF) ) / 10.0;

    batteryVoltage = ( ((data[11]<<8)&0xFF00) + ((data[12]<<0)&0xFF) ) / 10.0;

    batteryHeat = ( ((data[13]<<8)&0xFF00) + ((data[14]<<0)&0xFF) ) / 10.0;

    isBatteryActive = data[17];

    isBreak = data[18];

    isDeadSwitch = data[19];

    speedValue = data[20];

    curWatt = (batteryCurrent * batteryVoltage)*(float)(0.1);

    distance += speedWheel * 60 / 1000;

    totalWatt = curWatt + previousTotalWatt;
    previousTotalWatt = totalWatt;

    emit updateScreen(speedWheel,
                       mosfetHeat,
                       motorControllerHeat,
                       batteryCurrent,
                       batteryVoltage,
                       batteryHeat,
                       distance,
                       curWatt,
                       totalWatt,
                       isDeadSwitch,
                       isBreak,
                       speedValue,
                       isEngineActive,
                       isBatteryActive,
                       isMotorRun);



}


void Ae2::telemetryDataReceived( void ){

    if(!isPassedAck){

        if(serialTelemetry->bytesAvailable() >= 1 && ACK == (unsigned char)(serialTelemetry->read(1))[0]){

            frame.clear();
            frame.append(ACK);

            isPassedAck = true;
            //qDebug()<<"ACK ok;";

        }

    }else{


        if(!isPassedCtr){


            if(serialTelemetry->bytesAvailable() >= 3){

                QByteArray rcv;
                rcv = serialTelemetry->read(3);

                command = rcv[0];
                commandType = rcv[1];
                dataLength = rcv[2];

                frame.append(rcv);

                //qDebug()<<"Command:"<<command;
                //qDebug()<<"CommandType:"<<commandType;
                //qDebug()<<"DataLength:"<<dataLength;

                isPassedCtr = true;
            }

        }else{

      

            // Data işlemleri

            switch(command){

            case COMMAND_DATA_CHECKER:

                switch (commandType) {

                case COMMAND_TYPE_DATA_RESPONSE:

                    if(serialTelemetry->bytesAvailable() >= dataLength + 1){

                        QByteArray rcvData;
                        rcvData = serialTelemetry->read(dataLength);
                        uint8_t crc = (serialTelemetry->read(1))[0];


                        frame.append(rcvData);
                        serialTelemetry->readAll();

                        //qDebug()<<"Data:\n"<<rcvData;

                        //qDebug()<<"Crc:"<<crc;

                        uint32_t crcCalculated = crcCalc(frame,4 + dataLength);


                        //qDebug()<<"CrcCalc:"<<crcCalculated;

                    
                        if(crc == crcCalculated){

                            //qDebug()<<"Crc ok!";

                            updateData(rcvData);

                            uint32_t lat = latitude * 1000000;
                            uint32_t lon = longitude * 1000000;
                            uint32_t dist = distance * 1000000;

                            frame.append( (lat>>24)&0xFF );
                            frame.append( (lat>>16)&0xFF );
                            frame.append( (lat>>8)&0xFF );
                            frame.append( (lat>>0)&0xFF );


                            frame.append( (lon>>24)&0xFF );
                            frame.append( (lon>>16)&0xFF );
                            frame.append( (lon>>8)&0xFF );
                            frame.append( (lon>>0)&0xFF );

                            frame.append( (dist>>24)&0xFF );
                            frame.append( (dist>>16)&0xFF );
                            frame.append( (dist>>8)&0xFF );
                            frame.append( (dist>>0)&0xFF );

                            // Gps verileri eklenince dataLengthi değiştirdik
                            QByteArray arr;
                            arr.append(dataLength + 12);
                            frame.replace(3,1,arr);

                            // New crc appended
                            frame.append(crcCalc(frame,frame.length()));

                            //qDebug()<<"ServerFrame:\n"<<frame;
							

                            // Datas sent to server
                            client.sendData(frame);


                        }else{

                            //qDebug()<<"Crc fail!";

                        }


                        clearUart();
                    }

                    break;

                default:
                       clearUart();


                }


                break;

            default:
                clearUart();

            }

        }
    }
}






void Ae2::gpsDataReceived( void ){

    //qDebug()<<serialGps->bytesAvailable();

    if(serialGps->canReadLine()){
        rcvGpsData = serialGps->readLine();

        if( rcvGpsData.startsWith("$GPRMC") ){

            //qDebug()<<rcvGpsData;

            QStringList list = QString(rcvGpsData).split(",");

            //qDebug()<<list;

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

            rcvGpsData.clear();
        }
    }

}


void Ae2::timerTick()
{
    if (serialTelemetry->isOpen() && serialTelemetry->isWritable())
    {
        sendData[0] = ACK;
        sendData[1] = COMMAND_DATA_CHECKER;
        sendData[2] = COMMAND_TYPE_DATA_REQUEST;
        sendData[3] = 0;
        sendData[4] = (ACK + COMMAND_DATA_CHECKER + COMMAND_TYPE_DATA_REQUEST + 0) % 256;
        uartWrite(sendData);

    }else{

        serialTelemetry->open(QSerialPort::ReadWrite);
    }
}



int Ae2::crcCalc(QByteArray crcArray, int crcLenght)
{
    int calc=0,j=0;

    for(j=0;j<crcLenght;j++)
        calc += crcArray[j];

    return calc % 256;

}

void Ae2::uartWrite(QByteArray uartArray)
{
    serialTelemetry->write(uartArray);
    serialTelemetry->flush();
}


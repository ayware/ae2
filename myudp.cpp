#include "myudp.h"

MyUDP::MyUDP(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress("178.62.206.211"), 1235);
}

void MyUDP::sendData(QByteArray Data)
{

    socket->writeDatagram(Data, QHostAddress("178.62.206.211"), 1234);
}

#ifndef MYUDP_H
#define MYUDP_H

#include <QObject>
#include <QUdpSocket>

class MyUDP : public QObject
{
    Q_OBJECT
public:
    explicit MyUDP(QObject *parent = nullptr);

    void sendData(QByteArray Data);

private:
    QUdpSocket *socket;

signals:

public slots:
};

#endif // MYUDP_H

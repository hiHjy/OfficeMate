#include "envdecter.h"

#include <QDebug>
#include <QThread>

#define DEVICE "/dev/dht11"
#define SOCKET_PATH "/tmp/dht11.sock"
EnvDecter::EnvDecter(QObject *parent) : QObject(parent)
{



//    file = new QFile(DEVICE, this);
//    file->open(QIODevice::ReadOnly);
//    if (!file->isOpen()) {
//        qDebug() << "can not open "  << DEVICE;
//    }
    socket = new QLocalSocket(this);
    socket->connectToServer(SOCKET_PATH);
//    if (!socket->isValid()) {
//        qDebug() << socket->errorString();
//        return;
//    }
    connect(socket, &QLocalSocket::readyRead, this, &EnvDecter::readEnvStatus);
    connect(socket, &QLocalSocket::connected, [](){
       qDebug() << "[DHT11 Client] Connected successfully";
    });
    connect(socket, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), [this](QLocalSocket::LocalSocketError socketError){
       qDebug() << "err:" << socketError << " " << this->socket->errorString();
    });
    qDebug() << "dht11 local socket init successfully";

}

void EnvDecter::readEnvStatus()
{
//    QFile file(DEVICE);
//    file.open(QIODevice::ReadOnly);
//    if (!file.isOpen()) {
//        qDebug() << DEVICE << " can not open";
//        return;
//    }
//    char data[5];

//    /*
//        read() 可能返回：

//        0（没读到）

//        -1（错误）

//        <5（读到不完整）

//        5（正常）
//s
//    */
//    if (file.read(data, sizeof(data)) == 5) {

//        quint8 u0 = static_cast<quint8>(data[0]);
//        quint8 u2 = static_cast<quint8>(data[2]);
//        quint8 u3 = static_cast<quint8>(data[3]);
//        QString humidity = QString::number(u0) + ".0%RH";
//        QString temp = QString::number(u2) + "." + QString::number(u3) + "℃";
//        emit envStatusUpdated(temp, humidity);
//    }
    QByteArray frame;
    rx += socket->readAll();

    while(rx.size() >= 5) {
        frame = rx.left(5);
        rx.remove(0, 5);
        quint8 u0 = static_cast<quint8>(frame[0]);
        quint8 u2 = static_cast<quint8>(frame[2]);
        quint8 u3 = static_cast<quint8>(frame[3]);
        QString humidity = QString::number(u0) + ".0%RH";
        QString temp = QString::number(u2) + "." + QString::number(u3) + "℃";
        emit envStatusUpdated(temp, humidity);
    }



}


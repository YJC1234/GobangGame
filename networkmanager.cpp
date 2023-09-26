#include "networkmanager.h"
#include <QDataStream>
#include <QByteArray>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent), server(nullptr), socket(nullptr) {}

void NetworkManager::startServer(){
    if(!server){
        server = new QTcpServer(this);
        connect(server, &QTcpServer::newConnection, this, &NetworkManager::newConnection);
        server->listen(QHostAddress::Any, 12345);
    }
}

void NetworkManager::connectToServer(const QString &address, quint16 port){
    if(!socket){
        socket = new QTcpSocket(this);
        socket->connectToHost(address, port);
        connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::readData);
    }
}

void NetworkManager::sendData(int x, int y){
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << x << y;
    socket->write(block);
}

void NetworkManager::newConnection(){
    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::readData);
}

void NetworkManager::readData(){
    QDataStream in(socket);
    int x,y;
    in >> x >> y;
    emit receiveMove(x, y);
}


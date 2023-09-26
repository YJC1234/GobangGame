#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);

    void startServer();
    void connectToServer(const QString& address, quint16 port);

signals:
    void receiveMove(int x, int y);

public slots:
    void sendData(int x, int y);

private slots:
    void newConnection();
    void readData();

private:
    QTcpServer *server;
    QTcpSocket *socket;

};

#endif // NETWORKMANAGER_H

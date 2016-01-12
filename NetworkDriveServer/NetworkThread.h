#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QThread>
#include "NetworkDriveServer.h"

class NetworkThread : public QThread
{
    Q_OBJECT
    int m_port;
    NetworkDriveServer *m_server;

public:
    NetworkThread(int port = 12345);

    void run();

    NetworkDriveServer *getNetworkDriveServer();
signals:

public slots:
};

#endif // NETWORKTHREAD_H

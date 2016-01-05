#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QThread>
#include "NetworkDriveServer.h"

class NetworkThread : public QThread
{
    Q_OBJECT

    NetworkDriveServer *m_server;

public:
    NetworkThread();

    void run();

    NetworkDriveServer *getNetworkDriveServer();
signals:

public slots:
};

#endif // NETWORKTHREAD_H

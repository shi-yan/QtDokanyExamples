#include "NetworkThread.h"

NetworkThread::NetworkThread():QThread(),m_server(NULL)
{

}

void NetworkThread::run()
{
    m_server = new NetworkDriveServer();
    m_server->listen(QHostAddress::LocalHost, 12345);
    exec();
}

NetworkDriveServer *NetworkThread::getNetworkDriveServer()
{
    return m_server;
}

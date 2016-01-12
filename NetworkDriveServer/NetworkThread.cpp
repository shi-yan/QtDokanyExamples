#include "NetworkThread.h"

NetworkThread::NetworkThread(int port):QThread(),m_port(port),m_server(NULL)
{

}

void NetworkThread::run()
{
    m_server = new NetworkDriveServer();
    m_server->listen(QHostAddress::Any, m_port);
    exec();
}

NetworkDriveServer *NetworkThread::getNetworkDriveServer()
{
    return m_server;
}

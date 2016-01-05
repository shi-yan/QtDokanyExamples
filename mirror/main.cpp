#include "FileSystemThread.h"
#include "NetworkDriveClient.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    FileSystemThread *fsThread = new FileSystemThread();

    fsThread->start();
    NetworkDriveClient *networkDriveClient = new NetworkDriveClient();
    networkDriveClient->connectToServer();
    return app.exec();
}

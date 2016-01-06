#include "FileSystemThread.h"
#include "LocalDrive.h"
#include "DokanDrive.h"
#include "NetworkDrive.h"
#include "NetworkDriveClient.h"
#include "NetworkThread.h"

FileSystemThread::FileSystemThread()
{

}

void FileSystemThread::run()
{

    NetworkThread *netThread = new NetworkThread();
    netThread->start();

    while(!netThread->getNetworkDriveServer());

    NetworkDrive *networkDrive = new NetworkDrive(netThread->getNetworkDriveServer());
    DokanMirrorDriveInterface::mount(networkDrive);
/*
    LocalDrive *localDrive = new LocalDrive();
    DokanMirrorDriveInterface::mount(localDrive);*/
   // exec();

}


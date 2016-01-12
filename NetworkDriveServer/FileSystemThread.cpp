#include "FileSystemThread.h"
#include "../Common/DokanDrive.h"
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

    NetworkDrive *networkDrive = new NetworkDrive("c:\\test", netThread->getNetworkDriveServer());
    DokanMirrorDriveInterface::mount("m", networkDrive);
}


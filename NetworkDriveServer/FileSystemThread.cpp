#include "FileSystemThread.h"
#include "../Common/DokanDrive.h"
#include "NetworkDrive.h"
#include "NetworkDriveClient.h"
#include "NetworkThread.h"

FileSystemThread::FileSystemThread(const QString &directory, const QString &letter, NetworkDriveServer *server)
    :m_directory(directory),
      m_letter(letter),
      m_server(server)
{

}

void FileSystemThread::run()
{
    NetworkDrive *networkDrive = new NetworkDrive(m_directory, m_server);
    DokanMirrorDriveInterface::mount(m_letter, networkDrive);
}


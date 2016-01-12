#include "FileSystemThread.h"
#include "MirrorDrive.h"
#include "../Common/DokanDrive.h"

FileSystemThread::FileSystemThread(const QString &directory, const QString &letter)
    :m_directory(directory),
      m_letter(letter)
{

}

void FileSystemThread::run()
{
    MirrorDrive *mirrorDrive = new MirrorDrive(m_directory);
    DokanMirrorDriveInterface::mount(m_letter, mirrorDrive);
}


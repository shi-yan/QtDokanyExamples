#ifndef FILESYSTEMTHREAD_H
#define FILESYSTEMTHREAD_H

#include <QThread>
#include "NetworkDriveServer.h"

class FileSystemThread : public QThread
{
    NetworkDriveServer *m_server;
    const QString m_directory;
    const QString m_letter;
public:
    FileSystemThread(const QString &directory, const QString &letter, NetworkDriveServer *);


    void run() override;
};

#endif // FILESYSTEMTHREAD_H

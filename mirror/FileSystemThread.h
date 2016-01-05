#ifndef FILESYSTEMTHREAD_H
#define FILESYSTEMTHREAD_H

#include <QThread>

class FileSystemThread : public QThread
{
public:
    FileSystemThread();


    void run() override;
};

#endif // FILESYSTEMTHREAD_H

#ifndef FILESYSTEMTHREAD_H
#define FILESYSTEMTHREAD_H

#include <QThread>

class FileSystemThread : public QThread
{
    const QString m_directory;
    const QString m_letter;
public:
    FileSystemThread(const QString &directory, const QString &letter);
    void run() override;
};

#endif // FILESYSTEMTHREAD_H

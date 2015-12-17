#ifndef NETWORKDRIVESERVER_H
#define NETWORKDRIVESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include "NetworkDriveProtocol.h"
#include <QAtomicInteger>
#include <QSemaphore>

class NetworkDriveServer : public QTcpServer
{
private:
    QTcpSocket *m_socket;
    QByteArray m_dataBuffer;
    qint64 m_currentDataSize;

    QAtomicInteger<int> m_messageId;

    QHash<qint32, QSemaphore*> m_semaphoreTable;

public:
    NetworkDriveServer();

    void incomingConnection(qintptr handle) override;

    void remoteCreateFile(const QString &filePath, quint64 AccessMode, quint64 CreateDisposition, quint64 CreateOptions);

    void remoteCloseFile(const QString &filePath, quint64 DokanFileInfo_context);

    void remoteCleanUp(const QString &filePath, quint64 DokanFileInfo_context, bool DokanFileInfo_deleteOnClose);

    void remoteReadFile(const QString &filePath, quint64 BufferLength, quint64 ReadLength, quint64 Offset, quint64 DokanFileInfo_context);

    void remoteWriteFile(const QString &filePath, char *buffer, quint64 NumberOfBytesToWrite, quint64 Offset, quint64 DokanFileInfo_context);

    void remoteFlushFileBuffers(const QString &filePath, quint64 DokanFileInfo_context);

    void remoteGetFileInformation(const QString &filePath, bool DokanFileInfo_isDirectory);

    void remoteFindFiles(const QString &filePath);

    void remoteDeleteFile(const QString &filePath);

    void remoteDeleteDirectory(const QString &filePath);

    void remoteMoveFile(const QString &filePath, const QString &NewFileName, bool ReplaceIfExisting);

    void remoteLockFile(const QString &filePath, quint64 ByteOffset, quint64 Length, quint64 DokanFileInfo_context);

    void remoteSetEndOfFile(const QString &filePath, quint64 ByteOffset, quint64 DokanFileInfo_context);

    void remoteSetAllocationSize(const QString &filePath, quint64 AllocSize, quint64 DokanFileInfo_context);

    void remoteSetFileAttributes(const QString &filePath, quint64 FileAttributes, quint64 DokanFileInfo_context);

    void remoteSetFileTime(const QString &filePath, QDateTime &CreationTime, QDateTime &LastAccessTime, QDateTime &LastWriteTime, quint64 DokanFileInfo_context);

    void remoteUnlockFile(const QString &filePath, quint64 ByteOffset, quint64	Length, quint64	DokanFileInfo_context);

    void remoteGetFileSecurity(const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 BufferLength, quint64	LengthNeeded, quint64 DokanFileInfo_context);

    void remoteSetFileSecurity(const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 SecurityDescriptorLength, quint64 DokanFileInfo_context);

    void remoteGetVolumeInformation(const QString &VolumeNameBuffer, quint64 VolumeNameSize, quint64 VolumeSerialNumber, quint64	MaximumComponentLength, const QString &FileSystemFlags, const QString &FileSystemNameBuffer, quint64	FileSystemNameSize);

    void remoteUnmount();

    void remoteEnumerateNamedStreams(const QString &filePath, quint64 EnumContext, const QString &StreamName, quint64 StreamNameLength, quint64 StreamSize);


private slots:
    void onSocketReadyRead();
    void sendMessageToClient(QByteArray *);

private:
    void processData();
    void processMessage(QByteArray &message);
};

#endif // NETWORKDRIVESERVER_H

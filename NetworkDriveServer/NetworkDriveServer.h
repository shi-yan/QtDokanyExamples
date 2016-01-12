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
    Q_OBJECT

private:
    class RequestBarrier
    {
        QSemaphore m_semaphore;
        QByteArray m_data;
        qint32 m_messageId;
    public:
        RequestBarrier(qint32 messageId);
        ~RequestBarrier();

        void acquire();
        void release();
        void dumpData(const QByteArray &data);
        void getData(QByteArray &out);
    };

    QTcpSocket *m_socket;
    QByteArray m_dataBuffer;
    qint64 m_currentDataSize;

    QAtomicInteger<int> m_messageId;

    QHash<qint32, RequestBarrier*> m_semaphoreTable;

public:
    NetworkDriveServer();

    void incomingConnection(qintptr handle) override;

    void remoteCreateFile(QByteArray &result, const QString &filePath, quint64 AccessMode, quint64 CreateDisposition, quint64 CreateOptions);

    void remoteCloseFile(QByteArray &result,const QString &filePath, quint64 DokanFileInfo_context);

    void remoteCleanUp(QByteArray &result,const QString &filePath, quint64 DokanFileInfo_context, bool DokanFileInfo_deleteOnClose);

    void remoteReadFile(QByteArray &result,const QString &filePath, quint64 BufferLength, quint64 Offset, quint64 DokanFileInfo_context);

    void remoteWriteFile(QByteArray &result,const QString &filePath, char *buffer, quint64 NumberOfBytesToWrite, quint64 Offset, quint64 DokanFileInfo_context);

    void remoteFlushFileBuffers(QByteArray &result,const QString &filePath, quint64 DokanFileInfo_context);

    void remoteGetFileInformation(QByteArray &result,const QString &filePath, bool DokanFileInfo_isDirectory);

    void remoteFindFiles(QByteArray &result,const QString &filePath);

    void remoteDeleteFile(QByteArray &result,const QString &filePath);

    void remoteDeleteDirectory(QByteArray &result,const QString &filePath);

    void remoteMoveFile(QByteArray &result,const QString &filePath, const QString &NewFileName, bool ReplaceIfExisting);

    void remoteLockFile(QByteArray &result,const QString &filePath, quint64 ByteOffset, quint64 Length, quint64 DokanFileInfo_context);

    void remoteSetEndOfFile(QByteArray &result,const QString &filePath, quint64 ByteOffset, quint64 DokanFileInfo_context);

    void remoteSetAllocationSize(QByteArray &result,const QString &filePath, quint64 AllocSize, quint64 DokanFileInfo_context);

    void remoteSetFileAttributes(QByteArray &result,const QString &filePath, quint64 FileAttributes, quint64 DokanFileInfo_context);

    void remoteSetFileTime(QByteArray &result,const QString &filePath, QDateTime &CreationTime, QDateTime &LastAccessTime, QDateTime &LastWriteTime, quint64 DokanFileInfo_context);

    void remoteUnlockFile(QByteArray &result,const QString &filePath, quint64 ByteOffset, quint64	Length, quint64	DokanFileInfo_context);

    void remoteGetFileSecurity(QByteArray &result,const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 BufferLength, quint64	LengthNeeded, quint64 DokanFileInfo_context);

    void remoteSetFileSecurity(QByteArray &result,const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 SecurityDescriptorLength, quint64 DokanFileInfo_context);

    void remoteGetVolumeInformation(QByteArray &result,const QString &VolumeNameBuffer, quint64 VolumeNameSize, quint64 VolumeSerialNumber, quint64	MaximumComponentLength, const QString &FileSystemFlags, const QString &FileSystemNameBuffer, quint64	FileSystemNameSize);

    void remoteUnmount();

    void remoteEnumerateNamedStreams(QByteArray &result,const QString &filePath, quint64 EnumContext, const QString &StreamName, quint64 StreamNameLength, quint64 StreamSize);


private slots:
    void onSocketReadyRead();
    void onSendMessageToClient(QByteArray *);

private:
    void processData();
    void processMessage(QByteArray &message);

signals:
    void sendMessageToClient(QByteArray *);
};

#endif // NETWORKDRIVESERVER_H

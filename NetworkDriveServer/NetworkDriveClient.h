#ifndef NETWORKDRIVECLIENT_H
#define NETWORKDRIVECLIENT_H

#include <QTcpSocket>
#include <QByteArray>

class NetworkDriveClient : public QObject
{
    Q_OBJECT
private:
    QTcpSocket *m_socket;
    QByteArray m_dataBuffer;
    qint64 m_currentDataSize;

public:
    NetworkDriveClient();
    ~NetworkDriveClient();

    void connectToServer();
    void processData();
    void processMessage(QByteArray &message);

private slots:
    void onSocketReadyRead();

public:
    void clientCreateFile(QByteArray &reply, const QString &filePath, quint64 AccessMode, quint64 CreateDisposition, quint64 CreateOptions);

    void clientCloseFile(QByteArray &reply, const QString &filePath, quint64 DokanFileInfo_context);

    void clientCleanUp(QByteArray &reply, const QString &filePath, quint64 DokanFileInfo_context, bool DokanFileInfo_deleteOnClose);

    void clientReadFile(QByteArray &reply, const QString &filePath, quint64 BufferLength, quint64 Offset, quint64 DokanFileInfo_context);

    void clientWriteFile(QByteArray &reply, const QString &filePath, char *buffer, quint64 NumberOfBytesToWrite, quint64 Offset, quint64 DokanFileInfo_context);

    void clientFlushFileBuffers(QByteArray &reply, const QString &filePath, quint64 DokanFileInfo_context);

    void clientGetFileInformation(QByteArray &reply, const QString &filePath, bool DokanFileInfo_isDirectory);

    void clientFindFiles(QByteArray &reply, const QString &filePath);

    void clientDeleteFile(QByteArray &reply, const QString &filePath);

    void clientDeleteDirectory(QByteArray &reply, const QString &filePath);

    void clientMoveFile(QByteArray &reply, const QString &filePath, const QString &NewFileName, bool ReplaceIfExisting);

    void clientLockFile(QByteArray &reply, const QString &filePath, quint64 ByteOffset, quint64 Length, quint64 DokanFileInfo_context);

    void clientSetEndOfFile(QByteArray &reply, const QString &filePath, quint64 ByteOffset, quint64 DokanFileInfo_context);

    void clientSetAllocationSize(QByteArray &reply, const QString &filePath, quint64 AllocSize, quint64 DokanFileInfo_context);

    void clientSetFileAttributes(QByteArray &reply, const QString &filePath, quint64 FileAttributes, quint64 DokanFileInfo_context);

    void clientSetFileTime(QByteArray &reply, const QString &filePath, QDateTime &CreationTime, QDateTime &LastAccessTime, QDateTime &LastWriteTime, quint64 DokanFileInfo_context);

    void clientUnlockFile(QByteArray &reply, const QString &filePath, quint64 ByteOffset, quint64	Length, quint64	DokanFileInfo_context);

    void clientGetFileSecurity(QByteArray &reply, const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 BufferLength, quint64	LengthNeeded, quint64 DokanFileInfo_context);

    void clientSetFileSecurity(QByteArray &reply, const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 SecurityDescriptorLength, quint64 DokanFileInfo_context);

    void clientGetVolumeInformation(QByteArray &reply, const QString &VolumeNameBuffer, quint64 VolumeNameSize, quint64 VolumeSerialNumber, quint64	MaximumComponentLength, const QString &FileSystemFlags, const QString &FileSystemNameBuffer, quint64	FileSystemNameSize);

    void clientUnmount(QByteArray &reply);

    void clientEnumerateNamedStreams(QByteArray &reply, const QString &filePath, quint64 EnumContext, const QString &StreamName, quint64 StreamNameLength, quint64 StreamSize);



};

#endif // NETWORKDRIVECLIENT_H

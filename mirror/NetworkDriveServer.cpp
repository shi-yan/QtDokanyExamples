#include "NetworkDriveServer.h"
#include <QDataStream>

NetworkDriveServer::NetworkDriveServer()
    :QTcpServer(),
      m_socket(NULL),
      m_currentDataSize(-1)
{

}

void NetworkDriveServer::incomingConnection(qintptr handle)
{
    if (!m_socket)
    {
        m_socket = new QTcpSocket(this);

        m_socket->setSocketDescriptor(handle);
    }
}

void NetworkDriveServer::onSocketReadyRead()
{
    m_dataBuffer.append(m_socket->readAll());
}

void NetworkDriveServer::processData()
{
    while(1)
    {
        if (m_currentDataSize == -1)
        {
            if (m_dataBuffer.size() < sizeof(quint32))
            {
                return;
            }
            else
            {
                QDataStream stream(m_dataBuffer);
                quint32 size;
                stream >> size;
                m_currentDataSize = size;

                m_dataBuffer.remove(0, sizeof(quint32));
            }
        }

        if (m_currentDataSize != -1 && m_dataBuffer.size() >= m_currentDataSize)
        {
            QByteArray messageData(m_dataBuffer.data(), m_currentDataSize);
            m_dataBuffer.remove(0, m_currentDataSize);
            m_currentDataSize = -1;
            processMessage(messageData);
        }
        else
        {
            return;
        }
    }
}

void NetworkDriveServer::processMessage(QByteArray &message)
{
    QDataStream messageStream(message);

    quint32 messageType;

    messageStream >> messageType;

    switch(messageType)
    {
    case MOUNT:
        break;
    case UNMOUNT:
        break;
    case CREATE_FILE:
        break;
    }
}

void NetworkDriveServer::remoteCreateFile(const QString &filePath, quint64 AccessMode, quint64 CreateDisposition, quint64 CreateOptions)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = CREATE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId << filePath << AccessMode << CreateDisposition << CreateOptions;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteCloseFile(const QString &filePath, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = CLOSE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteCleanUp(const QString &filePath, quint64 DokanFileInfo_context, bool DokanFileInfo_deleteOnClose)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = CLEAN_UP_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_context << DokanFileInfo_deleteOnClose;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteReadFile(const QString &filePath, quint64 BufferLength, quint64 ReadLength, quint64 Offset, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = READ_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << BufferLength << ReadLength << Offset << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteWriteFile(const QString &filePath, char *buffer, quint64 NumberOfBytesToWrite, quint64 Offset, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = WRITE_FILE;
    qint32 currentMessageId = m_messageId++;

    QByteArray dataBuffer(buffer, NumberOfBytesToWrite);

    messageDataStream << messageType << currentMessageId <<filePath << NumberOfBytesToWrite << Offset << DokanFileInfo_context << dataBuffer;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteFlushFileBuffers(const QString &filePath, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = FLUSH_FILE_BUFFERS;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteGetFileInformation(const QString &filePath, bool DokanFileInfo_isDirectory)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = GET_FILE_INFORMATION;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_isDirectory;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteFindFiles(const QString &filePath)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = FIND_FILES;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteDeleteFile(const QString &filePath)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = DELETE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteDeleteDirectory(const QString &filePath)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = DELETE_DIRECTORY;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteMoveFile(const QString &filePath, const QString &NewFileName, bool ReplaceIfExisting)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = MOVE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << NewFileName << ReplaceIfExisting;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteLockFile(const QString &filePath, quint64 ByteOffset, quint64 Length, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = LOCK_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << ByteOffset << Length << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteSetEndOfFile(const QString &filePath, quint64 ByteOffset, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = SET_END_OF_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << ByteOffset << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteSetAllocationSize(const QString &filePath, quint64 AllocSize, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = SET_ALLOCATION_SIZE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << AllocSize << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteSetFileAttributes(const QString &filePath, quint64 FileAttributes, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = SET_FILE_ATTRIBUTES;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << FileAttributes << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteSetFileTime(const QString &filePath, QDateTime &CreationTime, QDateTime &LastAccessTime, QDateTime &LastWriteTime, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = SET_FILE_TIME;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << CreationTime << LastAccessTime <<LastWriteTime << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteUnlockFile(const QString &filePath, quint64 ByteOffset, quint64	Length, quint64	DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = UNLOCK_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << ByteOffset << Length << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteGetFileSecurity(const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 BufferLength, quint64	LengthNeeded, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = GET_FILE_SECURITY;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << SecurityInformation << SecurityDescriptor << BufferLength << LengthNeeded << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteSetFileSecurity(const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 SecurityDescriptorLength, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = SET_FILE_SECURITY;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << SecurityInformation << SecurityDescriptor<< SecurityDescriptorLength << DokanFileInfo_context;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteGetVolumeInformation(const QString &VolumeNameBuffer, quint64 VolumeNameSize, quint64 VolumeSerialNumber, quint64	MaximumComponentLength, const QString &FileSystemFlags, const QString &FileSystemNameBuffer, quint64	FileSystemNameSize)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = GET_VOLUME_INFORMATION;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<VolumeNameBuffer << VolumeNameSize << VolumeSerialNumber << MaximumComponentLength << FileSystemFlags << FileSystemNameBuffer << FileSystemNameSize;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteUnmount()
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = UNMOUNT;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::remoteEnumerateNamedStreams(const QString &filePath, quint64 EnumContext, const QString &StreamName, quint64 StreamNameLength, quint64 StreamSize)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(*messageArray);
    quint32 messageType = ENUMERATE_NAMED_STREAMS;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId << filePath << EnumContext << StreamName << StreamNameLength << StreamSize;

    QSemaphore *semaphore = new QSemaphore();
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();
}

void NetworkDriveServer::sendMessageToClient(QByteArray *){}

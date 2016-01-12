#include "NetworkDriveServer.h"
#include <QDataStream>

NetworkDriveServer::RequestBarrier::RequestBarrier(qint32 messageId)
    :m_messageId(messageId)
{

}

NetworkDriveServer::RequestBarrier::~RequestBarrier()
{

}

void NetworkDriveServer::RequestBarrier::acquire()
{
    m_semaphore.acquire();
}

void NetworkDriveServer::RequestBarrier::release()
{
    m_semaphore.release();
}

void NetworkDriveServer::RequestBarrier::dumpData(const QByteArray &data)
{
    quint32 offset = sizeof(quint32) + sizeof(qint32);
    m_data.append(data.data() + offset, data.size() - offset);
}

void NetworkDriveServer::RequestBarrier::getData(QByteArray &out)
{
    out.append(m_data);
}

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
        connect(this, SIGNAL(sendMessageToClient(QByteArray*)),this,SLOT(onSendMessageToClient(QByteArray*)), Qt::QueuedConnection);
        connect(m_socket, SIGNAL(readyRead()),this,SLOT(onSocketReadyRead()));
        m_socket->setSocketDescriptor(handle);
    }
}

void NetworkDriveServer::onSocketReadyRead()
{
    //qDebug() << "server received something";
    m_dataBuffer.append(m_socket->readAll());
    processData();
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

    quint32 messageType = 0;

    messageStream >> messageType;

    qint32 messageId = 0;

    messageStream >> messageId;

    //qDebug() << "server process message" << messageType << messageId;


    switch(messageType)
    {
    case MOUNT:
        break;
    case UNMOUNT:
        break;
    case CREATE_FILE:
        //qDebug() << "create file";
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case CLOSE_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case  CLEAN_UP_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case  READ_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case  WRITE_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case FLUSH_FILE_BUFFERS:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case GET_FILE_INFORMATION:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case FIND_FILES:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case DELETE_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case DELETE_DIRECTORY:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case MOVE_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case LOCK_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case SET_END_OF_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case SET_ALLOCATION_SIZE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case SET_FILE_ATTRIBUTES:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case SET_FILE_TIME:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case UNLOCK_FILE:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case GET_FILE_SECURITY:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case SET_FILE_SECURITY:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case GET_VOLUME_INFORMATION:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    case ENUMERATE_NAMED_STREAMS:
        m_semaphoreTable[messageId]->dumpData(message);
        m_semaphoreTable[messageId]->release();
        break;
    }
}

void NetworkDriveServer::remoteCreateFile(QByteArray &result,const QString &filePath, quint64 AccessMode, quint64 CreateDisposition, quint64 CreateOptions)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = CREATE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId << filePath << AccessMode << CreateDisposition << CreateOptions;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    //qDebug() << "-------- before the acquire";
    semaphore->acquire();

    //qDebug() << "-------- after the acquire";

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;

}

void NetworkDriveServer::remoteCloseFile(QByteArray &result,const QString &filePath, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = CLOSE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteCleanUp(QByteArray &result,const QString &filePath, quint64 DokanFileInfo_context, bool DokanFileInfo_deleteOnClose)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = CLEAN_UP_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_context << DokanFileInfo_deleteOnClose;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteReadFile(QByteArray &result,const QString &filePath, quint64 BufferLength, quint64 Offset, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = READ_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << BufferLength  << Offset << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteWriteFile(QByteArray &result,const QString &filePath, char *buffer, quint64 NumberOfBytesToWrite, quint64 Offset, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = WRITE_FILE;
    qint32 currentMessageId = m_messageId++;

    QByteArray dataBuffer(buffer, NumberOfBytesToWrite);

    messageDataStream << messageType << currentMessageId <<filePath << NumberOfBytesToWrite << Offset << DokanFileInfo_context << dataBuffer;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteFlushFileBuffers(QByteArray &result,const QString &filePath, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = FLUSH_FILE_BUFFERS;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteGetFileInformation(QByteArray &result,const QString &filePath, bool DokanFileInfo_isDirectory)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = GET_FILE_INFORMATION;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << DokanFileInfo_isDirectory;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteFindFiles(QByteArray &result,const QString &filePath)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = FIND_FILES;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteDeleteFile(QByteArray &result,const QString &filePath)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = DELETE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteDeleteDirectory(QByteArray &result,const QString &filePath)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = DELETE_DIRECTORY;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteMoveFile(QByteArray &result,const QString &filePath, const QString &NewFileName, bool ReplaceIfExisting)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = MOVE_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << NewFileName << ReplaceIfExisting;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteLockFile(QByteArray &result,const QString &filePath, quint64 ByteOffset, quint64 Length, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = LOCK_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << ByteOffset << Length << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteSetEndOfFile(QByteArray &result,const QString &filePath, quint64 ByteOffset, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = SET_END_OF_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << ByteOffset << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteSetAllocationSize(QByteArray &result,const QString &filePath, quint64 AllocSize, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = SET_ALLOCATION_SIZE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << AllocSize << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteSetFileAttributes(QByteArray &result,const QString &filePath, quint64 FileAttributes, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = SET_FILE_ATTRIBUTES;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << FileAttributes << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteSetFileTime(QByteArray &result,const QString &filePath, QDateTime &CreationTime, QDateTime &LastAccessTime, QDateTime &LastWriteTime, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = SET_FILE_TIME;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << CreationTime << LastAccessTime <<LastWriteTime << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteUnlockFile(QByteArray &result,const QString &filePath, quint64 ByteOffset, quint64	Length, quint64	DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = UNLOCK_FILE;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << ByteOffset << Length << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteGetFileSecurity(QByteArray &result,const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 BufferLength, quint64	LengthNeeded, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = GET_FILE_SECURITY;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << SecurityInformation << SecurityDescriptor << BufferLength << LengthNeeded << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteSetFileSecurity(QByteArray &result,const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 SecurityDescriptorLength, quint64 DokanFileInfo_context)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = SET_FILE_SECURITY;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<filePath << SecurityInformation << SecurityDescriptor<< SecurityDescriptorLength << DokanFileInfo_context;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteGetVolumeInformation(QByteArray &result,const QString &VolumeNameBuffer, quint64 VolumeNameSize, quint64 VolumeSerialNumber, quint64	MaximumComponentLength, const QString &FileSystemFlags, const QString &FileSystemNameBuffer, quint64	FileSystemNameSize)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = GET_VOLUME_INFORMATION;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId <<VolumeNameBuffer << VolumeNameSize << VolumeSerialNumber << MaximumComponentLength << FileSystemFlags << FileSystemNameBuffer << FileSystemNameSize;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteUnmount()
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = UNMOUNT;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

   // semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::remoteEnumerateNamedStreams(QByteArray &result,const QString &filePath, quint64 EnumContext, const QString &StreamName, quint64 StreamNameLength, quint64 StreamSize)
{
    QByteArray *messageArray = new QByteArray();
    QDataStream messageDataStream(messageArray, QIODevice::WriteOnly);
    quint32 messageType = ENUMERATE_NAMED_STREAMS;
    qint32 currentMessageId = m_messageId++;

    messageDataStream << messageType << currentMessageId << filePath << EnumContext << StreamName << StreamNameLength << StreamSize;

    RequestBarrier *semaphore = new RequestBarrier(currentMessageId);
    m_semaphoreTable[currentMessageId] = semaphore;


    emit sendMessageToClient(messageArray);
    semaphore->acquire();

    semaphore->getData(result);
    m_semaphoreTable.remove(currentMessageId);
    delete semaphore;
}

void NetworkDriveServer::onSendMessageToClient(QByteArray *data)
{
    if (m_socket)
    {
        QByteArray sizeData;
        QDataStream sds(&sizeData, QIODevice::WriteOnly);
        sds << (quint32) data->size();
        m_socket->write(sizeData);
        m_socket->write(*data);
    }

    if (data)
    {
        delete data;
    }
}

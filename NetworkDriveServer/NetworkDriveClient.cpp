#include "NetworkDriveClient.h"
#include <QHostAddress>
#include "NetworkDriveProtocol.h"
#include <QDataStream>
#include <QDateTime>
#include <QDir>
//#include <Windows.h>
#include "WindowsDefines.h"
//#include <winbase.h>
//#include <ntstatus.h>
#include <stdio.h>
#include <stdlib.h>
//#include "fileinfo.h"

const DWORD DataAccess = FILE_READ_DATA | FILE_WRITE_DATA | FILE_APPEND_DATA |
        FILE_EXECUTE | FILE_GENERIC_EXECUTE | FILE_GENERIC_WRITE | FILE_GENERIC_READ;

const DWORD DataWriteAccess = FILE_WRITE_DATA | FILE_APPEND_DATA | 65536 | FILE_GENERIC_WRITE;


NetworkDriveClient::NetworkDriveClient(const QString &directory, const QString &driveLetter)
    :m_socket(NULL),
      m_dataBuffer(),
      m_currentDataSize(-1),
      m_directory(directory),
      m_driveLetter(driveLetter)
{

}

NetworkDriveClient::~NetworkDriveClient()
{
    m_dataBuffer.clear();
}

void NetworkDriveClient::connectToServer(const QHostAddress &address, int port)
{
    if (!m_socket)
    {
        m_socket = new QTcpSocket(this);
        m_socket->connectToHost(address, port);
        connect(m_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
        connect(m_socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
    }
}

void NetworkDriveClient::onSocketReadyRead()
{
    m_dataBuffer.append(m_socket->readAll());
    processData();
}

void NetworkDriveClient::processData()
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
            //qDebug() << "datasize" << m_currentDataSize;
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

void NetworkDriveClient::processMessage(QByteArray &message)
{
    QDataStream messageDataStream(message);


    quint32 messageType;

    messageDataStream >> messageType;

    //qDebug() << "process message" << messageType;

    switch(messageType)
    {
    case MOUNT:
    {

    }
        break;

    case CREATE_FILE:
    {
        //qDebug() << "message is create file";
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 AccessMode = 0;
        quint64 CreateDisposition = 0;
        quint64 CreateOptions = 0;

        messageDataStream >> currentMessageId >> filePath >> AccessMode >> CreateDisposition >> CreateOptions;
        QByteArray resultArray;

        clientCreateFile(resultArray, filePath, AccessMode, CreateDisposition, CreateOptions);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);

            //qDebug() << "client side send stuff back";
            m_socket->write(resultArray);
        }
    }
        break;
    case CLOSE_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 context;

        messageDataStream >> currentMessageId >>filePath >> context;
        QByteArray resultArray;

        clientCloseFile(resultArray, filePath, context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case CLEAN_UP_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 DokanFileInfo_context;
        bool DokanFileInfo_deleteOnClose;

        messageDataStream >> currentMessageId >> filePath >> DokanFileInfo_context >> DokanFileInfo_deleteOnClose;
        QByteArray resultArray;

        clientCleanUp(resultArray, filePath, DokanFileInfo_context, DokanFileInfo_deleteOnClose);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case READ_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 BufferLength = 0;
        quint64 Offset = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> BufferLength  >> Offset >> DokanFileInfo_context;
        QByteArray resultArray;

        clientReadFile(resultArray, filePath, BufferLength, Offset, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case WRITE_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 NumberOfBytesToWrite = 0;
        quint64 Offset = 0;
        quint64 DokanFileInfo_context = 0;

        QByteArray dataBuffer;

        messageDataStream >> currentMessageId >> filePath >> NumberOfBytesToWrite >> Offset >> DokanFileInfo_context >> dataBuffer;

        QByteArray resultArray;

        clientWriteFile(resultArray, filePath, dataBuffer.data(), NumberOfBytesToWrite, Offset, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case FLUSH_FILE_BUFFERS:
    {
        qint32 currentMessageId = 0;
        quint64 DokanFileInfo_context = 0;
        QString filePath;

        messageDataStream >> currentMessageId >> filePath >> DokanFileInfo_context;

        QByteArray resultArray;

        clientFlushFileBuffers(resultArray, filePath, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case GET_FILE_INFORMATION:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        bool DokanFileInfo_isDirectory = false;

        messageDataStream >> currentMessageId >> filePath >> DokanFileInfo_isDirectory;

        QByteArray resultArray;

        clientGetFileInformation(resultArray, filePath, DokanFileInfo_isDirectory);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case FIND_FILES:
    {
        qint32 currentMessageId = 0;
        QString filePath;

        messageDataStream >> currentMessageId >> filePath;

        QByteArray resultArray;

        clientFindFiles(resultArray, filePath);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case DELETE_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;

        messageDataStream >> currentMessageId >> filePath;

        QByteArray resultArray;

        clientDeleteFile(resultArray, filePath);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case DELETE_DIRECTORY:
    {
        qint32 currentMessageId = 0;
        QString filePath;

        messageDataStream >> currentMessageId >> filePath;

        QByteArray resultArray;

        clientDeleteDirectory(resultArray, filePath);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case MOVE_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        QString NewFileName;
        bool ReplaceIfExisting = false;

        messageDataStream >> currentMessageId >> filePath >> NewFileName >> ReplaceIfExisting;

        QByteArray resultArray;

        clientMoveFile(resultArray, filePath, NewFileName, ReplaceIfExisting);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case LOCK_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 ByteOffset = 0;
        quint64 Length = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> ByteOffset >> Length >> DokanFileInfo_context;

        QByteArray resultArray;

        clientLockFile(resultArray, filePath, ByteOffset, Length, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case SET_END_OF_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 ByteOffset = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> ByteOffset >> DokanFileInfo_context;

        QByteArray resultArray;

        clientSetEndOfFile(resultArray, filePath, ByteOffset, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case SET_ALLOCATION_SIZE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 AllocSize = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> AllocSize >> DokanFileInfo_context;

        QByteArray resultArray;

        clientSetAllocationSize(resultArray, filePath, AllocSize, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case SET_FILE_ATTRIBUTES:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 FileAttributes = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> FileAttributes >> DokanFileInfo_context;

        QByteArray resultArray;

        clientSetFileAttributes(resultArray, filePath, FileAttributes, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case SET_FILE_TIME:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        QDateTime CreationTime;
        QDateTime LastAccessTime;
        QDateTime LastWriteTime;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> CreationTime >> LastAccessTime >> LastWriteTime >> DokanFileInfo_context;

        QByteArray resultArray;

        clientSetFileTime(resultArray, filePath, CreationTime, LastAccessTime, LastWriteTime, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case UNLOCK_FILE:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 ByteOffset = 0;
        quint64 Length = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> ByteOffset >> Length >> DokanFileInfo_context;

        QByteArray resultArray;

        clientUnlockFile(resultArray, filePath, ByteOffset, Length, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case GET_FILE_SECURITY:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 SecurityInformation = 0;
        quint64 SecurityDescriptor = 0;
        quint64 BufferLength = 0;
        quint64 LengthNeeded = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath >> SecurityInformation >> SecurityDescriptor >> BufferLength >> LengthNeeded >> DokanFileInfo_context;

        QByteArray resultArray;

        clientGetFileSecurity(resultArray, filePath, SecurityInformation, SecurityDescriptor, BufferLength, LengthNeeded, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case SET_FILE_SECURITY:
    {
        qint32 currentMessageId = 0;
        QString filePath;
        quint64 SecurityInformation = 0;
        quint64 SecurityDescriptor = 0;
        quint64 SecurityDescriptorLength = 0;
        quint64 DokanFileInfo_context = 0;

        messageDataStream >> currentMessageId >> filePath << SecurityInformation << SecurityDescriptor << SecurityDescriptorLength << DokanFileInfo_context;

        QByteArray resultArray;

        clientSetFileSecurity(resultArray, filePath, SecurityInformation, SecurityDescriptor, SecurityDescriptorLength, DokanFileInfo_context);

        if (m_socket)
        {
            QByteArray sizeData;
            QDataStream sds(&sizeData, QIODevice::WriteOnly);
            sds << (quint32) (resultArray.size() + sizeof(quint32) + sizeof(qint32));
            sds << messageType;
            sds << currentMessageId;

            m_socket->write(sizeData);
            m_socket->write(resultArray);
        }
    }
        break;
    case GET_VOLUME_INFORMATION:
    {
        qint32 currentMessageId = 0;
        QString VolumeNameBuffer;
        quint64 VolumeNameSize = 0;
        quint64 VolumeSerialNumber = 0;
        quint64 MaximumComponentLength = 0;
        quint64 FileSystemFlags = 0;
        quint64 FileSystemNameBuffer = 0;
        quint64 FileSystemNameSize = 0;


        messageDataStream  >> currentMessageId >> VolumeNameBuffer >> VolumeNameSize >> VolumeSerialNumber >> MaximumComponentLength >> FileSystemFlags >> FileSystemNameBuffer >> FileSystemNameSize;

        QByteArray resultArray;

      //  clientGetVolumeInformation(resultArray, VolumeNameBuffer, VolumeNameSize, VolumeSerialNumber, MaximumComponentLength, FileSystemFlags, FileSystemNameBuffer, FileSystemNameSize);

       /* if (m_socket)
        {
            m_socket->write(resultArray);
        }*/
    }
        break;
    case UNMOUNT:
    {
        qint32 currentMessageId = 0;

        messageDataStream >> currentMessageId;

        QByteArray resultArray;

        clientUnmount(resultArray);
    }
        break;
    case ENUMERATE_NAMED_STREAMS:
    {

        qint32 currentMessageId = 0;
        QString filePath;
        quint64 EnumContext = 0;
        QString StreamName;
        quint64 StreamNameLength = 0;
        quint64 StreamSize = 0;


        messageDataStream >> currentMessageId >> filePath >> EnumContext >> StreamName >> StreamNameLength >> StreamSize;

        QByteArray resultArray;

        clientEnumerateNamedStreams(resultArray, filePath, EnumContext, StreamName, StreamNameLength, StreamSize);
    }
        break;
    }
}

void NetworkDriveClient::clientCreateFile(QByteArray &reply, const QString &fileName, quint64 AccessMode, quint64 CreateDisposition, quint64 CreateOptions)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);
    //qDebug() << "inside create file" << fileName;
    //qDebug() << "beginning"   << CreateDisposition;

    bool DokanFileInfo_isDirectory = false;
    QDir dir(m_directory);

    QString filePath = dir.absoluteFilePath(fileName);

    //qDebug() << "create file ===================" << filePath;

    dir.setPath(filePath);
    QFileInfo fi(filePath);
    //qDebug() << "open" << filePath << "is dir?" << fi.isDir();
    if(((CreateOptions & FILE_DIRECTORY_FILE) == FILE_DIRECTORY_FILE) || fi.isDir())
    {
        //qDebug() << "open as dir";
        DokanFileInfo_isDirectory = true;
        if(CreateDisposition == FILE_CREATE)
        {
            //qDebug() << "create directory";

            dir.setPath(filePath);
            if(dir.exists())
            {
                replyDataStream << (quint64) ERROR_FILE_EXISTS;
                return;
            }

            if(!dir.mkpath(filePath))
            {
                replyDataStream << (quint64) ERROR_ACCESS_DENIED;
                return;
            }
            replyDataStream << (quint64) STATUS_SUCCESS << quint64(0)<< DokanFileInfo_isDirectory;

            return;

        }
        else if(CreateDisposition == FILE_OPEN_IF)
        {
            //qDebug() << "open directory" ;

            dir.setPath(filePath);
            if(!dir.exists())
            {
                replyDataStream << (quint64) STATUS_OBJECT_PATH_NOT_FOUND;

                return;
            }

            replyDataStream << (quint64) STATUS_SUCCESS << quint64(0) << DokanFileInfo_isDirectory;

            return ;
        }
    }
    else
    {
        //qDebug() << "open as file" << filePath;
        bool pathExists = true;
        bool pathIsDirectory = false;

        bool readWriteAttribute = (AccessMode & DataAccess) == 0;
        bool readAccess = (AccessMode & DataWriteAccess) == 0;

        QFileInfo fileInfo(filePath);

        pathExists = fileInfo.exists();
        pathIsDirectory = fileInfo.isDir();

        QFile::OpenMode openMode = readAccess ? QFile::ReadOnly : QFile::ReadWrite;

        if (AccessMode & FILE_APPEND_DATA)
            openMode |= QFile::Append;

        bool DokanFileInfo_IsDirectory = false;
        //qDebug() << "debug path exists" << filePath << pathExists;
        switch(CreateDisposition)
        {
        case OPEN_EXISTING:
            if (pathExists)
            {
                if (readWriteAttribute || pathIsDirectory)
                {
                   DokanFileInfo_IsDirectory = pathIsDirectory;
                    //qDebug() << "set dir" << pathIsDirectory;
                    quint64 DokanFileInfo_Context = 0;
                    //qDebug() << "debug 2";

                    replyDataStream << (quint64) STATUS_SUCCESS << DokanFileInfo_Context << DokanFileInfo_IsDirectory;
                    return;
                }
            }
            else
            {
                //qDebug() << "return object name not found";
                if (pathIsDirectory)
                {
                    //qDebug() << "return path not found";
                    replyDataStream << (quint64) STATUS_OBJECT_PATH_NOT_FOUND;
                    return;
                }
                else
                {
                    //qDebug() << "return file not found";
                    replyDataStream << (quint64) STATUS_OBJECT_NAME_NOT_FOUND;
                    return;
                }
            }
            break;

        case CREATE_NEW:
            if (pathExists)
            {//qDebug() << "debug 2";

                replyDataStream << (quint64) ERROR_FILE_EXISTS;
                return;
            }
            break;

        case TRUNCATE_EXISTING:
            openMode |= QFile::Truncate;
            if (!pathExists)
            {
                //qDebug() << "debug 3";

                replyDataStream << (quint64) ERROR_FILE_NOT_FOUND;
                return;
            }
            break;
        case OPEN_ALWAYS:
            break;
        case CREATE_ALWAYS:
            break;
        default:
            break;
        }

        //qDebug() << "trying to open this location:" << filePath;

        QFile *file = new QFile(filePath);

        if (file)
        {
            quint64 DokanFileInfo_Context = (ULONG64) file;
            if(file->open(openMode))
            {
                //qDebug() << "success" << openMode;
                replyDataStream << (quint64) STATUS_SUCCESS << DokanFileInfo_Context << DokanFileInfo_isDirectory;

                return;
            }
            else
            {
                replyDataStream << (quint64) STATUS_OBJECT_NAME_NOT_FOUND;
                return;
            }
        }
        else
        {
            replyDataStream << (quint64) STATUS_OBJECT_NAME_NOT_FOUND;
            return;
        }
    }
    replyDataStream << (quint64) STATUS_SUCCESS << quint64(0) << DokanFileInfo_isDirectory;
}

void NetworkDriveClient::clientCloseFile(QByteArray &reply, const QString &filePath, quint64 DokanFileInfo_context)
{
    if (DokanFileInfo_context)
    {
        QFile *file = (QFile*)DokanFileInfo_context;
        //qDebug() << "close" << file->fileName();
        file->close();
        DokanFileInfo_context = NULL;
    }
    else
    {
        //qDebug() << "------ no context defined to close" << filePath;
    }
}

void NetworkDriveClient::clientCleanUp(QByteArray &reply, const QString &fileName, quint64 DokanFileInfo_context, bool DokanFileInfo_deleteOnClose)
{
    if (DokanFileInfo_context)
    {
        QFile *file = (QFile*)DokanFileInfo_context;
        file->close();
        DokanFileInfo_context = 0;
    }

    QDir dir(m_directory);

    QString filePath = dir.absoluteFilePath(fileName);

    //qDebug() << "cleanup" << filePath;

    if (DokanFileInfo_deleteOnClose)
    {
        //qDebug() << "also delete";
        dir.remove(filePath);
    }
    return ;
}

void NetworkDriveClient::clientReadFile(QByteArray &reply, const QString &fileName, quint64 BufferLength, quint64 Offset, quint64 DokanFileInfo_context)
{
    //qDebug() << "########################## read file";


    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);
    replyDataStream << (quint64) STATUS_SUCCESS;

    QByteArray buffer(BufferLength, Qt::Uninitialized);
    if (DokanFileInfo_context)
    {
        QFile *file = (QFile*)DokanFileInfo_context;
        qDebug() << "normal read" << file->fileName();
        file->seek(Offset);
        quint64 ReadLength = file->read((char*)buffer.data(), BufferLength);
        replyDataStream << ReadLength;
        replyDataStream << QByteArray(buffer.data(), ReadLength);
    }
    else
    {

        QDir dir(m_directory);

        QString filePath = dir.absoluteFilePath(fileName);

        qDebug() << "memory mapped read" << filePath << Offset;
        QFile file(filePath);
        file.open(QFile::ReadOnly);
        file.seek(Offset);
        quint64 ReadLength = file.read((char*)buffer.data(), BufferLength);
        replyDataStream << ReadLength;
        replyDataStream << QByteArray(buffer.data(), ReadLength);
        qDebug() << "client size buffer" << buffer.size();
        file.close();
    }
}

void NetworkDriveClient::clientWriteFile(QByteArray &reply, const QString &fileName, char *buffer, quint64 NumberOfBytesToWrite, quint64 Offset, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);
    replyDataStream << (quint64) STATUS_SUCCESS;

    if (DokanFileInfo_context)
    {
        QFile *file = (QFile*)DokanFileInfo_context;
        //qDebug() << "normal read" << file->fileName();
        file->seek(Offset);
        quint64 NumberOfBytesWritten = file->write((char*)buffer, NumberOfBytesToWrite);
        replyDataStream << NumberOfBytesWritten;
    }
    else
    {
        QDir dir(m_directory);

        QString filePath = dir.absoluteFilePath(fileName);

        //qDebug() << "memory mapped read" << filePath;
        QFile file(filePath);
        file.open(QFile::WriteOnly);
        file.seek(Offset);
        quint64 NumberOfBytesWritten = file.write((char*)buffer, NumberOfBytesToWrite);
        replyDataStream << NumberOfBytesWritten;
        file.close();
    }
}

void NetworkDriveClient::clientFlushFileBuffers(QByteArray &reply, const QString &filePath, quint64 DokanFileInfo_context)
{
    if (DokanFileInfo_context)
    {
        QFile *file = (QFile*)DokanFileInfo_context;
        //qDebug() << "flush file" << file->fileName();

        file->flush();
    }
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);
    replyDataStream << (quint64) STATUS_SUCCESS;
}

static FILETIME toWinFileTime(const QDateTime &dateTime)
{
    // Definition of FILETIME from MSDN:
    // Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
    QDateTime origin(QDate(1601, 1, 1), QTime(0, 0, 0, 0), Qt::UTC);
    // Get offset - note we need 100-nanosecond intervals, hence we multiply by
    // 10000.
    qint64 _100nanosecs = 10000 * origin.msecsTo(dateTime);
    // Pack _100nanosecs into the structure.
    FILETIME fileTime;
    fileTime.dwLowDateTime = _100nanosecs;
    fileTime.dwHighDateTime = (_100nanosecs >> 32);
    return fileTime;
}

void NetworkDriveClient::clientGetFileInformation(QByteArray &reply, const QString &fileName, bool DokanFileInfo_isDirectory)
{
    QDir dir(m_directory);

    QString filePath = dir.absoluteFilePath(fileName);

    //qDebug() << "get file info ===================" << filePath;


    QFileInfo fileInfo(filePath);

    BY_HANDLE_FILE_INFORMATION HandleFileInformation;

    HandleFileInformation.ftCreationTime = toWinFileTime(fileInfo.created());
    HandleFileInformation.ftLastAccessTime = toWinFileTime(fileInfo.lastRead());
    HandleFileInformation.ftLastWriteTime = toWinFileTime(fileInfo.lastModified());
    HandleFileInformation.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
    if (fileInfo.isDir())
        HandleFileInformation.dwFileAttributes =0 ;
    if (fileInfo.isHidden())
    {
        HandleFileInformation.dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
    }

    quint64 size = fileInfo.size();

    HandleFileInformation.nFileSizeLow = size & 0xffffffff;
    HandleFileInformation.nFileSizeHigh = size >> 32;

    if( DokanFileInfo_isDirectory)
    {
        HandleFileInformation.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        HandleFileInformation.nFileSizeLow = 4096;
    }

    HandleFileInformation.dwVolumeSerialNumber = 0x19831116;

    HandleFileInformation.nNumberOfLinks = 1;
    HandleFileInformation.nFileIndexHigh = 0;
    HandleFileInformation.nFileIndexLow = 0;

    //qDebug() << "size low" << size<<  HandleFileInformation.nFileSizeLow;
    //qDebug() << "size high" << size<<HandleFileInformation.nFileSizeHigh;

    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);
    replyDataStream << (quint64) STATUS_SUCCESS << QByteArray((char*)&HandleFileInformation, sizeof(HandleFileInformation));


}

void NetworkDriveClient::clientFindFiles(QByteArray &reply, const QString &fileName)
{
    QDir dir(m_directory);

    QString filePath = dir.absoluteFilePath(fileName);

    //qDebug() << "findfiles ===================" << filePath;

    dir.setPath(filePath);

    QFileInfoList infoList = dir.entryInfoList();
    //qDebug() << "findfiles ===================" << filePath << infoList.size();

    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);
    replyDataStream << (quint64) STATUS_SUCCESS;

    replyDataStream << (quint32) infoList.size();

    for(int i = 0;i<infoList.size();++i)
    {
        WIN32_FIND_DATAW	findData;

        memset(&findData, 0, sizeof(findData));

        findData.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        if (infoList[i].isDir())
            findData.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        if (infoList[i].isHidden())
        {
            findData.dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
        }

        findData.ftCreationTime = toWinFileTime(infoList[i].created());
        findData.ftLastAccessTime = toWinFileTime(infoList[i].lastRead());
        findData.ftLastWriteTime = toWinFileTime(infoList[i].lastModified());

        quint64 size = infoList[i].size();

        findData.nFileSizeLow = size & 0xffffffff;
        findData.nFileSizeHigh = size >> 32;

        std::wstring stdfilename = infoList[i].fileName().toStdWString();
        std::u16string wfilename(stdfilename.begin(), stdfilename.end());
        std::copy(wfilename.c_str(), wfilename.c_str() +wfilename.size()+1, findData.cFileName);

        replyDataStream << QByteArray((char*)&findData, sizeof(findData));

       // FillFindData(&findData, DokanFileInfo);
    }

}

void NetworkDriveClient::clientDeleteFile(QByteArray &reply, const QString &filePath)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);
    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientDeleteDirectory(QByteArray &reply, const QString &fileName)
{

    QDir dir(m_directory);

    QString filePath = dir.absoluteFilePath(fileName);

    //qDebug() << "create folder" << filePath;

    dir.setPath(filePath);

    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    if (dir.entryInfoList().empty())
    {
        replyDataStream << (quint64) STATUS_SUCCESS;
    }
    else
    {
        replyDataStream << (quint64) ERROR_DIR_NOT_EMPTY;
    }
}

void NetworkDriveClient::clientMoveFile(QByteArray &reply, const QString &filePath, const QString &NewFileName, bool ReplaceIfExisting)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    QString oldFileName = filePath;
    oldFileName = oldFileName.right(oldFileName.size() - 1);

    QDir olddir(m_directory);

    QString oldFilePath = olddir.absoluteFilePath(oldFileName);

    //qDebug() << "move folder" << oldFilePath;


    QString newFileName = (NewFileName);
    newFileName = newFileName.right(newFileName.size() - 1);

    QDir newdir(m_directory);

    QString newFilePath = newdir.absoluteFilePath(newFileName);

    //qDebug() << "move folder" << newFilePath;


    QFileInfo info(newFilePath);
    bool exist = info.exists();

    if (!exist)
    {
      //  DokanFileInfo_Context = NULL;
        olddir.rename(oldFilePath, newFileName);
        replyDataStream << (quint64) STATUS_SUCCESS;
        return;
    }
    else if(ReplaceIfExisting)
    {
      //  DokanFileInfo->Context = NULL;
        newdir.remove(oldFilePath);
        olddir.rename(oldFilePath, newFileName);
        replyDataStream << (quint64) STATUS_SUCCESS;
        return;
    }

    replyDataStream << (quint64) ERROR_FILE_EXISTS;
    return;
}

void NetworkDriveClient::clientLockFile(QByteArray &reply, const QString &filePath, quint64 ByteOffset, quint64 Length, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientSetEndOfFile(QByteArray &reply, const QString &filePath, quint64 ByteOffset, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientSetAllocationSize(QByteArray &reply, const QString &filePath, quint64 AllocSize, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientSetFileAttributes(QByteArray &reply, const QString &filePath, quint64 FileAttributes, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientSetFileTime(QByteArray &reply, const QString &filePath, QDateTime &CreationTime, QDateTime &LastAccessTime, QDateTime &LastWriteTime, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientUnlockFile(QByteArray &reply, const QString &filePath, quint64 ByteOffset, quint64	Length, quint64	DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientGetFileSecurity(QByteArray &reply, const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 BufferLength, quint64	LengthNeeded, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientSetFileSecurity(QByteArray &reply, const QString &filePath, quint64 SecurityInformation, quint64 SecurityDescriptor, quint64 SecurityDescriptorLength, quint64 DokanFileInfo_context)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientGetVolumeInformation(QByteArray &reply, const QString &VolumeNameBuffer, quint64 VolumeNameSize, quint64 VolumeSerialNumber, quint64	MaximumComponentLength, const QString &FileSystemFlags, const QString &FileSystemNameBuffer, quint64	FileSystemNameSize)
{
    /*wcscpy_s(VolumeNameBuffer, VolumeNameSize, L"DOKAN");
    *VolumeSerialNumber = 0x19831116;
    *MaximumComponentLength = 256;
    *FileSystemFlags = FILE_CASE_SENSITIVE_SEARCH |
                        FILE_CASE_PRESERVED_NAMES |
                        FILE_SUPPORTS_REMOTE_STORAGE |
                        FILE_UNICODE_ON_DISK |
                        FILE_PERSISTENT_ACLS;

    wcscpy_s(FileSystemNameBuffer, FileSystemNameSize, L"Dokan");
*/
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientUnmount(QByteArray &reply)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::clientEnumerateNamedStreams(QByteArray &reply, const QString &filePath, quint64 EnumContext, const QString &StreamName, quint64 StreamNameLength, quint64 StreamSize)
{
    QDataStream replyDataStream(&reply, QIODevice::WriteOnly);

    replyDataStream << (quint64) STATUS_SUCCESS;
}

void NetworkDriveClient::onSocketConnected()
{
    qDebug() << "mount";
    quint32 messageType = MOUNT;
    qint32 currentMessageId = 0;

    QByteArray mountArray;
    QDataStream mstream(&mountArray, QIODevice::WriteOnly);
    mstream << m_directory << m_driveLetter;

    QByteArray sizeData;
    QDataStream sds(&sizeData, QIODevice::WriteOnly);
    sds << (quint32) (mountArray.size() + sizeof(quint32) + sizeof(qint32));
    sds << messageType;
    sds << currentMessageId;

    m_socket->write(sizeData);
    m_socket->write(mountArray);
}

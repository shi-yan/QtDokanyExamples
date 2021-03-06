#include "NetworkDrive.h"
#include <QDataStream>
#include <QFileInfo>

NetworkDrive::NetworkDrive(const QString &directory, NetworkDriveServer *server)
    :DokanDriveImplementation(directory)
{
    //m_client = new NetworkDriveClient();
    m_server = server;
    //m_server->listen(QHostAddress::LocalHost, 12345);
}

NetworkDrive::~NetworkDrive()
{

}

NTSTATUS NetworkDrive::MirrorCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext, ACCESS_MASK AccessMode, ULONG FileAttributes, ULONG ShareMode, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray replyArray;
    QString fileName = getFileName(FileName);

    //qDebug() << "before calling client createfile" << fileName << CreateDisposition;

    m_server->remoteCreateFile(replyArray, fileName, AccessMode, CreateDisposition, CreateOptions);

    quint64 result;

    QDataStream resultStream(replyArray);

    resultStream >> result;

    if (result == STATUS_SUCCESS)
    {
        quint64 context;
        resultStream >> context;
        DokanFileInfo->Context =  context;
        bool isDirectory;

        resultStream >> isDirectory;
        DokanFileInfo->IsDirectory = isDirectory;
    }

    return (NTSTATUS)result;
}

void NetworkDrive::MirrorCloseFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray replyArray;
    QString fileName = getFileName(FileName);

    //m_client->clientCloseFile(replyArray, fileName, (quint64) DokanFileInfo->Context);
    m_server->remoteCloseFile(replyArray, fileName, (quint64) DokanFileInfo->Context);
    DokanFileInfo->Context = NULL;

}

void NetworkDrive::MirrorCleanup(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray replyArray;
    QString fileName = getFileName(FileName);

    //m_client->clientCleanUp(replyArray, fileName, (quint64) DokanFileInfo->Context, (bool) DokanFileInfo->DeleteOnClose);

    m_server->remoteCleanUp(replyArray, fileName, (quint64) DokanFileInfo->Context, (bool) DokanFileInfo->DeleteOnClose);
    DokanFileInfo->Context = NULL;
}

NTSTATUS NetworkDrive::MirrorReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray replyArray;
    QString fileName = getFileName(FileName);

    //m_client->clientReadFile(replyArray, fileName, (quint64) BufferLength, (quint64) Offset, (quint64) DokanFileInfo->Context );

    m_server->remoteReadFile(replyArray, fileName, (quint64) BufferLength, (quint64) Offset, (quint64) DokanFileInfo->Context);

    quint64 result;

    QDataStream resultStream(replyArray);

    resultStream >> result;

    quint64 readLength;

    resultStream >> readLength;

    *ReadLength = readLength;

    QByteArray buffer;

    resultStream >> buffer;

    qDebug() << "read buffer" << buffer.size() << readLength;

    memcpy(Buffer, buffer.data(), readLength);

    return STATUS_SUCCESS;
}

NTSTATUS NetworkDrive::MirrorWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD	NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray replyArray;

    QString fileName = getFileName(FileName);

    //m_client->clientWriteFile(replyArray, fileName, (char*)Buffer, (quint64) NumberOfBytesToWrite, (quint64) Offset, (quint64) DokanFileInfo->Context);

    m_server->remoteWriteFile(replyArray, fileName, (char*) Buffer, (quint64) NumberOfBytesToWrite, (quint64) Offset, (quint64) DokanFileInfo->Context);

    quint64 result;

    QDataStream resultStream(replyArray);

    resultStream >> result;

    quint64 writeLength;

    resultStream >> writeLength;

    *NumberOfBytesWritten = writeLength;

    return STATUS_SUCCESS;

}

NTSTATUS NetworkDrive::MirrorFlushFileBuffers(LPCWSTR	FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;
    QString fileName = getFileName(FileName);
    //m_client->clientFlushFileBuffers(resultArray, fileName, (quint64) DokanFileInfo->Context);

    m_server->remoteFlushFileBuffers(resultArray, fileName, (quint64) DokanFileInfo->Context);

    QDataStream resultStream(resultArray);
    quint64 result;

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorGetFileInformation(LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;
    QString fileName = getFileName(FileName);

    //m_client->clientGetFileInformation(resultArray, fileName, DokanFileInfo->IsDirectory);

    m_server->remoteGetFileInformation(resultArray, fileName, DokanFileInfo->IsDirectory);

    quint64 result;
    QDataStream resultStream(resultArray);

    resultStream >> result;

    quint64 dwFileAttributes;
    QDateTime ftCreationTime;
    QDateTime ftLastAccessTime;
    QDateTime ftLastWriteTime;
    quint64 size;
    quint64 dwVolumeSerialNumber;
    quint64 nNumberOfLinks;

    resultStream    >> dwFileAttributes
                    >> ftCreationTime
                    >> ftLastAccessTime
                    >> ftLastWriteTime
                    >> size
                    >> dwVolumeSerialNumber
                    >> nNumberOfLinks;

    BY_HANDLE_FILE_INFORMATION handleFileInformation;

    memset(&handleFileInformation, 0, sizeof(handleFileInformation));

    HandleFileInformation->ftCreationTime = toWinFileTime(ftCreationTime);
    HandleFileInformation->ftLastAccessTime =toWinFileTime(ftLastAccessTime);
    HandleFileInformation->ftLastWriteTime = toWinFileTime(ftLastWriteTime);
    HandleFileInformation->dwFileAttributes = dwFileAttributes;
    HandleFileInformation->nFileSizeLow = size & 0xffffffff;
    HandleFileInformation->nFileSizeHigh = size >> 32;
    HandleFileInformation->dwVolumeSerialNumber = dwVolumeSerialNumber;
    HandleFileInformation->nNumberOfLinks = nNumberOfLinks;
    HandleFileInformation->nFileIndexHigh = 0;
    HandleFileInformation->nFileIndexLow = 0;

    //qDebug() << HandleFileInformation->nFileSizeLow << HandleFileInformation->nFileSizeHigh;

    return result;
}

NTSTATUS NetworkDrive::MirrorFindFiles(LPCWSTR FileName, PFillFindData FillFindData, PDOKAN_FILE_INFO	DokanFileInfo)
{
    QByteArray resultArray;
    QString fileName = getFileName(FileName);


    //m_client->clientFindFiles(resultArray, fileName);

    m_server->remoteFindFiles(resultArray, fileName);

    quint64 result;
    QDataStream resultStream(resultArray);

    resultStream >> result;

    quint32 dataCount;

    resultStream >> dataCount;

    //qDebug() << "find how many files?" << dataCount;

    for(int i = 0;i<dataCount;++i)
    {

        quint64 dwFileAttributes;


        QDateTime ftCreationTime ;
        QDateTime ftLastAccessTime ;
        QDateTime ftLastWriteTime ;

        quint64 size ;
        QString filename;

       // findData.nFileSizeLow = size & 0xffffffff;
       // findData.nFileSizeHigh = size >> 32;


        resultStream >> dwFileAttributes
                        >> ftCreationTime
                        >> ftLastAccessTime
                        >> ftLastWriteTime
                        >> size
                        >> filename;


        WIN32_FIND_DATAW	findData;

        memset(&findData, 0, sizeof(findData));

        findData.dwFileAttributes = dwFileAttributes;
        findData.ftCreationTime = toWinFileTime(ftCreationTime);
        findData.ftLastAccessTime = toWinFileTime(ftLastAccessTime);
        findData.ftLastWriteTime = toWinFileTime(ftLastWriteTime);
        findData.nFileSizeLow = size & 0xffffffff;
        findData.nFileSizeHigh = size >> 32;

        std::wstring stdfilename = filename.toStdWString();
        std::u16string wfilename(stdfilename.begin(), stdfilename.end());
        std::copy(wfilename.c_str(), wfilename.c_str() +wfilename.size()+1, findData.cFileName);

        FillFindData(&findData, DokanFileInfo);

    }

    return result;
}

NTSTATUS NetworkDrive::MirrorDeleteFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);

    //m_client->clientDeleteFile(resultArray, fileName);
    m_server->remoteDeleteFile(resultArray, fileName);

    quint64 result;
    QDataStream resultStream(resultArray);

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorDeleteDirectory(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);

    //m_client->clientDeleteDirectory(resultArray, fileName);

    m_server->remoteDeleteDirectory(resultArray, fileName);

    quint64 result;

    QDataStream resultStream(resultArray);

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorMoveFile(LPCWSTR	FileName, LPCWSTR NewFileName, BOOL ReplaceIfExisting, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);
    QString newFileName = getFileName(NewFileName);

    //m_client->clientMoveFile(resultArray, fileName, newFileName, ReplaceIfExisting);

    m_server->remoteMoveFile(resultArray, fileName, newFileName, ReplaceIfExisting);

    QDataStream resultStream(resultArray);

    quint64 result;

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorLockFile(LPCWSTR	FileName, LONGLONG ByteOffset, LONGLONG Length, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);

    //m_client->clientLockFile(resultArray, fileName, ByteOffset, Length, DokanFileInfo->Context);

    m_server->remoteLockFile(resultArray, fileName, ByteOffset, Length, DokanFileInfo->Context);

    QDataStream resultStream(resultArray);

    quint64 result;

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorSetEndOfFile(LPCWSTR	FileName, LONGLONG ByteOffset, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;
    QString fileName = getFileName(FileName);

    //m_client->clientSetEndOfFile(resultArray, fileName, ByteOffset, DokanFileInfo->Context);

    m_server->remoteSetEndOfFile(resultArray, fileName, ByteOffset, DokanFileInfo->Context);

    QDataStream resultStream(resultArray);

    quint64 result;
    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorSetAllocationSize(LPCWSTR FileName, LONGLONG	AllocSize, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);

    //m_client->clientSetAllocationSize(resultArray, fileName, AllocSize, DokanFileInfo->Context);

    m_server->remoteSetAllocationSize(resultArray, fileName, AllocSize, DokanFileInfo->Context);
    QDataStream resultStream(resultArray);

    quint64 result;

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorSetFileAttributes(LPCWSTR FileName, DWORD FileAttributes, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);

    //m_client->clientSetFileAttributes(resultArray, fileName, FileAttributes, DokanFileInfo->Context);

    m_server->remoteSetFileAttributes(resultArray, fileName, FileAttributes, DokanFileInfo->Context);

    QDataStream resultStream(resultArray);

    quint64 result;
    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorSetFileTime(LPCWSTR FileName, CONST FILETIME* CreationTime, CONST FILETIME* LastAccessTime, CONST FILETIME* LastWriteTime, PDOKAN_FILE_INFO DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);

    //m_client->clientSetFileTime(resultArray, fileName, QDateTime::currentDateTime(), QDateTime::currentDateTime(), QDateTime::currentDateTime(), (quint64) DokanFileInfo->Context);

    m_server->remoteSetFileTime(resultArray, fileName, QDateTime::currentDateTime(), QDateTime::currentDateTime(), QDateTime::currentDateTime(), (quint64) DokanFileInfo->Context);

    QDataStream resultStream(resultArray);

    quint64 result;

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorUnlockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG	Length, PDOKAN_FILE_INFO	DokanFileInfo)
{
    QByteArray resultArray;

    QString fileName = getFileName(FileName);

    //m_client->clientUnlockFile(resultArray, fileName, ByteOffset, Length, DokanFileInfo->Context);

    m_server->remoteUnlockFile(resultArray, fileName, ByteOffset, Length, DokanFileInfo->Context);

    QDataStream resultStream(resultArray);

    quint64 result;

    resultStream >> result;

    return result;
}

NTSTATUS NetworkDrive::MirrorGetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG BufferLength, PULONG	LengthNeeded, PDOKAN_FILE_INFO DokanFileInfo)
{
    return STATUS_SUCCESS;

}

NTSTATUS NetworkDrive::MirrorSetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG SecurityDescriptorLength, PDOKAN_FILE_INFO DokanFileInfo)
{
    return STATUS_SUCCESS;

}

NTSTATUS NetworkDrive::MirrorGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD	MaximumComponentLength, LPDWORD	FileSystemFlags, LPWSTR	FileSystemNameBuffer, DWORD	FileSystemNameSize, PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    wcscpy_s(VolumeNameBuffer, VolumeNameSize, L"DOKAN");
    *VolumeSerialNumber = 0x19831116;
    *MaximumComponentLength = 256;
    *FileSystemFlags = FILE_CASE_SENSITIVE_SEARCH |
                        FILE_CASE_PRESERVED_NAMES |
                        FILE_SUPPORTS_REMOTE_STORAGE |
                        FILE_UNICODE_ON_DISK |
                        FILE_PERSISTENT_ACLS;

    wcscpy_s(FileSystemNameBuffer, FileSystemNameSize, L"Dokan");

    return STATUS_SUCCESS;
}

NTSTATUS NetworkDrive::MirrorUnmount(PDOKAN_FILE_INFO	DokanFileInfo)
{
    //qDebug() << "Unmount";
    return STATUS_SUCCESS;
}

NTSTATUS NetworkDrive::MirrorEnumerateNamedStreams(LPCWSTR FileName, PVOID* EnumContext, LPWSTR StreamName, PULONG StreamNameLength, PLONGLONG StreamSize, PDOKAN_FILE_INFO DokanFileInfo)
{

    return STATUS_SUCCESS;
}




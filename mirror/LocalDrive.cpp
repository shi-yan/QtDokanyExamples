#include "LocalDrive.h"

LocalDrive::LocalDrive()
{

}

LocalDrive::~LocalDrive()
{

}

NTSTATUS LocalDrive::MirrorCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext, ACCESS_MASK AccessMode, ULONG FileAttributes, ULONG ShareMode, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO DokanFileInfo)
{
    QString fileName = getFileName(FileName);

    QDir dir("c:\\test");
    QString filePath = dir.absoluteFilePath(fileName);
    qDebug() << "inside create file" << filePath;

    qDebug() << "beginning"   << CreateDisposition;

    if((CreateOptions & FILE_DIRECTORY_FILE) == FILE_DIRECTORY_FILE)
    {
        DokanFileInfo->IsDirectory = true;
        if(CreateDisposition == FILE_CREATE)
        {
            qDebug() << "create directory";

            dir.setPath(filePath);
            if(dir.exists())
            {
                return ERROR_FILE_EXISTS;
            }

            if(!dir.mkpath(filePath))
            {
                return ERROR_ACCESS_DENIED;
            }

            return STATUS_SUCCESS;
        }
        else if(CreateDisposition == FILE_OPEN_IF)
        {
            qDebug() << "open directory" ;

            dir.setPath(filePath);
            if(!dir.exists())
            {
                return STATUS_OBJECT_PATH_NOT_FOUND;
            }

            return STATUS_SUCCESS;
        }
    }
    else
    {
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

        DokanFileInfo->IsDirectory = false;

        switch(CreateDisposition)
        {
        case OPEN_EXISTING:
            if (pathExists)
            {
                if (readWriteAttribute || pathIsDirectory)
                {
                    DokanFileInfo->IsDirectory = pathIsDirectory;
                    qDebug() << "set dir" << pathIsDirectory;
                    DokanFileInfo->Context = 0;
                    return STATUS_SUCCESS;
                }
            }
            else
            {
                qDebug() << "return object name not found";
                if (pathIsDirectory)
                {
                    qDebug() << "return path not found";
                    return STATUS_OBJECT_PATH_NOT_FOUND;
                }
                else
                {
                    qDebug() << "return file not found";
                    return STATUS_OBJECT_NAME_NOT_FOUND ;
                }
            }
            break;

        case CREATE_NEW:
            if (pathExists)
            {
                return ERROR_FILE_EXISTS;
            }
            break;

        case TRUNCATE_EXISTING:
            openMode |= QFile::Truncate;
            if (!pathExists)
            {
                return ERROR_FILE_NOT_FOUND;
            }
            break;
        case OPEN_ALWAYS:
            break;
        case CREATE_ALWAYS:
            break;
        default:
            break;
        }

        QFile *file = new QFile(filePath);

        if (file)
        {
            DokanFileInfo->Context = (ULONG64) file;
            if(file->open(openMode))
            {
                qDebug() << "success" << openMode;
                return STATUS_SUCCESS;
            }
            else
            {
                return STATUS_OBJECT_NAME_NOT_FOUND;
            }
        }
        else
        {
            return STATUS_OBJECT_NAME_NOT_FOUND;
        }
    }
    return STATUS_SUCCESS;
}

void LocalDrive::MirrorCloseFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "close" << file->fileName();
        file->close();
        DokanFileInfo->Context = NULL;
    }
}

void LocalDrive::MirrorCleanup(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        file->close();
        DokanFileInfo->Context = NULL;
    }

    QString fileName = getFileName(FileName);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "cleanup" << filePath;

    if (DokanFileInfo->DeleteOnClose)
    {
        qDebug() << "also delete";
        dir.remove(filePath);
    }
    return ;
}

NTSTATUS LocalDrive::MirrorReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "normal read" << file->fileName();
        file->seek(Offset);
        *ReadLength = file->read((char*)Buffer, BufferLength);
    }
    else
    {
        QString fileName = getFileName(FileName);

        QDir dir("c:\\test");

        QString filePath = dir.absoluteFilePath(fileName);

        qDebug() << "memory mapped read" << filePath;
        QFile file(filePath);
        file.open(QFile::ReadOnly);
        file.seek(Offset);
        *ReadLength = file.read((char*)Buffer, BufferLength);
        file.close();
    }

    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "normal read" << file->fileName();
        file->seek(Offset);
        *NumberOfBytesWritten = file->write((char*)Buffer, NumberOfBytesToWrite);
    }
    else
    {
        QString fileName = getFileName(FileName);

        QDir dir("c:\\test");

        QString filePath = dir.absoluteFilePath(fileName);

        qDebug() << "memory mapped read" << filePath;
        QFile file(filePath);
        file.open(QFile::ReadOnly);
        file.seek(Offset);
        *NumberOfBytesWritten = file.write((char*)Buffer, NumberOfBytesToWrite);
        file.close();
    }

    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorFlushFileBuffers(LPCWSTR	FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "flush file" << file->fileName();

        file->flush();
    }
    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorGetFileInformation(LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation, PDOKAN_FILE_INFO DokanFileInfo)
{
    QString fileName = getFileName(FileName);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "file info ===========================" << filePath;

    QFileInfo fileInfo(filePath);

    HandleFileInformation->ftCreationTime = toWinFileTime(fileInfo.created());
    HandleFileInformation->ftLastAccessTime = toWinFileTime(fileInfo.lastRead());
    HandleFileInformation->ftLastWriteTime = toWinFileTime(fileInfo.lastModified());
    HandleFileInformation->dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;
    if (fileInfo.isDir())
        HandleFileInformation->dwFileAttributes =0 ;
    if (fileInfo.isHidden())
    {
        HandleFileInformation->dwFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
    }

    quint64 size = fileInfo.size();

    HandleFileInformation->nFileSizeLow = size & 0xffffffff;
    HandleFileInformation->nFileSizeHigh = size >> 32;

    if( DokanFileInfo->IsDirectory)
    {
        HandleFileInformation->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        HandleFileInformation->nFileSizeLow = 4096;
    }

    HandleFileInformation->dwVolumeSerialNumber = 0x19831116;

    HandleFileInformation->nNumberOfLinks = 1;
    HandleFileInformation->nFileIndexHigh = 0;
    HandleFileInformation->nFileIndexLow = 0;

    qDebug() << "size low" << size<<  HandleFileInformation->nFileSizeLow;
    qDebug() << "size high" << size<<HandleFileInformation->nFileSizeHigh;

    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorFindFiles(LPCWSTR FileName, PFillFindData FillFindData, PDOKAN_FILE_INFO	DokanFileInfo)
{
    QString fileName = QString::fromStdWString(FileName);
    fileName = fileName.right(fileName.size() - 1);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "findfiles ===================" << filePath;

    dir.setPath(filePath);

    QFileInfoList infoList = dir.entryInfoList();
    qDebug() << "findfiles ===================" << filePath << infoList.size();

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

        FillFindData(&findData, DokanFileInfo);
    }

    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorDeleteFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);
    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorDeleteDirectory(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    QString fileName = getFileName(FileName);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "create folder" << filePath;

    dir.setPath(filePath);

    if (dir.entryInfoList().empty())
        return STATUS_SUCCESS;
    else
        return ERROR_DIR_NOT_EMPTY;
}

NTSTATUS LocalDrive::MirrorMoveFile(LPCWSTR	FileName, LPCWSTR NewFileName, BOOL ReplaceIfExisting, PDOKAN_FILE_INFO DokanFileInfo)
{
    QString oldFileName = QString::fromStdWString(FileName);
    oldFileName = oldFileName.right(oldFileName.size() - 1);

    QDir olddir("c:\\test");

    QString oldFilePath = olddir.absoluteFilePath(oldFileName);

    qDebug() << "move folder" << oldFilePath;


    QString newFileName = QString::fromStdWString(NewFileName);
    newFileName = newFileName.right(newFileName.size() - 1);

    QDir newdir("c:\\test");

    QString newFilePath = newdir.absoluteFilePath(newFileName);

    qDebug() << "move folder" << newFilePath;


    QFileInfo info(newFilePath);
    bool exist = info.exists();

    if (!exist)
    {
        DokanFileInfo->Context = NULL;
        olddir.rename(oldFilePath, newFileName);
        return STATUS_SUCCESS;
    }
    else if(ReplaceIfExisting)
    {
        DokanFileInfo->Context = NULL;
        newdir.remove(oldFilePath);
        olddir.rename(oldFilePath, newFileName);
        return STATUS_SUCCESS;
    }

    return ERROR_FILE_EXISTS;
}

NTSTATUS LocalDrive::MirrorLockFile(LPCWSTR	FileName, LONGLONG ByteOffset, LONGLONG Length, PDOKAN_FILE_INFO DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "lock file" << file->fileName();


    }

    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorSetEndOfFile(LPCWSTR	FileName, LONGLONG ByteOffset, PDOKAN_FILE_INFO DokanFileInfo)
{
    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorSetAllocationSize(LPCWSTR FileName, LONGLONG	AllocSize, PDOKAN_FILE_INFO DokanFileInfo)
{
    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorSetFileAttributes(LPCWSTR FileName, DWORD FileAttributes, PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "lock file" << file->fileName();


    }
    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorSetFileTime(LPCWSTR FileName, CONST FILETIME* CreationTime, CONST FILETIME* LastAccessTime, CONST FILETIME* LastWriteTime, PDOKAN_FILE_INFO DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "lock file" << file->fileName();



    }
    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorUnlockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG	Length, PDOKAN_FILE_INFO	DokanFileInfo)
{

    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorGetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG BufferLength, PULONG	LengthNeeded, PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorSetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG SecurityDescriptorLength, PDOKAN_FILE_INFO DokanFileInfo)
{
    return STATUS_SUCCESS;

}

NTSTATUS LocalDrive::MirrorGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD	MaximumComponentLength, LPDWORD	FileSystemFlags, LPWSTR	FileSystemNameBuffer, DWORD	FileSystemNameSize, PDOKAN_FILE_INFO DokanFileInfo)
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

NTSTATUS LocalDrive::MirrorUnmount(PDOKAN_FILE_INFO	DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    qDebug() << "Unmount";
    return STATUS_SUCCESS;
}

NTSTATUS LocalDrive::MirrorEnumerateNamedStreams(LPCWSTR FileName, PVOID* EnumContext, LPWSTR StreamName, PULONG StreamNameLength, PLONGLONG StreamSize, PDOKAN_FILE_INFO DokanFileInfo)
{
    return STATUS_SUCCESS;

}

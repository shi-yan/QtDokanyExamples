#include <QCoreApplication>
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <winbase.h>
#include <ntstatus.h>
#include <stdio.h>
#include <stdlib.h>
#include "dokan.h"
#include "fileinfo.h"

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDateTime>

#include <algorithm>

static BOOL g_UseStdErr;
static BOOL g_DebugMode;

static void DbgPrint(LPCWSTR format, ...)
{
    if (g_DebugMode) {
        WCHAR buffer[512];
        va_list argp;
        va_start(argp, format);
        vswprintf_s(buffer, sizeof(buffer)/sizeof(WCHAR), format, argp);
        va_end(argp);
        if (g_UseStdErr) {
            fputws(buffer, stderr);
        } else {
            OutputDebugStringW(buffer);
        }
    }
}

static WCHAR  RootDirectory[MAX_PATH] = L"C:";
static WCHAR MountPoint[MAX_PATH] = L"M:";

static void
GetFilePath(
    PWCHAR	filePath,
    ULONG	numberOfElements,
    LPCWSTR FileName)
{
    wcsncpy_s(filePath, numberOfElements, RootDirectory, wcslen(RootDirectory));
    wcsncat_s(filePath, numberOfElements, FileName, wcslen(FileName));
}


static void
PrintUserName(PDOKAN_FILE_INFO	DokanFileInfo)
{
    HANDLE	handle;
    UCHAR buffer[1024];
    DWORD returnLength;
    WCHAR accountName[256];
    WCHAR domainName[256];
    DWORD accountLength = sizeof(accountName) / sizeof(WCHAR);
    DWORD domainLength = sizeof(domainName) / sizeof(WCHAR);
    PTOKEN_USER tokenUser;
    SID_NAME_USE snu;

    handle = DokanOpenRequestorToken(DokanFileInfo);
    if (handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"  DokanOpenRequestorToken failed\n");
        return;
    }

    if (!GetTokenInformation(handle, TokenUser, buffer, sizeof(buffer), &returnLength)) {
        DbgPrint(L"  GetTokenInformaiton failed: %d\n", GetLastError());
        CloseHandle(handle);
        return;
    }

    CloseHandle(handle);

    tokenUser = (PTOKEN_USER)buffer;
    if (!LookupAccountSid(NULL, tokenUser->User.Sid, accountName,
            &accountLength, domainName, &domainLength, &snu)) {
        DbgPrint(L"  LookupAccountSid failed: %d\n", GetLastError());
        return;
    }

    DbgPrint(L"  AccountName: %s, DomainName: %s\n", accountName, domainName);
}

static NTSTATUS ToNtStatus(DWORD dwError)
{
    switch (dwError)
    {
    case ERROR_FILE_NOT_FOUND:
        return STATUS_OBJECT_NAME_NOT_FOUND;
    case ERROR_PATH_NOT_FOUND:
        return STATUS_OBJECT_PATH_NOT_FOUND;
    case ERROR_INVALID_PARAMETER:
        return STATUS_INVALID_PARAMETER;
    default:
        return STATUS_ACCESS_DENIED;
    }
}

#define MirrorCheckFlag(val, flag) if (val&flag) { DbgPrint(L"\t" L#flag L"\n"); }

const DWORD DataAccess = FILE_READ_DATA | FILE_WRITE_DATA | FILE_APPEND_DATA |
        FILE_EXECUTE | FILE_GENERIC_EXECUTE | FILE_GENERIC_WRITE | FILE_GENERIC_READ;

const DWORD DataWriteAccess = FILE_WRITE_DATA | FILE_APPEND_DATA | 65536 | FILE_GENERIC_WRITE;

static NTSTATUS DOKAN_CALLBACK
MirrorCreateFile(
        LPCWSTR FileName,
        PDOKAN_IO_SECURITY_CONTEXT SecurityContext,
        ACCESS_MASK AccessMode,
           ULONG FileAttributes,
           ULONG ShareMode,
           ULONG CreateDisposition,
           ULONG CreateOptions,
           PDOKAN_FILE_INFO DokanFileInfo
        )
{


    QString fileName = QString::fromStdWString(FileName);
    fileName = fileName.right(fileName.size() - 1);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "inside create file" << filePath;

    bool pathExists = true;
    bool pathIsDirectory = false;

    bool readWriteAttribute = (AccessMode & DataAccess) == 0;

    bool readAccess = (AccessMode & DataWriteAccess) == 0;

    QFileInfo fileInfo(filePath);

    pathExists = fileInfo.exists();
    pathIsDirectory = fileInfo.isDir();

    FileAttributes = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE;
    if ( pathIsDirectory) {
        FileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        FileAttributes |= FILE_FLAG_BACKUP_SEMANTICS;
        //AccessMode = 0;
    }
    QFile::OpenMode openMode = readAccess ? QFile::ReadOnly : QFile::ReadWrite;

    if (AccessMode & FILE_APPEND_DATA)
        openMode |= QFile::Append;
    qDebug() << "beginning" << pathIsDirectory << openMode << CreateDisposition;

    if((CreateOptions & FILE_DIRECTORY_FILE) == FILE_DIRECTORY_FILE)
    {

        if(CreateDisposition == FILE_CREATE) {


            qDebug() << "create directory";
            //UNREFERENCED_PARAMETER(DokanFileInfo);

            QString fileName = QString::fromStdWString(FileName);
            fileName = fileName.right(fileName.size() - 1);

            QDir dir("c:\\test");

            QString filePath = dir.absoluteFilePath(fileName);

            qDebug() << "create folder" << filePath;
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

            UNREFERENCED_PARAMETER(DokanFileInfo);

            QString fileName = QString::fromStdWString(FileName);
            fileName = fileName.right(fileName.size() - 1);

            QDir dir("c:\\test");

            QString filePath = dir.absoluteFilePath(fileName);

            qDebug() << "create folder" << filePath;

            dir.setPath(filePath);
            if(!dir.exists())
            {
                return STATUS_OBJECT_PATH_NOT_FOUND;
            }


            return STATUS_SUCCESS;
        }

    }else{

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
               /* if (pathIsDirectory)
                    return STATUS_ACCESS_DENIED;
               */

                /*if (pathIsDirectory)
                {
                    qDebug() << "return path not found";

                    return STATUS_OBJECT_PATH_NOT_FOUND;
                }*/
                qDebug() << "return success";
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
        qDebug() << "create new" << pathIsDirectory;
        DokanFileInfo->DeleteOnClose = false;
        if (pathExists){
            qDebug() << "return file exists";

            return ERROR_FILE_EXISTS;
        }
        break;

    case TRUNCATE_EXISTING:
        openMode |= QFile::Truncate;
        if (!pathExists)
        {
            qDebug() << "return file not found";

            return ERROR_FILE_NOT_FOUND;
        }
        break;
    case OPEN_ALWAYS:
        break;
    case CREATE_ALWAYS:
        DokanFileInfo->DeleteOnClose = false;
        qDebug() << "create new" << pathIsDirectory;

        break;
    default:
        break;
    }

    qDebug() << "create new3" << pathIsDirectory;


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


static NTSTATUS DOKAN_CALLBACK
MirrorCreateDirectory(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    qDebug() << "create directory";
    UNREFERENCED_PARAMETER(DokanFileInfo);

    QString fileName = QString::fromStdWString(FileName);
    fileName = fileName.right(fileName.size() - 1);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "create folder" << filePath;
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


static NTSTATUS DOKAN_CALLBACK
MirrorOpenDirectory(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    qDebug() << "open directory" ;

    UNREFERENCED_PARAMETER(DokanFileInfo);

    QString fileName = QString::fromStdWString(FileName);
    fileName = fileName.right(fileName.size() - 1);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "create folder" << filePath;

    dir.setPath(filePath);
    if(!dir.exists())
    {
        return STATUS_OBJECT_PATH_NOT_FOUND;
    }


    return STATUS_SUCCESS;
}


static void DOKAN_CALLBACK
MirrorCloseFile(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    qDebug() << "============== inside file close function";
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "close" << file->fileName();
        file->close();
        DokanFileInfo->Context = NULL;
    }
    qDebug() << "============ finished file close function";
}


static void DOKAN_CALLBACK
MirrorCleanup(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        file->close();
        DokanFileInfo->Context = NULL;
    }

    QString fileName = QString::fromStdWString(FileName);
    fileName = fileName.right(fileName.size() - 1);

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


static NTSTATUS DOKAN_CALLBACK
MirrorReadFile(
    LPCWSTR				FileName,
    LPVOID				Buffer,
    DWORD				BufferLength,
    LPDWORD				ReadLength,
    LONGLONG			Offset,
    PDOKAN_FILE_INFO	DokanFileInfo)
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
        QString fileName = QString::fromStdWString(FileName);
        fileName = fileName.right(fileName.size() - 1);

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


static NTSTATUS DOKAN_CALLBACK
MirrorWriteFile(
    LPCWSTR		FileName,
    LPCVOID		Buffer,
    DWORD		NumberOfBytesToWrite,
    LPDWORD		NumberOfBytesWritten,
    LONGLONG			Offset,
    PDOKAN_FILE_INFO	DokanFileInfo)
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
        QString fileName = QString::fromStdWString(FileName);
        fileName = fileName.right(fileName.size() - 1);

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


static NTSTATUS DOKAN_CALLBACK
MirrorFlushFileBuffers(
    LPCWSTR		FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{

    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "flush file" << file->fileName();

        file->flush();
    }
    return STATUS_SUCCESS;

}

// Convert a QDateTime to a FILETIME.
FILETIME toWinFileTime(const QDateTime &dateTime)
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

static NTSTATUS DOKAN_CALLBACK
MirrorGetFileInformation(
    LPCWSTR							FileName,
    LPBY_HANDLE_FILE_INFORMATION	HandleFileInformation,
    PDOKAN_FILE_INFO				DokanFileInfo)
{

    QString fileName = QString::fromStdWString(FileName);
    fileName = fileName.right(fileName.size() - 1);

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
    ///
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
/*

    WCHAR	filePath[MAX_PATH];
    HANDLE	handle = (HANDLE)DokanFileInfo->Context;
    BOOL	opened = FALSE;

    GetFilePath(filePath, MAX_PATH, FileName);

    DbgPrint(L"GetFileInfo : %s\n", filePath);

    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");

        // If CreateDirectory returned FILE_ALREADY_EXISTS and
        // it is called with FILE_OPEN_IF, that handle must be opened.
        handle = CreateFile(filePath, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (handle == INVALID_HANDLE_VALUE)
        {
            DWORD error = GetLastError();
            DbgPrint(L"GetFileInfo failed(%d)\n", error);
            return ToNtStatus(error);
        }
        opened = TRUE;
    }

    if (!GetFileInformationByHandle(handle,HandleFileInformation)) {
        DbgPrint(L"\terror code = %d\n", GetLastError());

        // FileName is a root directory
        // in this case, FindFirstFile can't get directory information
        if (wcslen(FileName) == 1) {
            DbgPrint(L"  root dir\n");
            HandleFileInformation->dwFileAttributes = GetFileAttributes(filePath);

        } else {
            WIN32_FIND_DATAW find;
            ZeroMemory(&find, sizeof(WIN32_FIND_DATAW));
            handle = FindFirstFile(filePath, &find);
            if (handle == INVALID_HANDLE_VALUE) {
                DWORD error = GetLastError();
                DbgPrint(L"\tFindFirstFile error code = %d\n\n", error);
                return ToNtStatus(error);
            }
            HandleFileInformation->dwFileAttributes = find.dwFileAttributes;
            HandleFileInformation->ftCreationTime = find.ftCreationTime;
            HandleFileInformation->ftLastAccessTime = find.ftLastAccessTime;
            HandleFileInformation->ftLastWriteTime = find.ftLastWriteTime;
            HandleFileInformation->nFileSizeHigh = find.nFileSizeHigh;
            HandleFileInformation->nFileSizeLow = find.nFileSizeLow;
            DbgPrint(L"\tFindFiles OK, file size = %d\n", find.nFileSizeLow);
            FindClose(handle);
        }
    } else {
        DbgPrint(L"\tGetFileInformationByHandle success, file size = %d\n",
            HandleFileInformation->nFileSizeLow);
    }

*/
  /*  MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_ARCHIVE);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_ENCRYPTED);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_HIDDEN);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_NORMAL);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_OFFLINE);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_READONLY);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_SYSTEM);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_ATTRIBUTE_TEMPORARY);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_WRITE_THROUGH);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_OVERLAPPED);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_NO_BUFFERING);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_RANDOM_ACCESS);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_SEQUENTIAL_SCAN);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_DELETE_ON_CLOSE);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_BACKUP_SEMANTICS);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_POSIX_SEMANTICS);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_OPEN_REPARSE_POINT);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, FILE_FLAG_OPEN_NO_RECALL);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, SECURITY_ANONYMOUS);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, SECURITY_IDENTIFICATION);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, SECURITY_IMPERSONATION);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, SECURITY_DELEGATION);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, SECURITY_CONTEXT_TRACKING);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, SECURITY_EFFECTIVE_ONLY);
    MirrorCheckFlag(HandleFileInformation->dwFileAttributes, SECURITY_SQOS_PRESENT);

    DbgPrint(L"\n");
*/
  /*  if (opened) {
        CloseHandle(handle);
    }
*/
    return STATUS_SUCCESS;
    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorFindFiles(
    LPCWSTR				FileName,
    PFillFindData		FillFindData, // function pointer
    PDOKAN_FILE_INFO	DokanFileInfo)
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

       // strncpy((char*)findData.cFileName, (char*)nameArray.data(), nameArray.size());
        FillFindData(&findData, DokanFileInfo);
    }


 /*   WCHAR				filePath[MAX_PATH];
    size_t				fileLen;
    HANDLE				hFind;
    WIN32_FIND_DATAW	findData;
    DWORD				error;
    int					count = 0;

    GetFilePath(filePath, MAX_PATH, FileName);

    DbgPrint(L"FindFiles :%s\n", filePath);

    fileLen = wcslen(filePath);
    if (filePath[fileLen-1] != L'\\') {
        filePath[fileLen++] = L'\\';
    }
    filePath[fileLen] = L'*';
    filePath[fileLen+1] = L'\0';

    hFind = FindFirstFile(filePath, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        error = GetLastError();
        DbgPrint(L"\tinvalid file handle. Error is %u\n\n", error);
        return ToNtStatus(error);
    }

    FillFindData(&findData, DokanFileInfo);
    count++;

    while (FindNextFile(hFind, &findData) != 0) {
        FillFindData(&findData, DokanFileInfo);
        count++;
    }

    error = GetLastError();
    FindClose(hFind);

    if (error != ERROR_NO_MORE_FILES) {
        DbgPrint(L"\tFindNextFile error. Error is %u\n\n", error);
        return ToNtStatus(error);
    }

    DbgPrint(L"\tFindFiles return %d entries in %s\n\n", count, filePath);

    return STATUS_SUCCESS;*/

    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorDeleteFile(
    LPCWSTR				FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);



    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorDeleteDirectory(
    LPCWSTR				FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    QString fileName = QString::fromStdWString(FileName);
    fileName = fileName.right(fileName.size() - 1);

    QDir dir("c:\\test");

    QString filePath = dir.absoluteFilePath(fileName);

    qDebug() << "create folder" << filePath;

    dir.setPath(filePath);

    if (dir.entryInfoList().empty())
        return STATUS_SUCCESS;
    else
        return ERROR_DIR_NOT_EMPTY;
}


static NTSTATUS DOKAN_CALLBACK
MirrorMoveFile(
    LPCWSTR				FileName, // existing file name
    LPCWSTR				NewFileName,
    BOOL				ReplaceIfExisting,
    PDOKAN_FILE_INFO	DokanFileInfo)
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


static NTSTATUS DOKAN_CALLBACK
MirrorLockFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    LONGLONG			Length,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "lock file" << file->fileName();


    }

    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorSetEndOfFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    PDOKAN_FILE_INFO	DokanFileInfo)
{


    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorSetAllocationSize(
    LPCWSTR				FileName,
    LONGLONG			AllocSize,
    PDOKAN_FILE_INFO	DokanFileInfo)
{

    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorSetFileAttributes(
    LPCWSTR				FileName,
    DWORD				FileAttributes,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "lock file" << file->fileName();


    }
    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorSetFileTime(
    LPCWSTR				FileName,
    CONST FILETIME*		CreationTime,
    CONST FILETIME*		LastAccessTime,
    CONST FILETIME*		LastWriteTime,
    PDOKAN_FILE_INFO	DokanFileInfo)
{

    if (DokanFileInfo->Context)
    {
        QFile *file = (QFile*)DokanFileInfo->Context;
        qDebug() << "lock file" << file->fileName();



    }
    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorUnlockFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    LONGLONG			Length,
    PDOKAN_FILE_INFO	DokanFileInfo)
{

    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorGetFileSecurity(
    LPCWSTR					FileName,
    PSECURITY_INFORMATION	SecurityInformation,
    PSECURITY_DESCRIPTOR	SecurityDescriptor,
    ULONG				BufferLength,
    PULONG				LengthNeeded,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    WCHAR	filePath[MAX_PATH];

    UNREFERENCED_PARAMETER(DokanFileInfo);
/*
    GetFilePath(filePath, MAX_PATH, FileName);

    DbgPrint(L"GetFileSecurity %s\n", filePath);

    MirrorCheckFlag(*SecurityInformation, FILE_SHARE_READ);
    MirrorCheckFlag(*SecurityInformation, OWNER_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, GROUP_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, DACL_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, SACL_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, LABEL_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, ATTRIBUTE_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, SCOPE_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, PROCESS_TRUST_LABEL_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, BACKUP_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, PROTECTED_DACL_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, PROTECTED_SACL_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, UNPROTECTED_DACL_SECURITY_INFORMATION);
    MirrorCheckFlag(*SecurityInformation, UNPROTECTED_SACL_SECURITY_INFORMATION);


    DbgPrint(L"  Opening new handle with READ_CONTROL access\n");
    HANDLE handle = CreateFile(
        filePath,
        READ_CONTROL | ((*SecurityInformation & SACL_SECURITY_INFORMATION) ? ACCESS_SYSTEM_SECURITY : 0),
        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL, // security attribute
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,// |FILE_FLAG_NO_BUFFERING,
        NULL);

    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        int error = GetLastError();
        return ToNtStatus(error);
    }

    if (!GetUserObjectSecurity(handle, SecurityInformation, SecurityDescriptor,
        BufferLength, LengthNeeded)) {
        int error = GetLastError();
        if (error == ERROR_INSUFFICIENT_BUFFER) {
            DbgPrint(L"  GetUserObjectSecurity failed: ERROR_INSUFFICIENT_BUFFER\n");
            CloseHandle(handle);
            return ToNtStatus(error);
        }
        else {
            DbgPrint(L"  GetUserObjectSecurity failed: %d\n", error);
            CloseHandle(handle);
            return ToNtStatus(error);
        }
    }
    CloseHandle(handle);
*/
    return STATUS_SUCCESS;
}


static NTSTATUS DOKAN_CALLBACK
MirrorSetFileSecurity(
    LPCWSTR					FileName,
    PSECURITY_INFORMATION	SecurityInformation,
    PSECURITY_DESCRIPTOR	SecurityDescriptor,
    ULONG				SecurityDescriptorLength,
    PDOKAN_FILE_INFO	DokanFileInfo)
{

    return STATUS_SUCCESS;
}

static NTSTATUS DOKAN_CALLBACK
MirrorGetVolumeInformation(
    LPWSTR		VolumeNameBuffer,
    DWORD		VolumeNameSize,
    LPDWORD		VolumeSerialNumber,
    LPDWORD		MaximumComponentLength,
    LPDWORD		FileSystemFlags,
    LPWSTR		FileSystemNameBuffer,
    DWORD		FileSystemNameSize,
    PDOKAN_FILE_INFO	DokanFileInfo)
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


static NTSTATUS DOKAN_CALLBACK
MirrorUnmount(
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    DbgPrint(L"Unmount\n");
    return STATUS_SUCCESS;
}

/**
 * Avoid #include <winternl.h> which as conflict with FILE_INFORMATION_CLASS definition.
 * This only for MirrorEnumerateNamedStreams. Link with ntdll.lib still required.
 *
 * Not needed if you're not using NtQueryInformationFile!
 *
 * BEGIN
 */
/*typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    } DUMMYUNIONNAME;

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

NTSYSCALLAPI NTSTATUS NTAPI 	NtQueryInformationFile(_In_ HANDLE FileHandle, _Out_ PIO_STATUS_BLOCK IoStatusBlock, _Out_writes_bytes_(Length) PVOID FileInformation, _In_ ULONG Length, _In_ FILE_INFORMATION_CLASS FileInformationClass);
*/
/**
 * END
 */

static NTSTATUS DOKAN_CALLBACK
MirrorEnumerateNamedStreams(
    LPCWSTR					FileName,
    PVOID*					EnumContext,
    LPWSTR					StreamName,
    PULONG					StreamNameLength,
    PLONGLONG				StreamSize,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    HANDLE	handle;
    WCHAR	filePath[MAX_PATH];

    GetFilePath(filePath, MAX_PATH, FileName);

    DbgPrint(L"EnumerateNamedStreams %s\n", filePath);

    handle = (HANDLE)DokanFileInfo->Context;
    if (!handle || handle == INVALID_HANDLE_VALUE) {
        DbgPrint(L"\tinvalid handle\n\n");
        return STATUS_NOT_IMPLEMENTED;
    }

    // As we are requested one by one, it would be better to use FindFirstStream / FindNextStream instead of requesting all streams each time
    // But this doesn't really matter on mirror sample
    BYTE InfoBlock[64 * 1024];
    PFILE_STREAM_INFORMATION pStreamInfo = (PFILE_STREAM_INFORMATION)InfoBlock;
    //IO_STATUS_BLOCK ioStatus;
    //ZeroMemory(InfoBlock, sizeof(InfoBlock));

   /* NTSTATUS status = NtQueryInformationFile(handle, &ioStatus, InfoBlock, sizeof(InfoBlock), FileStreamInformation);
    if (status != STATUS_SUCCESS) {
        DbgPrint(L"\tNtQueryInformationFile failed with %d.\n", status);
        return STATUS_NOT_IMPLEMENTED;
    }
*/
    if (pStreamInfo->StreamNameLength == 0) {
        DbgPrint(L"\tNo stream found.\n");
        return STATUS_NOT_IMPLEMENTED;
    }

    UINT index = (UINT)*EnumContext;
    DbgPrint(L"\tStream #%d requested.\n", index);

    for (UINT i = 0; i != index; ++i) {
        if (pStreamInfo->NextEntryOffset == 0) {
            DbgPrint(L"\tNo more stream.\n");
            return STATUS_NOT_IMPLEMENTED;
        }
        pStreamInfo = (PFILE_STREAM_INFORMATION) ((LPBYTE)pStreamInfo + pStreamInfo->NextEntryOffset);   // Next stream record
    }

    wcscpy_s(StreamName, SHRT_MAX + 1, pStreamInfo->StreamName);
    *StreamNameLength = pStreamInfo->StreamNameLength;
    *StreamSize = pStreamInfo->StreamSize.QuadPart;

    DbgPrint(L"\t Stream %ws\n", pStreamInfo->StreamName);

    // Remember next stream entry index
    *EnumContext = (PVOID)++index;

    return STATUS_SUCCESS;
}



int main(int argc, char *argv[]){
    int status;
    ULONG command;
    PDOKAN_OPERATIONS dokanOperations = (PDOKAN_OPERATIONS)malloc(sizeof(DOKAN_OPERATIONS));
    if (dokanOperations == NULL) {
        return EXIT_FAILURE;
    }
    PDOKAN_OPTIONS dokanOptions = (PDOKAN_OPTIONS)malloc(sizeof(DOKAN_OPTIONS));
    if (dokanOptions == NULL) {
        free(dokanOperations);
        return EXIT_FAILURE;
    }

    if (argc < 5) {
        fprintf(stderr, "mirror.exe\n"
            "  /r RootDirectory (ex. /r c:\\test)\n"
            "  /l DriveLetter (ex. /l m)\n"
            "  /t ThreadCount (ex. /t 5)\n"
            "  /d (enable debug output)\n"
            "  /s (use stderr for output)\n"
            "  /n (use network drive)\n"
            "  /m (use removable drive)\n"
            "  /w (write-protect drive)\n"
            "  /i (Timeout in Milliseconds ex. /i 30000)\n");
         free(dokanOperations);
        free(dokanOptions);
        return EXIT_FAILURE;
    }

    g_DebugMode = TRUE;
    g_UseStdErr = TRUE;

    ZeroMemory(dokanOptions, sizeof(DOKAN_OPTIONS));
    dokanOptions->Version = DOKAN_VERSION;
    dokanOptions->ThreadCount = 1; // use default

    for (command = 1; command < argc; command++) {
        switch (towlower(argv[command][1])) {
        case L'r':
            command++;
        {std::wstring wc( strlen(argv[command]), L'#' );
            mbstowcs( &wc[0], argv[command], strlen(argv[command]) );
            wcscpy_s(RootDirectory, sizeof(RootDirectory)/sizeof(WCHAR), wc.c_str());
            DbgPrint(L"RootDirectory: %ls\n", RootDirectory);}
            break;
        case L'l':
            command++;{
            std::wstring wc( strlen(argv[command]), L'#' );
            mbstowcs( &wc[0], argv[command], strlen(argv[command]) );
            wcscpy_s(MountPoint, sizeof(MountPoint)/sizeof(WCHAR), wc.c_str());
            dokanOptions->MountPoint = MountPoint;}
            break;
        case L't':
            command++;
            dokanOptions->ThreadCount = (USHORT)atoi(argv[command]);
            break;
        case L'd':
            g_DebugMode = TRUE;
            break;
        case L's':
            g_UseStdErr = TRUE;
            break;
        case L'n':
            dokanOptions->Options |= DOKAN_OPTION_NETWORK;
            break;
        case L'm':
            dokanOptions->Options |= DOKAN_OPTION_REMOVABLE;
            break;
        case L'w':
            //dokanOptions->Options |= DOKAN_OPTION_WRITE_PROTECT;
            break;
        case L'i':
            command++;
            dokanOptions->Timeout = (ULONG)atol(argv[command]);
            break;
        default:
            fwprintf(stderr, L"unknown command: %s\n", argv[command]);
            free(dokanOperations);
            free(dokanOptions);
            return EXIT_FAILURE;
        }
    }

    // Add security name privilege. Required here to handle GetFileSecurity properly.
    /*if (!AddSeSecurityNamePrivilege()) {
        fwprintf(stderr, L"  Failed to add security privilege to process\n");
        free(dokanOperations);
        free(dokanOptions);
        return -1;
    }*/

    if (g_DebugMode) {
        dokanOptions->Options |= DOKAN_OPTION_DEBUG;
    }
    if (g_UseStdErr) {
        dokanOptions->Options |= DOKAN_OPTION_STDERR;
    }

    dokanOptions->Options |= DOKAN_OPTION_ALT_STREAM;

    ZeroMemory(dokanOperations, sizeof(DOKAN_OPERATIONS));
    dokanOperations->ZwCreateFile = MirrorCreateFile;
    dokanOperations->Cleanup = MirrorCleanup;
    dokanOperations->CloseFile = MirrorCloseFile;
    dokanOperations->ReadFile = MirrorReadFile;
    dokanOperations->WriteFile = MirrorWriteFile;
    dokanOperations->FlushFileBuffers = MirrorFlushFileBuffers;
    dokanOperations->GetFileInformation = MirrorGetFileInformation;
    dokanOperations->FindFiles = MirrorFindFiles;
    dokanOperations->FindFilesWithPattern = NULL;
    dokanOperations->SetFileAttributes = MirrorSetFileAttributes;
    dokanOperations->SetFileTime = MirrorSetFileTime;
    dokanOperations->DeleteFile = MirrorDeleteFile;
    dokanOperations->DeleteDirectory = MirrorDeleteDirectory;
    dokanOperations->MoveFile = MirrorMoveFile;
    dokanOperations->SetEndOfFile = MirrorSetEndOfFile;
    dokanOperations->SetAllocationSize = MirrorSetAllocationSize;
    dokanOperations->LockFile = MirrorLockFile;
    dokanOperations->UnlockFile = MirrorUnlockFile;
    dokanOperations->GetFileSecurity = MirrorGetFileSecurity;
    dokanOperations->SetFileSecurity = MirrorSetFileSecurity;
    dokanOperations->GetDiskFreeSpace = NULL;
    dokanOperations->GetVolumeInformation = MirrorGetVolumeInformation;
    dokanOperations->Unmount = MirrorUnmount;
    //dokanOperations->CreateDirectoryW = MirrorCreateDirectory;
    //dokanOperations->OpenDirectory = MirrorOpenDirectory;
   // dokanOperations->f = MirrorFindStreams;

    status = DokanMain(dokanOptions, dokanOperations);
    switch (status) {
    case DOKAN_SUCCESS:
        fprintf(stderr, "Success\n");
        break;
    case DOKAN_ERROR:
        fprintf(stderr, "Error\n");
        break;
    case DOKAN_DRIVE_LETTER_ERROR:
        fprintf(stderr, "Bad Drive letter\n");
        break;
    case DOKAN_DRIVER_INSTALL_ERROR:
        fprintf(stderr, "Can't install driver\n");
        break;
    case DOKAN_START_ERROR:
        fprintf(stderr, "Driver something wrong\n");
        break;
    case DOKAN_MOUNT_ERROR:
        fprintf(stderr, "Can't assign a drive letter\n");
        break;
    case DOKAN_MOUNT_POINT_ERROR:
        fprintf(stderr, "Mount point error\n");
        break;
    default:
        fprintf(stderr, "Unknown error: %d\n", status);
        break;
    }

    free(dokanOptions);
    free(dokanOperations);
//    QCoreApplication a(argc, argv);
  //  a.exec();
    return EXIT_SUCCESS;
}


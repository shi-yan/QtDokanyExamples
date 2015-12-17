#ifndef DOKANDRIVEIMPLEMENTATION_H
#define DOKANDRIVEIMPLEMENTATION_H
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

const DWORD DataAccess = FILE_READ_DATA | FILE_WRITE_DATA | FILE_APPEND_DATA |
        FILE_EXECUTE | FILE_GENERIC_EXECUTE | FILE_GENERIC_WRITE | FILE_GENERIC_READ;

const DWORD DataWriteAccess = FILE_WRITE_DATA | FILE_APPEND_DATA | 65536 | FILE_GENERIC_WRITE;

class DokanDriveImplementation
{
protected:
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

    QString getFileName(LPCWSTR FileName)
    {
        QString fileName = QString::fromStdWString(FileName);
        qDebug() << "inside get file name" << fileName;
        fileName = fileName.right(fileName.size() - 1);
        return fileName;
    }

public:
    DokanDriveImplementation();
    virtual ~DokanDriveImplementation() {};

    virtual NTSTATUS MirrorCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext, ACCESS_MASK AccessMode, ULONG FileAttributes, ULONG ShareMode, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual void MirrorCloseFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual void MirrorCleanup(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD	NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorFlushFileBuffers(LPCWSTR	FileName, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorGetFileInformation(LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorFindFiles(LPCWSTR FileName, PFillFindData FillFindData, PDOKAN_FILE_INFO	DokanFileInfo) = 0;

    virtual NTSTATUS MirrorDeleteFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorDeleteDirectory(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorMoveFile(LPCWSTR	FileName, LPCWSTR NewFileName, BOOL ReplaceIfExisting, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorLockFile(LPCWSTR	FileName, LONGLONG ByteOffset, LONGLONG Length, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorSetEndOfFile(LPCWSTR	FileName, LONGLONG ByteOffset, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorSetAllocationSize(LPCWSTR FileName, LONGLONG	AllocSize, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorSetFileAttributes(LPCWSTR FileName, DWORD FileAttributes, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorSetFileTime(LPCWSTR FileName, CONST FILETIME* CreationTime, CONST FILETIME* LastAccessTime, CONST FILETIME* LastWriteTime, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorUnlockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG	Length, PDOKAN_FILE_INFO	DokanFileInfo) = 0;

    virtual NTSTATUS MirrorGetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG BufferLength, PULONG	LengthNeeded, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorSetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG SecurityDescriptorLength, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD	MaximumComponentLength, LPDWORD	FileSystemFlags, LPWSTR	FileSystemNameBuffer, DWORD	FileSystemNameSize, PDOKAN_FILE_INFO DokanFileInfo) = 0;

    virtual NTSTATUS MirrorUnmount(PDOKAN_FILE_INFO	DokanFileInfo) = 0;

    virtual NTSTATUS MirrorEnumerateNamedStreams(LPCWSTR FileName, PVOID* EnumContext, LPWSTR StreamName, PULONG StreamNameLength, PLONGLONG StreamSize, PDOKAN_FILE_INFO DokanFileInfo) = 0;
};

#endif // DOKANDRIVEIMPLEMENTATION_H

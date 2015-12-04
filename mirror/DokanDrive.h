#ifndef DOKANDRIVE_H
#define DOKANDRIVE_H

#include <QObject>
#include "DokanDriveImplementation.h"

struct DokanMirrorDriveInterface
{
    static DokanDriveImplementation *m_implementation;
    static int mount(DokanDriveImplementation *implementation);

    static NTSTATUS DOKAN_CALLBACK
    MirrorCreateFile(
        LPCWSTR FileName,
        PDOKAN_IO_SECURITY_CONTEXT SecurityContext,
        ACCESS_MASK AccessMode,
        ULONG FileAttributes,
        ULONG ShareMode,
        ULONG CreateDisposition,
        ULONG CreateOptions,
        PDOKAN_FILE_INFO DokanFileInfo);

    static void DOKAN_CALLBACK
    MirrorCloseFile(
        LPCWSTR					FileName,
        PDOKAN_FILE_INFO		DokanFileInfo);


    static void DOKAN_CALLBACK
    MirrorCleanup(
        LPCWSTR					FileName,
        PDOKAN_FILE_INFO		DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorReadFile(
        LPCWSTR				FileName,
        LPVOID				Buffer,
        DWORD				BufferLength,
        LPDWORD				ReadLength,
        LONGLONG			Offset,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorWriteFile(
        LPCWSTR		FileName,
        LPCVOID		Buffer,
        DWORD		NumberOfBytesToWrite,
        LPDWORD		NumberOfBytesWritten,
        LONGLONG			Offset,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorFlushFileBuffers(
        LPCWSTR		FileName,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorGetFileInformation(
        LPCWSTR							FileName,
        LPBY_HANDLE_FILE_INFORMATION	HandleFileInformation,
        PDOKAN_FILE_INFO				DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorFindFiles(
        LPCWSTR				FileName,
        PFillFindData		FillFindData, // function pointer
        PDOKAN_FILE_INFO	DokanFileInfo);

    static NTSTATUS DOKAN_CALLBACK
    MirrorDeleteFile(
        LPCWSTR				FileName,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorDeleteDirectory(
        LPCWSTR				FileName,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorMoveFile(
        LPCWSTR				FileName, // existing file name
        LPCWSTR				NewFileName,
        BOOL				ReplaceIfExisting,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorLockFile(
        LPCWSTR				FileName,
        LONGLONG			ByteOffset,
        LONGLONG			Length,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorSetEndOfFile(
        LPCWSTR				FileName,
        LONGLONG			ByteOffset,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorSetAllocationSize(
        LPCWSTR				FileName,
        LONGLONG			AllocSize,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorSetFileAttributes(
        LPCWSTR				FileName,
        DWORD				FileAttributes,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorSetFileTime(
        LPCWSTR				FileName,
        CONST FILETIME*		CreationTime,
        CONST FILETIME*		LastAccessTime,
        CONST FILETIME*		LastWriteTime,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorUnlockFile(
        LPCWSTR				FileName,
        LONGLONG			ByteOffset,
        LONGLONG			Length,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorGetFileSecurity(
        LPCWSTR					FileName,
        PSECURITY_INFORMATION	SecurityInformation,
        PSECURITY_DESCRIPTOR	SecurityDescriptor,
        ULONG				BufferLength,
        PULONG				LengthNeeded,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorSetFileSecurity(
        LPCWSTR					FileName,
        PSECURITY_INFORMATION	SecurityInformation,
        PSECURITY_DESCRIPTOR	SecurityDescriptor,
        ULONG				SecurityDescriptorLength,
        PDOKAN_FILE_INFO	DokanFileInfo);

    static NTSTATUS DOKAN_CALLBACK
    MirrorGetVolumeInformation(
        LPWSTR		VolumeNameBuffer,
        DWORD		VolumeNameSize,
        LPDWORD		VolumeSerialNumber,
        LPDWORD		MaximumComponentLength,
        LPDWORD		FileSystemFlags,
        LPWSTR		FileSystemNameBuffer,
        DWORD		FileSystemNameSize,
        PDOKAN_FILE_INFO	DokanFileInfo);


    static NTSTATUS DOKAN_CALLBACK
    MirrorUnmount(
        PDOKAN_FILE_INFO	DokanFileInfo);

    static NTSTATUS DOKAN_CALLBACK
    MirrorEnumerateNamedStreams(
        LPCWSTR					FileName,
        PVOID*					EnumContext,
        LPWSTR					StreamName,
        PULONG					StreamNameLength,
        PLONGLONG				StreamSize,
        PDOKAN_FILE_INFO		DokanFileInfo);

};

#endif // DOKANDRIVE_H

#ifndef LOCALDRIVE_H
#define LOCALDRIVE_H

#include "DokanDriveImplementation.h"

class LocalDrive : public DokanDriveImplementation
{
public:
    LocalDrive();
    ~LocalDrive();

    NTSTATUS MirrorCreateFile(LPCWSTR FileName, PDOKAN_IO_SECURITY_CONTEXT SecurityContext, ACCESS_MASK AccessMode, ULONG FileAttributes, ULONG ShareMode, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO DokanFileInfo);

    void MirrorCloseFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo);

    void MirrorCleanup(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorReadFile(LPCWSTR FileName, LPVOID Buffer, DWORD BufferLength, LPDWORD ReadLength, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorWriteFile(LPCWSTR FileName, LPCVOID Buffer, DWORD	NumberOfBytesToWrite, LPDWORD NumberOfBytesWritten, LONGLONG Offset, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorFlushFileBuffers(LPCWSTR	FileName, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorGetFileInformation(LPCWSTR FileName, LPBY_HANDLE_FILE_INFORMATION HandleFileInformation, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorFindFiles(LPCWSTR FileName, PFillFindData FillFindData, PDOKAN_FILE_INFO	DokanFileInfo);

    NTSTATUS MirrorDeleteFile(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorDeleteDirectory(LPCWSTR FileName, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorMoveFile(LPCWSTR	FileName, LPCWSTR NewFileName, BOOL ReplaceIfExisting, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorLockFile(LPCWSTR	FileName, LONGLONG ByteOffset, LONGLONG Length, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorSetEndOfFile(LPCWSTR	FileName, LONGLONG ByteOffset, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorSetAllocationSize(LPCWSTR FileName, LONGLONG	AllocSize, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorSetFileAttributes(LPCWSTR FileName, DWORD FileAttributes, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorSetFileTime(LPCWSTR FileName, CONST FILETIME* CreationTime, CONST FILETIME* LastAccessTime, CONST FILETIME* LastWriteTime, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorUnlockFile(LPCWSTR FileName, LONGLONG ByteOffset, LONGLONG	Length, PDOKAN_FILE_INFO	DokanFileInfo);

    NTSTATUS MirrorGetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG BufferLength, PULONG	LengthNeeded, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorSetFileSecurity(LPCWSTR FileName, PSECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR SecurityDescriptor, ULONG SecurityDescriptorLength, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorGetVolumeInformation(LPWSTR VolumeNameBuffer, DWORD VolumeNameSize, LPDWORD VolumeSerialNumber, LPDWORD	MaximumComponentLength, LPDWORD	FileSystemFlags, LPWSTR	FileSystemNameBuffer, DWORD	FileSystemNameSize, PDOKAN_FILE_INFO DokanFileInfo);

    NTSTATUS MirrorUnmount(PDOKAN_FILE_INFO	DokanFileInfo);

    NTSTATUS MirrorEnumerateNamedStreams(LPCWSTR FileName, PVOID* EnumContext, LPWSTR StreamName, PULONG StreamNameLength, PLONGLONG StreamSize, PDOKAN_FILE_INFO DokanFileInfo);
};

#endif // LOCALDRIVE_H

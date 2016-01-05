#include "DokanDrive.h"
#include <QDebug>

DokanDriveImplementation *DokanMirrorDriveInterface::m_implementation = NULL;

int DokanMirrorDriveInterface::mount(DokanDriveImplementation *implementation)
{
    DokanMirrorDriveInterface::m_implementation = implementation;
    PDOKAN_OPERATIONS dokanOperations = (PDOKAN_OPERATIONS)malloc(sizeof(DOKAN_OPERATIONS));
    if (dokanOperations == NULL)
    {
        qDebug() << "can't allocate dokanOperations";
        return EXIT_FAILURE;
    }

    PDOKAN_OPTIONS dokanOptions = (PDOKAN_OPTIONS)malloc(sizeof(DOKAN_OPTIONS));
    if (dokanOptions == NULL)
    {
        qDebug() << "can't allocate dokanOptions";
        free(dokanOperations);
        return EXIT_FAILURE;
    }

    ZeroMemory(dokanOptions, sizeof(DOKAN_OPTIONS));
    dokanOptions->Version = DOKAN_VERSION;
    dokanOptions->ThreadCount = 1;
    QString mountPoint = "m";
    dokanOptions->MountPoint = (const wchar_t*) mountPoint.utf16();
    //dokanOptions->Options |= DOKAN_OPTION_DEBUG;
    dokanOptions->Options |= DOKAN_OPTION_STDERR;
    dokanOptions->Options |= DOKAN_OPTION_ALT_STREAM;

    ZeroMemory(dokanOperations, sizeof(DOKAN_OPERATIONS));

    dokanOperations->ZwCreateFile = DokanMirrorDriveInterface::MirrorCreateFile;
    dokanOperations->Cleanup = DokanMirrorDriveInterface::MirrorCleanup;
    dokanOperations->CloseFile = DokanMirrorDriveInterface::MirrorCloseFile;
    dokanOperations->ReadFile = DokanMirrorDriveInterface::MirrorReadFile;
    dokanOperations->WriteFile = DokanMirrorDriveInterface::MirrorWriteFile;
    dokanOperations->FlushFileBuffers = DokanMirrorDriveInterface::MirrorFlushFileBuffers;
    dokanOperations->GetFileInformation = DokanMirrorDriveInterface::MirrorGetFileInformation;
    dokanOperations->FindFiles = DokanMirrorDriveInterface::MirrorFindFiles;
    dokanOperations->FindFilesWithPattern = NULL;
    dokanOperations->SetFileAttributes = DokanMirrorDriveInterface::MirrorSetFileAttributes;
    dokanOperations->SetFileTime = DokanMirrorDriveInterface::MirrorSetFileTime;
    dokanOperations->DeleteFile = DokanMirrorDriveInterface::MirrorDeleteFile;
    dokanOperations->DeleteDirectory = DokanMirrorDriveInterface::MirrorDeleteDirectory;
    dokanOperations->MoveFile = DokanMirrorDriveInterface::MirrorMoveFile;
    dokanOperations->SetEndOfFile = DokanMirrorDriveInterface::MirrorSetEndOfFile;
    dokanOperations->SetAllocationSize = DokanMirrorDriveInterface::MirrorSetAllocationSize;
    dokanOperations->LockFile = DokanMirrorDriveInterface::MirrorLockFile;
    dokanOperations->UnlockFile = DokanMirrorDriveInterface::MirrorUnlockFile;
    dokanOperations->GetFileSecurity = DokanMirrorDriveInterface::MirrorGetFileSecurity;
    dokanOperations->SetFileSecurity = DokanMirrorDriveInterface::MirrorSetFileSecurity;
    dokanOperations->GetDiskFreeSpace = NULL;
    dokanOperations->GetVolumeInformation = DokanMirrorDriveInterface::MirrorGetVolumeInformation;
    dokanOperations->Unmount = DokanMirrorDriveInterface::MirrorUnmount;

    int status = DokanMain(dokanOptions, dokanOperations);

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

    return EXIT_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorCreateFile(
    LPCWSTR FileName,
    PDOKAN_IO_SECURITY_CONTEXT SecurityContext,
    ACCESS_MASK AccessMode,
    ULONG FileAttributes,
    ULONG ShareMode,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PDOKAN_FILE_INFO DokanFileInfo)
{
    return m_implementation->MirrorCreateFile(FileName, SecurityContext, AccessMode, FileAttributes, ShareMode, CreateDisposition, CreateOptions, DokanFileInfo);
}

void DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorCloseFile(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    m_implementation->MirrorCloseFile(FileName, DokanFileInfo);
}

void DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorCleanup(
    LPCWSTR					FileName,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    m_implementation->MirrorCleanup(FileName, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorReadFile(
    LPCWSTR				FileName,
    LPVOID				Buffer,
    DWORD				BufferLength,
    LPDWORD				ReadLength,
    LONGLONG			Offset,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorReadFile(FileName, Buffer, BufferLength, ReadLength, Offset, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorWriteFile(
    LPCWSTR		FileName,
    LPCVOID		Buffer,
    DWORD		NumberOfBytesToWrite,
    LPDWORD		NumberOfBytesWritten,
    LONGLONG			Offset,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorWriteFile(FileName, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten, Offset, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorFlushFileBuffers(
    LPCWSTR		FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorFlushFileBuffers(FileName, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorGetFileInformation(
    LPCWSTR							FileName,
    LPBY_HANDLE_FILE_INFORMATION	HandleFileInformation,
    PDOKAN_FILE_INFO				DokanFileInfo)
{
    return m_implementation->MirrorGetFileInformation(FileName, HandleFileInformation, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorFindFiles(
    LPCWSTR				FileName,
    PFillFindData		FillFindData, // function pointer
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorFindFiles(FileName, FillFindData, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorDeleteFile(
    LPCWSTR				FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorDeleteFile(FileName, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorDeleteDirectory(
    LPCWSTR				FileName,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorDeleteDirectory(FileName, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorMoveFile(
    LPCWSTR				FileName, // existing file name
    LPCWSTR				NewFileName,
    BOOL				ReplaceIfExisting,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorMoveFile(FileName, NewFileName, ReplaceIfExisting, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorLockFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    LONGLONG			Length,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorLockFile(FileName, ByteOffset, Length, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorSetEndOfFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorSetEndOfFile(FileName, ByteOffset, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorSetAllocationSize(
    LPCWSTR				FileName,
    LONGLONG			AllocSize,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorSetAllocationSize(FileName, AllocSize, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorSetFileAttributes(
    LPCWSTR				FileName,
    DWORD				FileAttributes,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorSetFileAttributes(FileName, FileAttributes, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorSetFileTime(
    LPCWSTR				FileName,
    CONST FILETIME*		CreationTime,
    CONST FILETIME*		LastAccessTime,
    CONST FILETIME*		LastWriteTime,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorSetFileTime(FileName, CreationTime, LastAccessTime, LastWriteTime, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorUnlockFile(
    LPCWSTR				FileName,
    LONGLONG			ByteOffset,
    LONGLONG			Length,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorUnlockFile(FileName, ByteOffset, Length, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorGetFileSecurity(
    LPCWSTR					FileName,
    PSECURITY_INFORMATION	SecurityInformation,
    PSECURITY_DESCRIPTOR	SecurityDescriptor,
    ULONG				BufferLength,
    PULONG				LengthNeeded,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorGetFileSecurity(FileName, SecurityInformation, SecurityDescriptor, BufferLength, LengthNeeded, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorSetFileSecurity(
    LPCWSTR					FileName,
    PSECURITY_INFORMATION	SecurityInformation,
    PSECURITY_DESCRIPTOR	SecurityDescriptor,
    ULONG				SecurityDescriptorLength,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorSetFileSecurity(FileName, SecurityInformation, SecurityDescriptor, SecurityDescriptorLength, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorGetVolumeInformation(
    LPWSTR		VolumeNameBuffer,
    DWORD		VolumeNameSize,
    LPDWORD		VolumeSerialNumber,
    LPDWORD		MaximumComponentLength,
    LPDWORD		FileSystemFlags,
    LPWSTR		FileSystemNameBuffer,
    DWORD		FileSystemNameSize,
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorGetVolumeInformation(VolumeNameBuffer, VolumeNameSize, VolumeSerialNumber, MaximumComponentLength, FileSystemFlags, FileSystemNameBuffer, FileSystemNameSize, DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorUnmount(
    PDOKAN_FILE_INFO	DokanFileInfo)
{
    return m_implementation->MirrorUnmount(DokanFileInfo);
}

NTSTATUS DOKAN_CALLBACK
DokanMirrorDriveInterface::MirrorEnumerateNamedStreams(
    LPCWSTR					FileName,
    PVOID*					EnumContext,
    LPWSTR					StreamName,
    PULONG					StreamNameLength,
    PLONGLONG				StreamSize,
    PDOKAN_FILE_INFO		DokanFileInfo)
{
    return m_implementation->MirrorEnumerateNamedStreams(FileName, EnumContext, StreamName, StreamNameLength, StreamSize, DokanFileInfo);
}

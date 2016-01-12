QT += core network
QT -= gui

TARGET = Mirror
CONFIG += console
CONFIG -= app_bundle
DEFINES += UNICODE
DEFINES += _UNICODE

TEMPLATE = app

INCLUDEPATH += C:/code/dokan_tot/dokany/dokan \
               C:/code/dokan_tot/dokany/sys

SOURCES += \
    main.cpp \
    NetworkDrive.cpp \
    NetworkDriveServer.cpp \
    NetworkDriveClient.cpp \
    FileSystemThread.cpp \
    NetworkThread.cpp \
    ../Common/DokanDrive.cpp \
    ../Common/DokanDriveImplementation.cpp

HEADERS += \
    NetworkDrive.h \
    NetworkDriveServer.h \
    NetworkDriveProtocol.h \
    NetworkDriveClient.h \
    FileSystemThread.h \
    NetworkThread.h \
    ../Common/DokanDrive.h \
    ../Common/DokanDriveImplementation.h

LIBS += -lUser32 \
        -lAdvapi32 \
        -lntdll

LIBS += -LC:/code/dokan_tot/dokany/x64/Debug \
        -ldokan

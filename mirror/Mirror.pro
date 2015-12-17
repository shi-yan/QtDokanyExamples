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
    DokanDrive.cpp \
    DokanDriveImplementation.cpp \
    LocalDrive.cpp \
    main.cpp \
    NetworkDrive.cpp \
    NetworkDriveServer.cpp \
    NetworkDriveClient.cpp

HEADERS += \
    DokanDrive.h \
    DokanDriveImplementation.h \
    LocalDrive.h \
    NetworkDrive.h \
    NetworkDriveServer.h \
    NetworkDriveProtocol.h \
    NetworkDriveClient.h

LIBS += -lUser32 \
        -lAdvapi32 \
        -lntdll

LIBS += -LC:/code/dokan_tot/dokany/x64/Debug \
        -ldokan

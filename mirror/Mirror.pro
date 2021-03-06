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
    FileSystemThread.cpp \
    ../Common/DokanDrive.cpp \
    ../Common/DokanDriveImplementation.cpp \
    MirrorDrive.cpp

HEADERS += \
    FileSystemThread.h \
    ../Common/DokanDrive.h \
    ../Common/DokanDriveImplementation.h \
    MirrorDrive.h

LIBS += -lUser32 \
        -lAdvapi32 \
        -lntdll

LIBS += -LC:/code/dokan_tot/dokany/x64/Debug \
        -ldokan

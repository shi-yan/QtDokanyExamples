#-------------------------------------------------
#
# Project created by QtCreator 2016-01-11T14:21:05
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetworkDriveClient
TEMPLATE = app

INCLUDEPATH += C:/code/dokan_tot/dokany/dokan \
               C:/code/dokan_tot/dokany/sys

SOURCES += main.cpp\
    ../NetworkDriveServer/NetworkDriveClient.cpp \
    NetworkDriveDialog.cpp

HEADERS  += \
    ../NetworkDriveServer/NetworkDriveClient.h \
    ../NetworkDriveServer/NetworkDriveProtocol.h \
    NetworkDriveDialog.h

FORMS    += \
    Dialog.ui

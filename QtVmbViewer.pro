#-------------------------------------------------
#
# Project created by QtCreator 2017-02-07T16:33:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Vimba SDK
INCLUDEPATH += /opt/Vimba_2_0/VimbaC/Include
LIBS += -L/opt/Vimba_2_0/VimbaC/DynamicLib/x86_64bit -lVimbaC


TARGET = QtVmbViewer
TEMPLATE = app


SOURCES += main.cpp\
        qtvmbviewer.cpp \
    VmbCamera.cpp

HEADERS  += qtvmbviewer.h \
    VmbCamera.h

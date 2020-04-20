#-------------------------------------------------
#
# Project created by QtCreator 2018-04-29T13:29:22
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = soxnav5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    genericclient.cpp \
    setupgui.cpp \
    utility.cpp \
    airplane.cpp

HEADERS  += mainwindow.h \
    genericclient.h \
    setupgui.h \
    constants.h \
    utility.h \
    airplane.h

FORMS    += mainwindow.ui

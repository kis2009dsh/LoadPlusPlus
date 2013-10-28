#-------------------------------------------------
#
# Project created by QtCreator 2013-09-16T10:06:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PresureTest
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    ../dialog.cpp \
    dialog.cpp \
    datachart.cpp \
    datachart.cpp

HEADERS  += mainwindow.h \
    ../dialog.h \
    dialog.h \
    datachart.h

FORMS    += mainwindow.ui \
    dialog.ui

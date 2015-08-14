#-------------------------------------------------
#
# Project created by QtCreator 2012-05-03T15:27:31
#
#-------------------------------------------------

QT       += core gui
QT       += sql

TARGET = auditview
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    auditcompsmodel.cpp \
    softwaredetails.cpp \
    netadapterwindow.cpp \
    softwarefind.cpp

HEADERS  += mainwindow.h \
    auditcompsmodel.h \
    softwaredetails.h \
    netadapterwindow.h \
    softwarefind.h

FORMS    += mainwindow.ui \
    softwaredetails.ui \
    netadapterwindow.ui \
    softwarefind.ui

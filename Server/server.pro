# server.pro
QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

SOURCES += server.cpp \
    ../Gobang.cpp \
    ../networkmanager.cpp

HEADERS += \
    ../Gobang.h \
    ../networkmanager.h

RESOURCES += ../resources.qrc
FORMS += ../GobangUI.ui

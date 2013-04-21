#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T11:40:32
#
#-------------------------------------------------

QT       += xml

QT       -= gui

TARGET = atom
TEMPLATE = lib

CONFIG += link_pkgconfig plugin mobility

MOBILITY += contacts

DEFINES += ATOM_LIBRARY

SOURCES += Atom.cpp

HEADERS += Atom.h\
        atom_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

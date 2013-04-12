#-------------------------------------------------
#
# Buteo Google contacts synchronization plugin
#
#-------------------------------------------------

QT       += network sql

QT       -= gui

TARGET = buteo-gcontact-plugin
TEMPLATE = lib

CONFIG += link_pkgconfig plugin mobility
PKGCONFIG += buteosyncfw

MOBILITY += contacts

VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

QMAKE_CXXFLAGS = -Wall \
    -g \
    -Wno-cast-align \
    -O2 -finline-functions

DEFINES += BUTEOGCONTACTPLUGIN_LIBRARY

SOURCES += GContactClient.cpp \
    GAuth.cpp \
    GTransport.cpp \
    GContact.cpp

HEADERS += GContactClient.h\
        buteo-gcontact-plugin_global.h \
    GAuth.h \
    GTransport.h \
    GContact.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib/buteo-plugins
    }
    INSTALLS += target
}

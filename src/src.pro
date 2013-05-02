#-------------------------------------------------
#
# Buteo Google contacts synchronization plugin
#
#-------------------------------------------------

QT       += network sql xml

QT       -= gui

TARGET = buteo-gcontact-plugin
TEMPLATE = lib

$$PKG_CONFIG_PATH = $$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig/

CONFIG += link_pkgconfig plugin mobility
PKGCONFIG += buteosyncfw QJson

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
    GContactEntry.cpp \
    GParseStream.cpp \
    GAtom.cpp \
    GContactsBackend.cpp

HEADERS += GContactClient.h\
        buteo-gcontact-plugin_global.h \
    GAuth.h \
    GTransport.h \
    GContactEntry.h \
    GParseStream.h \
    GAtom.h \
    GContactsBackend.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib/buteo-plugins
    }
    INSTALLS += target
}

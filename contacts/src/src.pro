#-------------------------------------------------
#
# Buteo Google contacts synchronization plugin
#
#-------------------------------------------------

QT       += network sql xml

QT       -= gui

TARGET = googlecontacts-client
TEMPLATE = lib

#$$PKG_CONFIG_PATH = $$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig/

CONFIG += link_pkgconfig plugin mobility debug
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
    GContactEntry.cpp \
    GParseStream.cpp \
    GAtom.cpp \
    GContactsBackend.cpp \
    GContactCustomDetail.cpp

HEADERS += GContactClient.h\
        buteo-gcontact-plugin_global.h \
    GAuth.h \
    GTransport.h \
    GContactEntry.h \
    GParseStream.h \
    GAtom.h \
    GContactsBackend.h \
    GContactDetail.h \
    GContactCustomDetail.h

target.path = /usr/lib/buteo-plugins

sync.path = /etc/buteo/profiles/sync
sync.files = xmls/sync/*

client.path = /etc/buteo/profiles/client
client.files = xmls/client/*

INSTALLS += target sync client

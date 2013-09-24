#-------------------------------------------------
#
# Buteo Google contacts synchronization plugin
#
#-------------------------------------------------

QT       += contacts network

QT       -= gui

TARGET = googlecontacts-client
TEMPLATE = lib

#$$PKG_CONFIG_PATH = $$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig/

CONFIG += link_pkgconfig plugin debug console
PKGCONFIG += buteosyncfw5 libsignon-qt5 accounts-qt5 signon-oauth2plugin libsailfishkeyprovider qtcontacts-sqlite-qt5-extensions

VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

#DEFINES += CUSTOM_DETAIL_IS_SUPPORTED

QMAKE_CXXFLAGS = -Wall \
    -g \
    -Wno-cast-align \
    -O2 -finline-functions

DEFINES += BUTEOGCONTACTPLUGIN_LIBRARY

SOURCES += GContactClient.cpp \
    GWriteStream.cpp \
    GAuth.cpp \
    GTransport.cpp \
    GContactEntry.cpp \
    GParseStream.cpp \
    GAtom.cpp \
    GContactsBackend.cpp \
    GContactCustomDetail.cpp

HEADERS += GContactClient.h\
        buteo-gcontact-plugin_global.h \
        GWriteStream.h \
        GAuth.h \
        GTransport.h \
        GContactEntry.h \
        GParseStream.h \
        GAtom.h \
        GContactsBackend.h \
        GContactDetail.h \
        GContactCustomDetail.h \
        GConfig.h


target.path = /usr/lib/buteo-plugins-qt5

sync.path = /etc/buteo/profiles/sync
sync.files = xmls/sync/*

client.path = /etc/buteo/profiles/client
client.files = xmls/client/*

INSTALLS += target sync client

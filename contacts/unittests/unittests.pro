TEMPLATE = app
TARGET =  sync-google-contacts-tests

QT       += contacts network testlib
CONFIG += debug link_pkgconfig console
QT -= gui

PKGCONFIG += buteosyncfw5

INCLUDEPATH += ../src

LIBS += -L../src \
        -lgooglecontacts-client

SOURCES +=  main.cpp \
    GAtomTest.cpp \
    GParseStreamTest.cpp \
    GTransportTest.cpp \
    GContactClientTest.cpp \
    GContactsBackendTest.cpp \
    GWriteStreamTest.cpp

HEADERS += GAtomTest.h \
    GParseStreamTest.h \
    GTransportTest.h \
    GContactClientTest.h \
    GContactsBackendTest.h \
    GWriteStreamTest.h

QMAKE_CXXFLAGS = -Wall \
    -Wno-cast-align \
    -O0 -finline-functions

target.files += sync-google-contacts-tests \
                runstarget.sh
tests.files   = contact_v3.xml
tests.path    = /opt/tests/buteo-sync-plugins-google/

testdefinition.files = tests.xml
testdefinition.path  = /opt/tests/buteo-sync-plugins-google/test-definition

target.path = /opt/tests/buteo-sync-plugins-google/

INSTALLS += tests \
            target \
            testdefinition

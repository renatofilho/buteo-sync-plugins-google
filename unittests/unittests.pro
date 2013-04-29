TEMPLATE = app

CONFIG += qtestlib mobility debug
MOBILITY += contacts

INCLUDEPATH += ../src

LIBS += -L../src \
        -lbuteo-gcontact-plugin

SOURCES +=  \
    main.cpp \
    GAtomTest.cpp \
    GParseStreamTest.cpp

HEADERS += \
    GAtomTest.h \
    GParseStreamTest.h

QMAKE_CXXFLAGS = -Wall \
    -Wno-cast-align \
    -O0 -finline-functions

TEMPLATE = lib
CONFIG += plugin
#CONFIG += debug
DESTDIR = ..

HEADERS += \
    utils/coordinates.h \
    utils/config.h \
    blockcache.h \
    binaryheap.h \
    interfaces/irouter.h \
    contractionhierarchiesclient.h \
    compressedgraph.h \
    interfaces/igpslookup.h \
    utils/bithelpers.h \
    utils/qthelpers.h

SOURCES += \
    contractionhierarchiesclient.cpp

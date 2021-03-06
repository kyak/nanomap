include(../../nanomap.pri)

TEMPLATE = lib
CONFIG += plugin
#CONFIG += debug
DESTDIR = ..
unix {
	QMAKE_CXXFLAGS_RELEASE -= -O2
	QMAKE_CXXFLAGS_RELEASE += -O3 \
		 -Wno-unused-function
	QMAKE_CXXFLAGS_DEBUG += -Wno-unused-function
}

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

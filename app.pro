include(nanomap.pri)

TARGET = NanoMap
TEMPLATE = app

QT += network

#LIBS += -Lmonav -lgpsgridclient -lcontractionhierarchiesclient
INCLUDEPATH += monav

SOURCES += main.cpp \
            mainwidget.cpp \
            projection.cpp \
            abstractlayer.cpp \
            gpslayer.cpp \
            markerlayer.cpp \
            gpxlayer.cpp \
            poilayer.cpp \
            monavlayer.cpp \
            timelayer.cpp \
            batterylayer.cpp \
            mapwidget.cpp \
            markerlist.cpp \
            downloadwidget.cpp \
            searchwidget.cpp \
            gpsclient.cpp

HEADERS += mainwidget.h \
            projection.h \
            abstractlayer.h \
            gpslayer.h \
            markerlayer.h \
            gpxlayer.h \
            poilayer.h \
            monavlayer.h \
            timelayer.h \
            batterylayer.h \
            mapwidget.h \
            markerlist.h \
            downloadwidget.h \
            searchwidget.h \
            gpsclient.h

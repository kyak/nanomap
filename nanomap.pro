TARGET = NanoMap
TEMPLATE = app

QT += network

SOURCES += main.cpp \
            mainwidget.cpp \
            projection.cpp \
            abstractlayer.cpp \
            markerlayer.cpp \
            gpxlayer.cpp \
            timelayer.cpp \
            batterylayer.cpp \
            mapwidget.cpp \
            markerlist.cpp \
            downloadwidget.cpp \
            gpsclient.cpp

HEADERS += mainwidget.h \
            projection.h \
            abstractlayer.h \
            markerlayer.h \
            gpxlayer.h \
            timelayer.h \
            batterylayer.h \
            mapwidget.h \
            markerlist.h \
            downloadwidget.h \
            gpsclient.h

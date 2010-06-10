TARGET = NanoMap
TEMPLATE = app

QT += network

SOURCES += main.cpp \
            mainwidget.cpp \
            mapwidget.cpp \
            markerlist.cpp \
            downloadwidget.cpp \
            gpsclient.cpp

HEADERS += mainwidget.h \
            mapwidget.h \
            markerlist.h \
            downloadwidget.h \
            gpsclient.h

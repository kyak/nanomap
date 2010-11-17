#-------------------------------------------------
#
# Project created by QtCreator 2010-06-25T08:48:06
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += plugin
#CONFIG += debug

DESTDIR = ..

HEADERS += \
    utils/coordinates.h \
    utils/config.h \
    cell.h \
    interfaces/igpslookup.h \
    gpsgridclient.h \
    table.h \
    utils/bithelpers.h \
    utils/qthelpers.h

SOURCES += \
    gpsgridclient.cpp

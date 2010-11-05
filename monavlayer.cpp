/*
 * Copyright 2010  Niels Kummerfeldt <niels.kummerfeldt@tu-harburg.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "monavlayer.h"

#include "mapwidget.h"
#include "projection.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPluginLoader>
#include <QtCore/QSettings>

MonavLayer::MonavLayer(MapWidget *map) :
    AbstractLayer(map),
    m_gpsLookup(0),
    m_router(0),
    m_loaded(true),
    m_routeStart(),
    m_routeEnd(),
    m_track(),
    m_trackOnScreen(),
    m_trackOffset(),
    m_zoomLevel(0)
{
    QSettings set(QDir::homePath()+"/Maps/nanomap.conf", QSettings::NativeFormat);
    set.beginGroup("monav");
    QString dataDir = set.value("datadir").toString();
    QString routerLib = set.value("router",
            "/usr/lib/monav/libcontractionhierarchiesclient.so").toString();
    QString gpsLookupLib = set.value("gpslookup",
            "/usr/lib/monav/libgpsgridclient.so").toString();
    set.endGroup();

    QSettings pluginSettings(dataDir+"/plugins.ini", QSettings::IniFormat);
    QString routerName = pluginSettings.value("router").toString();
    QString gpsLookupName = pluginSettings.value("gpsLookup").toString();

    QPluginLoader rLoader(routerLib);
    QObject *plugin = rLoader.instance();
    if (plugin) {
        m_router = qobject_cast<IRouter*>(plugin);
        if (m_router) {
            m_router->SetInputDirectory(dataDir);
            m_loaded = m_loaded && m_router->LoadData();
        } else {
            m_loaded = false;
        }
    }
    QPluginLoader gLoader(gpsLookupLib);
    plugin = gLoader.instance();
    if (plugin) {
        m_gpsLookup = qobject_cast<IGPSLookup*>(plugin);
        if (m_gpsLookup) {
            m_gpsLookup->SetInputDirectory(dataDir);
            m_loaded = m_loaded && m_gpsLookup->LoadData();
        } else {
            m_loaded = false;
        }
    }
}

void MonavLayer::zoom(int level)
{
    m_zoomLevel = level;
    if (m_track.count() > 1) {
        int scale = 1 << level;
        m_trackOnScreen.clear();
        m_trackOffset = map()->raw2screen(m_track.first().x(), m_track.first().y(), scale);
        m_trackOnScreen << QPoint(0, 0);
        for (int i = 1; i < m_track.count(); ++i) {
            QPointF p = m_track.at(i);
            m_trackOnScreen << map()->raw2screen(p.x(), p.y(), scale) - m_trackOffset;
        }
    }
}

void MonavLayer::pan(const QPoint &move)
{
    m_trackOffset += move;
}

void MonavLayer::paint(QPainter *painter)
{
    if (!m_loaded) {
        return;
    }
    if (m_trackOnScreen.count() > 1) {
        QPoint p1, p2 = m_trackOnScreen.first();
        for (int i = 1; i < m_trackOnScreen.count(); ++i) {
            p1 = m_trackOnScreen.at(i);
            painter->drawLine(p1 + m_trackOffset, p2 + m_trackOffset);
            p2 = p1;
        }
    }

    QPoint p;
    QPolygon tri;

    if (!m_routeStart.isNull()) {
        p = map()->geo2screen(m_routeStart.x(), m_routeStart.y());
        tri << p << p+QPoint(-5, -9) << p+QPoint(5, -9) << p;
        painter->setBrush(Qt::red);
        painter->drawPolygon(tri);
    }

    if (!m_routeEnd.isNull()) {
        p = map()->geo2screen(m_routeEnd.x(), m_routeEnd.y());
        tri.clear();
        tri << p << p+QPoint(-5, -9) << p+QPoint(5, -9) << p;
        painter->setBrush(Qt::blue);
        painter->drawPolygon(tri);
    }
}

void MonavLayer::keyPressed(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_R:
        {
            if (event->modifiers() == Qt::NoModifier) {
                findRoute();
            }
            break;
        }
        case Qt::Key_S:
        {
            if (event->modifiers() == Qt::NoModifier) {
                QPointF p = map()->geoPos();
                UnsignedCoordinate coord(GPSCoordinate(p.y(), p.x()));
                IGPSLookup::Result pos;
                if (m_gpsLookup->GetNearestEdge(&pos, coord, 1000.0)) {
                    m_routeStart = p;
                }
            }
            break;
        }
        case Qt::Key_E:
        {
            if (event->modifiers() == Qt::NoModifier) {
                QPointF p = map()->geoPos();
                UnsignedCoordinate coord(GPSCoordinate(p.y(), p.x()));
                IGPSLookup::Result pos;
                if (m_gpsLookup->GetNearestEdge(&pos, coord, 1000.0)) {
                    m_routeEnd = p;
                }
            }
            break;
        }
    }
}

void MonavLayer::findRoute()
{
    if (!m_loaded) {
        return;
    }

    QVector<IRouter::Node> nodes;
    QVector<IRouter::Edge> edges;
    double lookupRadius = 1000.0;
    double dist;

    UnsignedCoordinate startCoord(GPSCoordinate(m_routeStart.y(), m_routeStart.x()));
    IGPSLookup::Result startPos;
    if (!m_gpsLookup->GetNearestEdge(&startPos, startCoord, lookupRadius)) {
        qDebug() << "source not found";
        return;
    }
    UnsignedCoordinate endCoord(GPSCoordinate(m_routeEnd.y(), m_routeEnd.x()));
    IGPSLookup::Result endPos;
    if (!m_gpsLookup->GetNearestEdge(&endPos, endCoord, lookupRadius)) {
        qDebug() << "target not found";
        return;
    }
    if (m_router->GetRoute(&dist, &nodes, &edges, startPos, endPos)) {
        qDebug() << "route found";
        m_track.clear();
        for (int j = 0; j < nodes.size(); ++j) {
            GPSCoordinate c = nodes[j].coordinate.ToGPSCoordinate();
            m_track << QPointF(Projection::lon2rawx(c.longitude), Projection::lat2rawy(c.latitude));
        }
        zoom(m_zoomLevel);
    }
}


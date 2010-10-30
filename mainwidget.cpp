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

#include "mainwidget.h"

#include "downloadwidget.h"
#include "mapwidget.h"
#include "markerlist.h"
#include "routingwidget.h"

#include "batterylayer.h"
#include "gpslayer.h"
#include "gpxlayer.h"
#include "markerlayer.h"
#include "poilayer.h"
#include "timelayer.h"

#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QLayout>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),
    m_stack(new QStackedWidget(this)),
    m_map(new MapWidget(this)),
    m_markerList(new MarkerList(this)),
    m_dlWidget(new DownloadWidget(this)),
    m_routingWidget(new RoutingWidget(this))
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_stack);

    AbstractLayer *l = new TimeLayer(m_map);
    l->setVisible(false);
    m_map->addLayer(l, 4, "Time");

    l = new BatteryLayer(m_map);
    l->setVisible(false);
    m_map->addLayer(l, 4, "Battery");

    l = new MarkerLayer(m_map);
    connect(l, SIGNAL(markerAdded(QString)), m_markerList, SLOT(addMarker(QString)));
    connect(m_markerList, SIGNAL(centerOnMarker(int)), l, SLOT(centerOnMarker(int)));
    connect(m_markerList, SIGNAL(removeMarker(int)), l, SLOT(removeMarker(int)));
    connect(m_markerList, SIGNAL(markerRenamed(int, QString)), l, SLOT(renameMarker(int, QString)));
    l->load(QDir::homePath()+"/Maps/marker.list");
    m_map->addLayer(l, 3, "Marker");

    l = new GpsLayer(m_map);
    m_map->addLayer(l, 1, "GPS-Position");

    connect(m_map, SIGNAL(close()), this, SIGNAL(close()));
    connect(m_map, SIGNAL(showMarkerList()), this, SLOT(showList()));
    connect(m_map, SIGNAL(downloadArea(int, QRectF)), this, SLOT(downloadArea(int, QRectF)));
    connect(m_map, SIGNAL(route(QPointF, QPointF)), this, SLOT(findRoute(QPointF, QPointF)));
    m_stack->insertWidget(0, m_map);

    connect(m_markerList, SIGNAL(back()), this, SLOT(showMap()));
    connect(m_markerList, SIGNAL(centerOnMarker(int)), this, SLOT(showMap()));
    m_stack->insertWidget(1, m_markerList);

    connect(m_dlWidget, SIGNAL(back()), this, SLOT(showMap()));
    m_stack->insertWidget(2, m_dlWidget);

    connect(m_routingWidget, SIGNAL(back()), this, SLOT(showMap()));
    m_stack->insertWidget(3, m_routingWidget);

    resize(320, 240);
}

MainWidget::~MainWidget()
{
}

void MainWidget::loadFile(const QString &fileName)
{
    if (fileName.endsWith(".gpx")) {
        AbstractLayer *l = new GpxLayer(m_map);
        l->load(fileName);
        m_map->addLayer(l, 2, "GPS-Track");
    } else if (fileName.endsWith(".osm")) {
        AbstractLayer *l = new PoiLayer(m_map);
        l->load(fileName);
        m_map->addLayer(l, 3, "Points Of Interest");
    }
}

void MainWidget::showList()
{
    m_stack->setCurrentIndex(1);
}

void MainWidget::markerAdded(const QString &name)
{
    m_markerList->addMarker(name);
}

void MainWidget::showMap()
{
    m_stack->setCurrentIndex(0);
}

void MainWidget::downloadArea(int level, const QRectF &rect)
{
    m_dlWidget->setStartLevel(level);
    m_dlWidget->setDownloadRect(rect);
    m_stack->setCurrentIndex(2);
}

void MainWidget::findRoute(const QPointF &from, const QPointF &to)
{
    m_routingWidget->setFrom(from);
    m_routingWidget->setTo(to);

    m_stack->setCurrentIndex(3);
}


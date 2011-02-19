/*
 * Copyright 2010-2011  Niels Kummerfeldt <niels.kummerfeldt@tu-harburg.de>
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
#include "fileselector.h"
#include "mapwidget.h"
#include "markerlist.h"
#include "searchwidget.h"

#include "batterylayer.h"
#include "gpslayer.h"
#include "gpxlayer.h"
#include "markerlayer.h"
#include "monavlayer.h"
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
    m_fileSelector(new FileSelector(this)),
    m_search(new SearchWidget(this))
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

    l = new MonavLayer(m_map);
    m_map->addLayer(l, 2, "MoNav Routing");

    l = new GpsLayer(m_map);
    m_map->addLayer(l, 1, "GPS-Position");

    connect(m_map, SIGNAL(close()), this, SIGNAL(close()));
    connect(m_map, SIGNAL(showMarkerList()), this, SLOT(showList()));
    connect(m_map, SIGNAL(downloadArea(int, QRectF)), this, SLOT(downloadArea(int, QRectF)));
    connect(m_map, SIGNAL(loadFile()), this, SLOT(showFileSelector()));
    connect(m_map, SIGNAL(search()), this, SLOT(search()));
    m_stack->insertWidget(0, m_map);

    connect(m_markerList, SIGNAL(back()), this, SLOT(showMap()));
    connect(m_markerList, SIGNAL(centerOnMarker(int)), this, SLOT(showMap()));
    m_stack->insertWidget(1, m_markerList);

    connect(m_dlWidget, SIGNAL(back()), this, SLOT(showMap()));
    connect(m_dlWidget, SIGNAL(loadFile(QString, QString)), this, SLOT(loadFile(QString, QString)));
    m_stack->insertWidget(2, m_dlWidget);

    connect(m_search, SIGNAL(back()), this, SLOT(showMap()));
    connect(m_search, SIGNAL(centerOn(qreal, qreal)), this, SLOT(showMap(qreal, qreal)));
    m_stack->insertWidget(3, m_search);

    m_fileSelector->setTitle("Open POI / Track file");
    m_fileSelector->setFileTypes(QStringList() << "*.gpx" << "*.osm");
    connect(m_fileSelector, SIGNAL(cancel()), this, SLOT(showMap()));
    connect(m_fileSelector, SIGNAL(fileSelected(QString)), this, SLOT(loadFile(QString)));
    m_stack->insertWidget(4, m_fileSelector);

    resize(320, 240);
}

MainWidget::~MainWidget()
{
}

void MainWidget::loadFile(const QString &fileName)
{
    loadFile(fileName, "");
}

void MainWidget::loadFile(const QString &fileName, const QString &title)
{
    if (fileName.endsWith(".gpx")) {
        AbstractLayer *l = new GpxLayer(m_map);
        l->load(fileName);
        QString t = title.isEmpty() ? "GPS-Track" : title;
        m_map->addLayer(l, 2, t);
    } else if (fileName.endsWith(".osm")) {
        AbstractLayer *l = new PoiLayer(m_map);
        l->load(fileName);
        QString t = title.isEmpty() ? "Points Of Interest" : title;
        m_map->addLayer(l, 3, t);
    }
    showMap();
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
    m_map->setFocus(Qt::OtherFocusReason);
}

void MainWidget::showMap(qreal lon, qreal lat)
{
    m_map->centerOnGeoPos(lon, lat);
    showMap();
}

void MainWidget::downloadArea(int level, const QRectF &rect)
{
    m_dlWidget->setStartLevel(level);
    m_dlWidget->setDownloadRect(rect);
    m_stack->setCurrentIndex(2);
}

void MainWidget::search()
{
    m_stack->setCurrentIndex(3);
}

void MainWidget::showFileSelector()
{
    m_stack->setCurrentIndex(4);
}


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

#include "gpslayer.h"

#include "gpsclient.h"
#include "mapwidget.h"

#include <QtCore/QPoint>

GpsLayer::GpsLayer(MapWidget *map) :
    AbstractLayer(map),
    m_gps(new GpsClient(this)),
    m_pos(QPointF(9.8, 54))
{
    setVisible(false);
    connect(m_gps, SIGNAL(position(QPointF)), this, SLOT(position(QPointF)));
    connect(m_gps, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_gps, SIGNAL(disconnected()), this, SLOT(disconnected()));
    m_gps->connectGps();
}

void GpsLayer::triggerAction()
{
    if (isVisible()) {
        map()->centerOnGeoPos(m_pos.x(), m_pos.y());
    }
}

void GpsLayer::keyPressed(QKeyEvent *event)
{
    if (event->modifiers() == Qt::NoModifier &&
        event->key() == Qt::Key_G) {
        triggerAction();
    }
}

void GpsLayer::paint(QPainter *painter)
{
    QPoint pos = map()->geo2screen(m_pos.x(), m_pos.y());

    painter->drawPoint(pos);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(QBrush(QColor(0, 0, 255, 110)), 4));
    painter->drawEllipse(pos, 8, 8);
}

void GpsLayer::position(const QPointF &pos)
{
    m_pos = pos;
    if (isVisible()) {
        map()->update();
    }
}

void GpsLayer::connected()
{
    setVisible(true);
    map()->update();
}

void GpsLayer::disconnected()
{
    setVisible(false);
    map()->update();
}


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

#include "gpslayer.h"

#include "gpsclient.h"
#include "mapwidget.h"

#include <QtCore/QPoint>

GpsLayer::GpsLayer(MapWidget *map) :
    AbstractLayer(map),
    m_gps(new GpsClient(this)),
    m_pos(QPointF(0, 0)),
    m_alt(0),
    m_track(0),
    m_speed(0),
    m_fix(false)
{
    setVisible(false);
    connect(m_gps, SIGNAL(position(QPointF)), this, SLOT(position(QPointF)));
    connect(m_gps, SIGNAL(altitude(qreal)), this, SLOT(altitude(qreal)));
    connect(m_gps, SIGNAL(direction(qreal)), this, SLOT(direction(qreal)));
    connect(m_gps, SIGNAL(speed(qreal)), this, SLOT(speed(qreal)));
    connect(m_gps, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_gps, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(m_gps, SIGNAL(fixed(bool)), this, SLOT(fixed(bool)));
    m_gps->connectGps();
}

void GpsLayer::keyPressed(QKeyEvent *event)
{
    if (event->modifiers() == Qt::NoModifier &&
        event->key() == Qt::Key_G) {
        if (isVisible()) {
            map()->centerOnGeoPos(m_pos.x(), m_pos.y());
        }
    }
}

void GpsLayer::paint(QPainter *painter)
{
    QPoint pos = map()->geo2screen(m_pos.x(), m_pos.y());
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(QColor(0, 0, 255, 110), 4));
    painter->drawEllipse(pos, 8, 8);

    if (m_fix) {
        painter->setPen(QPen(QColor(0, 0, 255, 210), 2, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
        painter->translate(pos);
        painter->rotate(m_track);
        painter->drawLine(QPoint(4, 3), QPoint(0, -7));
        painter->drawLine(QPoint(-4, 3), QPoint(0, -7));
        painter->drawLine(QPoint(4, 3), QPoint(0, 0));
        painter->drawLine(QPoint(-4, 3), QPoint(0, 0));
    } else {
        painter->drawText(pos.x()-8, pos.y()-8, 16, 16, Qt::AlignCenter, "?");
    }
}

void GpsLayer::position(const QPointF &pos)
{
    m_pos = pos;
    if (isVisible()) {
        map()->update();
    }
}

void GpsLayer::altitude(qreal alt)
{
    m_alt = alt;
    if (isVisible()) {
        map()->update();
    }
}

void GpsLayer::direction(qreal track)
{
    m_track = track;
    if (isVisible()) {
        map()->update();
    }
}

void GpsLayer::speed(qreal speed)
{
    m_speed = speed;
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

void GpsLayer::fixed(bool fix)
{
    m_fix = fix;
    if (isVisible()) {
        map()->update();
    }
}


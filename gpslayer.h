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

#ifndef GPS_LAYER_H
#define GPS_LAYER_H

#include "abstractlayer.h"

#include <QtGui/QPainter>

class GpsClient;

class GpsLayer : public AbstractLayer
{
    Q_OBJECT
public:
    GpsLayer(MapWidget *map);

    virtual void triggerAction();
    virtual void keyPressed(QKeyEvent *event);

protected:
    virtual void paint(QPainter *painter);

private slots:
    void position(const QPointF &pos);
    void altitude(qreal alt);
    void direction(qreal track);
    void speed(qreal speed);
    void connected();
    void disconnected();

private:
    GpsClient *m_gps;
    QPointF m_pos;
    qreal m_alt, m_track, m_speed;

};

#endif // GPS_LAYER_H

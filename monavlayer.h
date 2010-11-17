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

#ifndef MONAV_LAYER_H
#define MONAV_LAYER_H

#include "abstractlayer.h"

#include <QtGui/QPainter>

#include "interfaces/irouter.h"
#include "interfaces/igpslookup.h"

class MonavLayer : public AbstractLayer
{
    Q_OBJECT
public:
    MonavLayer(MapWidget *map);

    virtual void zoom(int level);
    virtual void pan(const QPoint &move);
    virtual void keyPressed(QKeyEvent *event);

protected:
    virtual void paint(QPainter *painter);

private:
    void findRoute();

    IGPSLookup *m_gpsLookup;
    IRouter *m_router;
    bool m_loaded;
    QPointF m_routeStart, m_routeEnd;
    QPolygonF m_track;
    QList<QPoint> m_trackOnScreen;
    QPoint m_trackOffset;
    int m_zoomLevel, m_currentDirection;
    QStringList m_names, m_types;

};

#endif // MONAV_LAYER_H

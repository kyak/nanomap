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

#ifndef POI_LAYER_H
#define POI_LAYER_H

#include "abstractlayer.h"

#include <QtGui/QPainter>

class PoiLayer : public AbstractLayer
{
    Q_OBJECT
public:
    PoiLayer(MapWidget *map);

    virtual void load(const QString &filename);
    virtual void zoom(int level);
    virtual void pan(const QPoint &move);

protected:
    virtual void paint(QPainter *painter);

private:
    QList<QPointF> m_points;
    QList<QPoint> m_pointsOnScreen;
    QStringList m_icons;
    QPoint m_pointsOffset;
    QString m_iconPath;
    QHash<QString, QPixmap*> m_iconCache;

};

#endif // POI_LAYER_H

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

#include "projection.h"

#include <cmath>

qreal Projection::lon2rawx(qreal lon)
{
    return (lon + 180.0) / 360.0;
}
 
qreal Projection::lat2rawy(qreal lat)
{
    return (1.0 - log(tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0;
}

qreal Projection::lon2tilex(qreal lon, int z)
{
    return (lon + 180.0) / 360.0 * (1 << z);
}
 
qreal Projection::lat2tiley(qreal lat, int z)
{
    return (1.0 - log(tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * (1 << z);
}
 
qreal Projection::tilex2lon(qreal x, int z)
{
    return x / (1 << z) * 360.0 - 180;
}
 
qreal Projection::tiley2lat(qreal y, int z)
{
    qreal n = M_PI - 2.0 * M_PI * y / (1 << z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

QString Projection::geo2string(const QPointF &geo)
{
    QString lat = geo.y() > 0 ? QString("N %1").arg(geo.y()) : QString("S %1").arg(-geo.y());
    QString lon = geo.x() > 0 ? QString("E %1").arg(geo.x()) : QString("W %1").arg(-geo.x());
    return lat+" "+lon;
}


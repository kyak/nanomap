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

#ifndef PROJECTION_H
#define PROJECTION_H

#include <QtCore/QObject>

namespace Projection
{
    qreal lon2rawx(qreal lon);
    qreal lat2rawy(qreal lat);
    qreal lon2tilex(qreal lon, int z);
    qreal lat2tiley(qreal lat, int z);
    qreal tilex2lon(qreal x, int z);
    qreal tiley2lat(qreal y, int z);
};

#endif // PROJECTION_H

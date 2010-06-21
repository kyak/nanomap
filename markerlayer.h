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

#ifndef MARKER_LAYER_H
#define MARKER_LAYER_H

#include "abstractlayer.h"

#include <QtGui/QPainter>

class MarkerLayer : public AbstractLayer
{
    Q_OBJECT
public:
    MarkerLayer(MapWidget *map);
    ~MarkerLayer();

    virtual void load(const QString &filename);
    virtual void triggerAction();

protected:
    virtual void paint(QPainter *painter);

public slots:
    void removeMarker(int index);
    void renameMarker(int index, const QString &name);
    void centerOnMarker(int index);

signals:
    void markerAdded(const QString &name);

private:
    QList<QPointF> m_markerPos;
    QStringList m_markerName;
    QString m_filename;

};

#endif // MARKER_LAYER_H

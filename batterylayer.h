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

#ifndef BATTERY_LAYER_H
#define BATTERY_LAYER_H

#include "abstractlayer.h"

#include <QtCore/QTimer>
#include <QtGui/QPainter>

class BatteryLayer : public AbstractLayer
{
    Q_OBJECT
public:
    BatteryLayer(MapWidget *map);

protected:
    virtual void paint(QPainter *painter);

private slots:
    void repaint();
    void fileChanged(const QString &filename);

private:
    QTimer *m_updateTimer;
    int m_percent;
    bool m_isCharging;

};

#endif // BATTERY_LAYER_H

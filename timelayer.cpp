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

#include "timelayer.h"

#include "mapwidget.h"

#include <QtCore/QTime>

TimeLayer::TimeLayer(MapWidget *map) :
    AbstractLayer(map),
    m_updateTimer(new QTimer(this))
{
    m_updateTimer->setInterval(60 * 1000);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(repaint()));
    int time = (60 - QTime::currentTime().second()) * 1000;
    QTimer::singleShot(time, this, SLOT(repaint()));
    QTimer::singleShot(time, m_updateTimer, SLOT(start()));
}

void TimeLayer::paint(QPainter *painter)
{
    int w = map()->width();

    painter->setBrush(QBrush(QColor(255, 255, 255, 210)));
    painter->drawRoundedRect(w - 82, 1, 80, 16, 5, 5);
    painter->drawText(w - 77, 3, 70, 14, Qt::AlignCenter,
                      QTime::currentTime().toString("h:mm"));
}

void TimeLayer::repaint()
{
    if (isVisible()) {
        map()->update();
    }
}


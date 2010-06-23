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

#include "batterylayer.h"

#include "mapwidget.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTime>

BatteryLayer::BatteryLayer(MapWidget *map) :
    AbstractLayer(map),
    m_updateTimer(new QTimer(this)),
    m_percent(0),
    m_isCharging(false)
{
    m_updateTimer->setInterval(60 * 1000);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(repaint()));
    m_updateTimer->start();

    reload();
}

void BatteryLayer::keyPressed(QKeyEvent *event)
{
    if (event->modifiers() == Qt::NoModifier &&
        event->key() == Qt::Key_B) {
        toggleVisibility();
        reload();
    }
}

void BatteryLayer::paint(QPainter *painter)
{
    int w = map()->width();
    int h = map()->height();

    painter->setBrush(QBrush(QColor(255, 255, 255, 210)));
    painter->drawRoundedRect(w - 111, h - 17, 110, 16, 5, 5);
    painter->drawText(w - 101, h - 15, 90, 14, Qt::AlignCenter, QString("%1%").arg(m_percent));
    if (m_isCharging) {
        painter->setBrush(QBrush(QColor(0, 255, 0, 110)));
    } else {
        painter->setBrush(QBrush(QColor(0, 0, 255, 110)));
    }
    painter->drawRoundedRect(w - 111, h - 17, 10 + m_percent, 16, 5, 5);
}

void BatteryLayer::repaint()
{
    if (isVisible()) {
        if (reload()) {
            map()->update();
        }
    }
}

bool BatteryLayer::reload()
{
    int percent = m_percent;
    QFile capacity("/sys/class/power_supply/battery/capacity");
    if (capacity.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&capacity);
        QString l = in.readLine();
        percent = l.toInt();
    }

    bool charging = m_isCharging;
    QFile status("/sys/class/power_supply/battery/status");
    if (status.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&status);
        QString l = in.readLine().toLower();
        charging = (l == "charging");
    }

    if (charging != m_isCharging || percent != m_percent) {
        m_isCharging = charging;
        m_percent = percent;

        return true;
    } else {
        return false;
    }
}


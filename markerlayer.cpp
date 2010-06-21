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

#include "markerlayer.h"

#include "mapwidget.h"

#include <QtCore/QSettings>

MarkerLayer::MarkerLayer(MapWidget *map) :
    AbstractLayer(map),
    m_markerPos(),
    m_markerName(),
    m_filename()
{
}

MarkerLayer::~MarkerLayer()
{
    QSettings set(m_filename, QSettings::NativeFormat);

    set.beginGroup("marker");
    set.remove("");
    for (int i = 0; i < m_markerPos.count(); ++i) {
        set.setValue(m_markerName.at(i), m_markerPos.at(i));
    }
    set.endGroup();
}

void MarkerLayer::load(const QString &filename)
{
    m_filename = filename;

    QSettings set(filename, QSettings::NativeFormat);

    set.beginGroup("marker");
    QStringList m = set.childKeys();
    foreach (const QString &marker, m) {
        QPointF pos = set.value(marker).toPointF();
        m_markerPos << pos;
        m_markerName << marker;
        emit markerAdded(marker);
    }
    set.endGroup();
}

void MarkerLayer::triggerAction()
{
    int n = 0;
    if (!m_markerName.isEmpty()) {
        n = m_markerName.last().toInt();
    }
    QString newName = QString::number(n+1);

    m_markerPos << map()->geoPos();
    m_markerName << newName;
    emit markerAdded(newName);
}

void MarkerLayer::paint(QPainter *painter)
{
    int i = 0;
    painter->setBrush(QBrush(QColor(255, 237, 60)));
    QFontMetrics fm(painter->font());
    int h = fm.height() / 2;
    foreach (const QPointF &m, m_markerPos) {
        QPoint pos = map()->geo2screen(m.x(), m.y());
        int w = fm.width(m_markerName.at(i)) / 2;
        QRect rect(pos.x() - w - 2, pos.y() - h - 11, 2*w + 4, 2*h);
        QPolygon polygon;
        polygon << pos;
        polygon << pos + QPoint(-2, -5);
        polygon << rect.bottomLeft();
        polygon << rect.topLeft();
        polygon << rect.topRight();
        polygon << rect.bottomRight();
        polygon << pos + QPoint(2, -5);
        polygon << pos;
        painter->drawPolygon(polygon);
        painter->drawText(rect, Qt::AlignCenter, m_markerName.at(i));
        ++i;
    }
}

void MarkerLayer::removeMarker(int index)
{
    if (index >= 0 && m_markerPos.count() > index) {
        m_markerPos.removeAt(index);
        m_markerName.removeAt(index);
        map()->update();
    }
}

void MarkerLayer::renameMarker(int index, const QString &name)
{
    if (index >= 0 && m_markerName.count() > index) {
        m_markerName.replace(index, name);
    }
}

void MarkerLayer::centerOnMarker(int index)
{
    if (index >= 0 && m_markerPos.count() > index) {
        qreal lon = m_markerPos.at(index).x();
        qreal lat = m_markerPos.at(index).y();

        map()->centerOnGeoPos(lon, lat);
    }
}


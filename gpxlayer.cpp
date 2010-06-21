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

#include "gpxlayer.h"

#include "mapwidget.h"
#include "projection.h"

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtXml/QXmlStreamReader>

GpxLayer::GpxLayer(MapWidget *map) :
    AbstractLayer(map)
{
}

void GpxLayer::load(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&file);

        QPolygonF points;
        QList<float> elev;
        QList<int> time;

        QString tag, tag2;
        QString name;
        QPointF pos;
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == "trkpt") {
                    tag = "trkpt";
                    float lat = xml.attributes().value("lat").toString().toFloat();
                    float lon = xml.attributes().value("lon").toString().toFloat();

                    points << QPointF(Projection::lon2rawx(lon), Projection::lat2rawy(lat));
                } else if (xml.name() == "ele") {
                    tag2 = "ele";
                } else if (xml.name() == "time") {
                    tag2 = "time";
                } else if (xml.name() == "wpt") {
                    tag = "wpt";
                    float lat = xml.attributes().value("lat").toString().toFloat();
                    float lon = xml.attributes().value("lon").toString().toFloat();

                    pos = QPointF(lon, lat);
                } else if (xml.name() == "name") {
                    tag2 = "name";
                } else if (xml.name() == "trk" ||
                           xml.name() == "trkseg") {
                } else {
                    tag2.clear();
                }
            } else if (xml.isEndElement()) {
                if (xml.name() == "trkseg") {
                    if (!points.isEmpty()) {
                        m_track << points;
                        m_track << QPointF();
                    }
                    points.clear();
                    elev.clear();
                    time.clear();
                } else if (xml.name() == "wpt") {
                    //addMarker(pos, name);
                    name.clear();
                }
            } else if (xml.isCharacters() && !xml.isWhitespace()) {
                if (tag == "trkpt") {
                    if (tag2 == "ele") {
                        elev << xml.text().toString().toFloat();
                    } else if (tag2 == "time") {
                        QDateTime dt = QDateTime::fromString(xml.text().toString(), Qt::ISODate);
                        time << dt.toTime_t();
                    }
                } else if (tag == "wpt") {
                    if (tag2 == "name") {
                        name = xml.text().toString();
                    }
                }
            }
        }
        zoom(0);
    }
}

void GpxLayer::zoom(int level)
{
    if (m_track.count() > 1) {
        int scale = 1 << level;
        m_trackOnScreen.clear();
        m_trackOffset = map()->raw2screen(m_track.first().x(), m_track.first().y(), scale);
        m_trackOnScreen << QPoint(0, 0);
        for (int i = 1; i < m_track.count(); ++i) {
            QPointF p = m_track.at(i);
            if (!p.isNull()) {
                m_trackOnScreen << map()->raw2screen(p.x(), p.y(), scale) - m_trackOffset;
            } else {
                m_trackOnScreen << QPoint();
            }
        }
    }
}

void GpxLayer::pan(const QPoint &move)
{
    m_trackOffset += move;
}

void GpxLayer::paint(QPainter *painter)
{
    if (m_trackOnScreen.count() > 1) {
        QPoint p1, p2 = m_trackOnScreen.first();
        for (int i = 1; i < m_trackOnScreen.count(); ++i) {
            p1 = m_trackOnScreen.at(i);
            if (!p1.isNull()) {
                painter->drawLine(p1 + m_trackOffset, p2 + m_trackOffset);
                p2 = p1;
            }
        }
    }
}


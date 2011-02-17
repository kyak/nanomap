/*
 * Copyright 2010-2011  Niels Kummerfeldt <niels.kummerfeldt@tu-harburg.de>
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

#include "poilayer.h"

#include "mapwidget.h"
#include "projection.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtXml/QXmlStreamReader>

PoiLayer::PoiLayer(MapWidget *map) :
    AbstractLayer(map),
    m_points(),
    m_pointsOnScreen(),
    m_icons(),
    m_pointsOffset(0, 0),
    m_iconPath(),
    m_iconCache()
{
    QSettings set(QDir::homePath()+"/Maps/nanomap.conf", QSettings::NativeFormat);

    set.beginGroup("poi");
    m_iconPath = set.value("iconpath", "/usr/share/NanoMap/icons").toString();
    set.endGroup();
}

void PoiLayer::load(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&file);

        QStringList categories;
        categories << "highway";
        categories << "traffic_calming";
        categories << "barrier";
        categories << "waterway";
        categories << "railway";
        categories << "aeroway";
        categories << "aerialway";
        categories << "power";
        categories << "man_made";
        categories << "leisure";
        categories << "amenity";
        categories << "office";
        categories << "shop";
        categories << "craft";
        categories << "emergency";
        categories << "tourism";
        categories << "historic";
        categories << "landuse";
        categories << "military";
        categories << "natural";
        categories << "geological";
        categories << "sport";

        QHash<QString, QString> tags;
        QPointF pos;
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == "node") {
                    float lat = xml.attributes().value("lat").toString().toFloat();
                    float lon = xml.attributes().value("lon").toString().toFloat();

                    pos = QPointF(Projection::lon2rawx(lon), Projection::lat2rawy(lat));
                } else if (xml.name() == "tag") {
                    tags.insert(xml.attributes().value("k").toString(), 
                                xml.attributes().value("v").toString());
                }
            } else if (xml.isEndElement()) {
                if (xml.name() == "node") {
                    foreach (const QString &c, categories) {
                        QString t = tags.value(c, "");
                        if (!t.isEmpty()) {
                            QString icon = m_iconPath+"/"+t+".png";
                            if (QFile::exists(icon)) {
                                if (!m_iconCache.contains(t)) {
                                    m_iconCache.insert(t, new QPixmap(icon));
                                }
                                m_points << pos;
                                m_icons << t;
                                break;
                            }
                        }
                    }
                    tags.clear();
                }
            }
        }
        zoom(0);
    }
}

void PoiLayer::zoom(int level)
{
    if (m_points.isEmpty()) {
        return;
    }

    int scale = 1 << level;
    m_pointsOnScreen.clear();
    m_pointsOffset = map()->raw2screen(m_points.first().x(), m_points.first().y(), scale);
    m_pointsOnScreen << QPoint(0, 0);
    if (m_points.count() > 1) {
        for (int i = 1; i < m_points.count(); ++i) {
            QPointF p = m_points.at(i);
            m_pointsOnScreen << map()->raw2screen(p.x(), p.y(), scale) - m_pointsOffset;
        }
    }
}

void PoiLayer::pan(const QPoint &move)
{
    m_pointsOffset += move;
}

void PoiLayer::paint(QPainter *painter)
{
    QPoint p;
    for (int i = 0; i < m_pointsOnScreen.count(); ++i) {
        p = m_pointsOnScreen.at(i);
        painter->drawPixmap(p + m_pointsOffset, *m_iconCache.value(m_icons.at(i)));
    }
}


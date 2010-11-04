/*
 * Copyright 2008, 2010  Niels Kummerfeldt <niels.kummerfeldt@tu-harburg.de>
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

#include "gpsclient.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

GpsClient::GpsClient(QObject *parent) : QObject(parent),
    m_socket(new QTcpSocket(this))
{
    connect(m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_socket, SIGNAL(connected()), this, SLOT(conn()));
    connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void GpsClient::connectGps()
{
    m_socket->connectToHost("127.0.0.1", 2947);
}

void GpsClient::disconnectGps()
{
    QTextStream out(m_socket);
    out << "?WATCH={\"enable\":false}\n";
    m_socket->disconnectFromHost();
}

void GpsClient::readData()
{
    QTextStream in(m_socket);
    QString reply = in.readLine();
    if (reply.contains("TPV")) {
        reply.remove("{");
        reply.remove("}");
        qreal lat = 0;
        qreal lon = 0;
        int mode = 0;
        QStringList entries = reply.split(",");
        foreach (const QString &entry, entries) {
            if (entry.contains("lat")) {
                lat = entry.section(":", 1, 1).toDouble();
            } else if (entry.contains("lon")) {
                lon = entry.section(":", 1, 1).toDouble();
            } else if (entry.contains("alt")) {
                qreal alt = entry.section(":", 1, 1).toDouble();
                emit altitude(alt);
            } else if (entry.contains("track")) {
                qreal track = entry.section(":", 1, 1).toDouble();
                emit direction(track);
            } else if (entry.contains("speed")) {
                qreal currentSpeed = entry.section(":", 1, 1).toDouble();
                emit speed(currentSpeed);
            } else if (entry.contains("mode")) {
                // 0: no mode value yet seen
                // 1: no fix
                // 2: 2D
                // 3: 3D
                mode = entry.section(":", 1, 1).toInt();
            }
        }
        if (mode > 1) {
            emit fixed(true);
            emit position(QPointF(lon, lat));
        } else {
            emit fixed(false);
        }
    }
}

void GpsClient::conn()
{
    QTextStream out(m_socket);
    out << "?WATCH={\"enable\":true,\"json\":true}\n";
}


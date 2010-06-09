/*
 * Copyright 2008  Niels Kummerfeldt <niels.kummerfeldt@tu-harburg.de>
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
    m_socket->disconnectFromHost();
}

void GpsClient::query()
{
    QTextStream out(m_socket);
    out << "p\n";
}

void GpsClient::readData()
{
    QTextStream in(m_socket);
    QString reply = in.readLine();
    reply.remove(0, 7);
    if (reply != "?") {
        float lat = reply.section(' ', 0, 0).toFloat();
        float lon = reply.section(' ', 1, 1).toFloat();
        emit position(QPointF(lon, lat));
    }
    QTimer::singleShot(1000, this, SLOT(query()));
}

void GpsClient::conn()
{
    QTimer::singleShot(1000, this, SLOT(query()));
}


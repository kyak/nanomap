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

#ifndef GPSCLIENT_H
#define GPSCLIENT_H
  
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtNetwork/QTcpSocket>

class GpsClient : public QObject
{
    Q_OBJECT
public:
    GpsClient(QObject *parent = 0);

public slots:
    void connectGps();
    void disconnectGps();

signals:
    void position(const QPointF &pos);
    void altitude(qreal alt);
    void direction(qreal track);
    void speed(qreal speed);
    void connected();
    void disconnected();
    void fixed(bool fix);

private slots:
    void readData();
    void conn();

private:
    QTcpSocket *m_socket;

};

#endif // GPSCLIENT_H

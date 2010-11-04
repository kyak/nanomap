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

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>

#include "interfaces/iaddresslookup.h"

class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

signals:
    void back();
    void centerOn(qreal lon, qreal lat);

private slots:
    void cityChanged(const QString &city);
    void cityEntered();
    void citySelected(QListWidgetItem *item);
    void streetChanged(const QString &street);
    void streetSelected(QListWidgetItem *item);

private:
    IAddressLookup *m_addrLookup;
    QHash<int, GPSCoordinate> m_cityCoordinates;
    bool m_loaded;
    QLineEdit *m_city;
    QListWidget *m_cityList;
    QLineEdit *m_street;
    QListWidget *m_streetList;
 
};

#endif // SEARCHWIDGET_H

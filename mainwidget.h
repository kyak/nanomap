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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QListWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>

class MapWidget;
class MarkerList;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:
    void showList();
    void markerAdded(const QString &name);
    void showMap();
    void centerOnMarker(int row);
    void removeMarker(int row);
    void markerRenamed(int index, const QString &name);

private:
    QStackedWidget *m_stack;
    MapWidget *m_map;
    MarkerList *m_markerList;

};

#endif // MAINWIDGET_H

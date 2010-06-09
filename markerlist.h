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

#ifndef MARKERLIST_H
#define MARKERLIST_H

#include <QtGui/QListWidget>
#include <QtGui/QWidget>

class MarkerList : public QWidget
{
    Q_OBJECT

public:
    MarkerList(QWidget *parent = 0);
    ~MarkerList();

    void addMarker(const QString &name);

signals:
    void back();
    void centerOnMarker(int index);
    void removeMarker(int index);
    void markerRenamed(int index, const QString &name);

private slots:
    void center();
    void removeMarker();
    void beginRenameMarker();
    void endRenameMarker();

private:
    QListWidget *m_list;
    bool m_edit;

};

#endif // MAINWIDGET_H

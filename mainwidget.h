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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QListWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>

class DownloadWidget;
class FileSelector;
class MapWidget;
class MarkerList;
class SearchWidget;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
    void loadFile(const QString &fileName);
    void loadFile(const QString &fileName, const QString &title);

signals:
    void close();

private slots:
    void showList();
    void markerAdded(const QString &name);
    void showMap();
    void showMap(qreal lon, qreal lat);
    void downloadArea(int level, const QRectF &rect);
    void search();
    void showFileSelector();

private:
    QStackedWidget *m_stack;
    MapWidget *m_map;
    MarkerList *m_markerList;
    DownloadWidget *m_dlWidget;
    FileSelector *m_fileSelector;
    SearchWidget *m_search;

};

#endif // MAINWIDGET_H

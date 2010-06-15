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

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QPoint>
#include <QtNetwork/QNetworkAccessManager>

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    MapWidget(QWidget *parent = 0);
    ~MapWidget();

    void removeMarker(int index);
    void renameMarker(int index, const QString &name);

public slots:
    void centerOnMarker(int index);

signals:
    void markerAdded(const QString &name);
    void showMarkerList();
    void downloadArea(int level, const QRectF &rect);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private slots:
    void replyFinished(QNetworkReply *reply);
    void loadConfig();

private:
    void updatePos();
    void reloadPixmaps();
    void updateTrack();
    void addMarker(const QPointF &pos, const QString &name);
    QString filename(int x, int y);
    QPixmap *loadPixmap(int x, int y);
    void loadMapFile(const QString &filename);
    void loadGpx(const QString &filename);
    void saveConfig();
    void downloadTile(int x, int y, int level);
    void changeZoomLevel(int diff);
    void centerOnGeoPos(qreal lon, qreal lat);
    QRectF geoRect() const;
    QPointF geoPos() const;
    QPoint geo2screen(qreal lon, qreal lat) const;
    qreal lon2tilex(qreal lon, int z) const;
    qreal lat2tiley(qreal lat, int z) const;
    qreal tilex2lon(qreal x, int z) const;
    qreal tiley2lat(qreal y, int z) const;

    bool m_usage, m_infos, m_zoomable;
    QString m_baseName;
    int m_xPadding, m_yPadding;
    QPoint m_pos, m_startPos;
    bool m_isMoving;
    int m_pixWidth, m_pixHeight;
    QPixmap *m_pix[100][100];
    int m_cols, m_rows;
    int m_indexX, m_indexY;
    int m_minIndexX, m_minIndexY;
    int m_maxIndexX, m_maxIndexY;
    QList<int> m_minIndexXList, m_minIndexYList;
    QList<int> m_maxIndexXList, m_maxIndexYList;
    int m_level;
    QStringList m_zoomLevel;
    QNetworkAccessManager *m_manager;
    bool m_networkMode;
    QString m_copyright;
    QList<QPointF> m_markerPos;
    QStringList m_markerName;
    bool m_drawMarker;
    QPolygonF m_track;
    QList<QPoint> m_trackOnScreen;
    QPoint m_trackOffset;

};

#endif // MAPWIDGET_H

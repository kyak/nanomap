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

#include "mapwidget.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#ifdef Q_WS_QWS
    #include <QtGui/QWSServer>
#endif
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent),
    m_usage(false),
    m_infos(true),
    m_zoomable(false),
    m_baseName(),
    m_xPadding(0),
    m_yPadding(0),
    m_pos(0, 0),
    m_startPos(0, 0),
    m_isMoving(false),
    m_pixWidth(256),
    m_pixHeight(256),
    m_pix(),
    m_cols(2),
    m_rows(2),
    m_indexX(0),
    m_indexY(0),
    m_minIndexX(0),
    m_minIndexY(0),
    m_maxIndexX(0),
    m_maxIndexY(0),
    m_minIndexXList(),
    m_minIndexYList(),
    m_maxIndexXList(),
    m_maxIndexYList(),
    m_level(0),
    m_zoomLevel(),
    m_manager(new QNetworkAccessManager(this)),
    m_networkMode(false),
    m_copyright(),
    m_markerPos(),
    m_markerName(),
    m_drawMarker(true)
{
    for (int x = 0; x < 100; ++x) {
        for (int y = 0; y < 100; ++y) {
            m_pix[x][y] = 0;
        }
    }
    if (QApplication::arguments().count() > 1) {
        loadMapFile(QApplication::arguments().at(1));

        m_zoomable = m_zoomLevel.count() > 1 &&
                     m_zoomLevel.count() == m_minIndexXList.count() &&
                     m_zoomLevel.count() == m_maxIndexXList.count() &&
                     m_zoomLevel.count() == m_minIndexYList.count() &&
                     m_zoomLevel.count() == m_maxIndexYList.count();

        m_indexX = (m_minIndexX + m_maxIndexX) / 2;
        m_indexY = (m_minIndexY + m_maxIndexY) / 2;

        if (QApplication::arguments().count() > 3) {
            bool ok;
            int x = QApplication::arguments().at(2).toInt(&ok);
            if (ok) {
                m_indexX = x;
            }
            int y = QApplication::arguments().at(3).toInt(&ok);
            if (ok) {
                m_indexY = y;
            }
        }

        m_cols = ceil(320.0 / (qreal) m_pixWidth) + 1;
        m_rows = ceil(240.0 / (qreal) m_pixHeight) + 1;

        for (int x = 0; x < m_cols; ++x) {
            for (int y = 0; y < m_rows; ++y) {
                m_pix[x][y] = loadPixmap(m_indexX+x, m_indexY+y);
            }
        }
    } else {
        m_networkMode = true;
        m_zoomable = true;
        for (int i = 0; i < 19; ++i) {
            m_zoomLevel << QString::number(i);
            m_minIndexXList << 0;
            m_maxIndexXList << pow(2, i) - 1;
            m_minIndexYList << 0;
            m_maxIndexYList << pow(2, i) - 1;
        }
        m_baseName = QDir::homePath()+"/Maps/OSM/%z/%x/%y.png";
        QTimer::singleShot(100, this, SLOT(loadConfig()));
    }

    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

#ifdef Q_WS_QWS
    QWSServer::setCursorVisible(false);
#endif
    setFocusPolicy(Qt::StrongFocus);
    resize(320, 240);
}

MapWidget::~MapWidget()
{
    saveConfig();
}

void MapWidget::removeMarker(int index)
{
    if (index >= 0 && m_markerPos.count() > index) {
        m_markerPos.removeAt(index);
        m_markerName.removeAt(index);
        update();
    }
}

void MapWidget::renameMarker(int index, const QString &name)
{
    if (index >= 0 && m_markerName.count() > index) {
        m_markerName.replace(index, name);
    }
}

void MapWidget::centerOnMarker(int index)
{
    if (index >= 0 && m_markerPos.count() > index) {
        qreal lon = m_markerPos.at(index).x();
        qreal lat = m_markerPos.at(index).y();

        centerOnGeoPos(lon, lat);
    }
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    qreal width = event->size().width();
    qreal height = event->size().height();

    for (int x = 0; x < m_cols; ++x) {
        for (int y = 0; y < m_rows; ++y) {
            delete m_pix[x][y];
            m_pix[x][y] = 0;
        }
    }

    m_cols = ceil(width / (qreal) m_pixWidth) + 1;
    m_rows = ceil(height / (qreal) m_pixHeight) + 1;

    for (int x = 0; x < m_cols; ++x) {
        for (int y = 0; y < m_rows; ++y) {
            m_pix[x][y] = loadPixmap(m_indexX+x, m_indexY+y);
        }
    }
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isMoving) {
        m_pos = event->pos() - m_startPos;
        updatePos();
    }
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    if (QRect(9, 14, 13, 13).contains(event->pos())) {
        changeZoomLevel(1);
        reloadPixmaps();
        updatePos();
        m_isMoving = false;
    } else if (QRect(9, 214, 13, 13).contains(event->pos())) {
        changeZoomLevel(-1);
        reloadPixmaps();
        updatePos();
        m_isMoving = false;
    } else if (!QRect(5, 10, 20, 220).contains(event->pos())) {
        m_startPos = event->pos() - m_pos;
        m_isMoving = true;
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    m_isMoving = false;
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() < 0) {
        changeZoomLevel(-1);
        reloadPixmaps();
    } else {
        changeZoomLevel(1);
        reloadPixmaps();
    }
    updatePos();
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    int width = 10;
    if (event->modifiers() & Qt::AltModifier) {
        width = 100;
    }
    switch (event->key()) {
        case Qt::Key_Tab:
        {
            emit showMarkerList();
            break;
        }
        case Qt::Key_M:
        {
            if (event->modifiers() & Qt::AltModifier) {
                m_drawMarker = !m_drawMarker;
            } else if (event->modifiers() == Qt::NoModifier) {
                int n = 0;
                if (!m_markerName.isEmpty()) {
                    n = m_markerName.last().toInt();
                }
                QString newName = QString::number(n+1);
                m_markerPos << geoPos();
                m_markerName << newName;
                emit markerAdded(newName);
            }
            break;
        }
        case Qt::Key_K:
        case Qt::Key_Up:
        {
            m_pos += QPoint(0, width);
            break;
        }
        case Qt::Key_J:
        case Qt::Key_Down:
        {
            m_pos += QPoint(0, -width);
            break;
        }
        case Qt::Key_H:
        case Qt::Key_Left:
        {
            m_pos += QPoint(width, 0);
            break;
        }
        case Qt::Key_L:
        case Qt::Key_Right:
        {
            m_pos += QPoint(-width, 0);
            break;
        }
        case Qt::Key_C:
        {
            m_indexX = (m_minIndexX + m_maxIndexX) / 2;
            m_indexY = (m_minIndexY + m_maxIndexY) / 2;
            m_pos.setX(m_pixWidth/2);
            m_pos.setY(m_pixHeight/2);
            reloadPixmaps();
            break;
        }
        case Qt::Key_O:
        {
            changeZoomLevel(-1);
            reloadPixmaps();
            break;
        }
        case Qt::Key_I:
        {
            changeZoomLevel(1);
            reloadPixmaps();
            break;
        }
        case Qt::Key_D:
        {
            emit downloadArea(m_level, geoRect());
            break;
        }
        case Qt::Key_U:
        {
            m_infos = !m_infos;
            break;
        }
        case Qt::Key_Q:
        {
            qApp->quit();
            break;
        }
        case Qt::Key_Question:
        {
            m_usage = !m_usage;
            break;
        }
    }
    updatePos();
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    bool empty = true;
    for (int x = 0; x < m_cols; ++x) {
        for (int y = 0; y < m_rows; ++y) {
            QPixmap *pix = m_pix[x][y];
            if (pix) {
                empty = false;
                QRect rect(m_pos+QPoint(m_pixWidth*x, m_pixHeight*y), pix->size());
                painter.drawPixmap(rect, *pix);
            }
        }
    }
    if (empty) {
        painter.drawText(0, 0, width(), height(), Qt::AlignCenter, "No Map Loaded");
    }
    if (m_drawMarker) {
        int i = 0;
        painter.setBrush(QBrush(QColor(255, 237, 60)));
        QFontMetrics fm(painter.font());
        int h = fm.height() / 2;
        foreach (const QPointF &m, m_markerPos) {
            QPoint pos = geo2screen(m.x(), m.y());
            int w = fm.width(m_markerName.at(i)) / 2;
            QRect rect(pos.x() - w, pos.y() - h, 2*w, 2*h);
            painter.drawRect(rect.adjusted(-2, 0, 2, 0));
            painter.drawText(rect, Qt::AlignCenter, m_markerName.at(i));
            ++i;
        }
    }
    if (m_infos) {
        painter.setBrush(QBrush(QColor(255, 255, 255, 210)));
        if (m_networkMode) {
            painter.drawRoundedRect(30, height() - 20, width() - 35, 19, 10, 10);
            QPointF geo = geoPos();
            QString lon = geo.x() > 0 ? QString("E %1").arg(geo.x()) : QString("W %1").arg(-geo.x());
            QString lat = geo.y() > 0 ? QString("N %1").arg(geo.y()) : QString("S %1").arg(-geo.y());
            painter.drawText(35, height() - 18, width() - 45, 15, Qt::AlignCenter, lat+" "+lon);
        }
        if (m_zoomable) {
            painter.drawRoundedRect(5, 10, 20, 220, 10, 10);
            painter.setBrush(QBrush(QColor(0, 0, 255, 210)));
            int step = 180 / (m_zoomLevel.count() - 1);
            for (int i = 0; i < m_zoomLevel.count(); ++i) {
                painter.drawLine(5, 211-(i * step), 25, 211-(i * step));
            }
            painter.drawRect(9, 208-(m_level * step), 12, 5);
            painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap));
            painter.drawLine(9, 20, 22, 20);
            painter.drawLine(15, 14, 15, 27);
            painter.drawLine(9, 220, 22, 220);
            painter.setPen(QPen());
        }
        int midX = width() / 2;
        int midY = height() / 2;
        painter.drawLine(midX - 5, midY, midX + 5, midY);
        painter.drawLine(midX, midY - 5, midX, midY + 5);
    }
    if (m_usage) {
        painter.setBrush(QBrush(QColor(255, 255, 255, 210)));
        painter.drawRoundedRect(20, 5, 280, 215, 10, 10);

        QStringList usage;
        usage << "q: Quit application";
        usage << "?: Show/hide this message";
        usage << "Arrows: Move map by 10 pixel";
        usage << "Alt+Arrows: Move map by 100 pixel";
        usage << "c: Move to the center of the map";
        if (m_zoomable) {
            usage << "i: Zoom in";
            usage << "o: Zoom out";
        }
        usage << "u: Show/hide user interface";
        usage << "m: Add a marker";
        usage << "Alt+m: Show/hide all marker";
        usage << "tab: Show/hide marker list";
        if (m_networkMode) {
            painter.drawText(30, 200, 260, 20, Qt::AlignCenter, "Map data: (C) OpenStreetMap.org");
        } else if (!m_copyright.isEmpty()) {
            painter.drawText(30, 200, 260, 20, Qt::AlignCenter, "Map data: (C) "+m_copyright);
        }
        painter.drawText(30, 10, 260, 20, Qt::AlignCenter, "NanoMap - Usage");
        painter.drawLine(70, 27, 250, 27);
        painter.drawText(30, 30, 260, 200, Qt::AlignLeft, usage.join("\n"));
    }

    painter.end();
}

void MapWidget::replyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString path = reply->url().path();
        int level = path.section('/', 1, 1).toInt();
        int x = path.section('/', 2, 2).toInt();
        QString name = path.section('/', 3, 3);
        int y = name.section('.', 0, 0).toInt();

        QDir base(QDir::homePath()+"/Maps/OSM");
        base.mkpath(QString("%1/%2").arg(level).arg(x));

        QByteArray data = reply->readAll();
        if (!data.isEmpty()) {
            QFile file(QDir::homePath()+"/Maps/OSM"+path);
            if (file.open(QFile::WriteOnly)) {
                file.write(data);
                if (level == m_level) {
                    if (m_pix[x-m_indexX][y-m_indexY]) {
                        m_pix[x-m_indexX][y-m_indexY]->loadFromData(data);
                    } else {
                        m_pix[x-m_indexX][y-m_indexY] = new QPixmap(file.fileName());
                    }
                    update();
                }
            }
        }
    }
    reply->deleteLater();
}

void MapWidget::updatePos()
{
    if (m_pos.x() < -m_pixWidth) {
        if (m_indexX < m_maxIndexX) {
            m_pos.setX(m_pos.x() + m_pixWidth);
            m_startPos.setX(m_startPos.x() - m_pixWidth);
            ++m_indexX;

            for (int y = 0; y < m_rows; ++y) {
                for (int x = 0; x < m_cols; ++x) {
                    if (x == 0) {
                        delete m_pix[x][y];
                        m_pix[x][y] = 0;
                    }
                    if (x < m_cols-1) {
                        m_pix[x][y] = m_pix[x+1][y];
                    } else {
                        m_pix[x][y] = loadPixmap(m_indexX+x, m_indexY+y);
                    }
                }
            }
        }
    } else if (m_pos.x() > 0) {
        if (m_indexX > m_minIndexX) {
            m_pos.setX(m_pos.x() - m_pixWidth);
            m_startPos.setX(m_startPos.x() + m_pixWidth);
            --m_indexX;

            for (int y = 0; y < m_rows; ++y) {
                for (int x = m_cols-1; x >= 0; --x) {
                    if (x == m_cols-1) {
                        delete m_pix[x][y];
                        m_pix[x][y] = 0;
                    }
                    if (x > 0) {
                        m_pix[x][y] = m_pix[x-1][y];
                    } else {
                        m_pix[x][y] = loadPixmap(m_indexX, m_indexY+y);
                    }
                }
            }
        }
    }
    if (m_pos.y() < -m_pixHeight) {
        if (m_indexY < m_maxIndexY) {
            m_pos.setY(m_pos.y() + m_pixHeight);
            m_startPos.setY(m_startPos.y() - m_pixHeight);
            ++m_indexY;

            for (int x = 0; x < m_cols; ++x) {
                for (int y = 0; y < m_rows; ++y) {
                    if (y == 0) {
                        delete m_pix[x][y];
                        m_pix[x][y] = 0;
                    }
                    if (y < m_rows-1) {
                        m_pix[x][y] = m_pix[x][y+1];
                    } else {
                        m_pix[x][y] = loadPixmap(m_indexX+x, m_indexY+y);
                    }
                }
            }
        }
    } else if (m_pos.y() > 0) {
        if (m_indexY > m_minIndexY) {
            m_pos.setY(m_pos.y() - m_pixHeight);
            m_startPos.setY(m_startPos.y() + m_pixHeight);
            --m_indexY;

            for (int x = 0; x < m_cols; ++x) {
                for (int y = m_rows-1; y >= 0; --y) {
                    if (y == m_rows-1) {
                        delete m_pix[x][y];
                        m_pix[x][y] = 0;
                    }
                    if (y > 0) {
                        m_pix[x][y] = m_pix[x][y-1];
                    } else {
                        m_pix[x][y] = loadPixmap(m_indexX+x, m_indexY);
                    }
                }
            }
        }
    }
    update();
}

void MapWidget::reloadPixmaps()
{
    for (int x = 0; x < m_cols; ++x) {
        for (int y = 0; y < m_rows; ++y) {
            delete m_pix[x][y];
            m_pix[x][y] = 0;
            m_pix[x][y] = loadPixmap(m_indexX+x, m_indexY+y);
        }
    }
}

QString MapWidget::filename(int x, int y)
{
    QString result;
    if (x >= m_minIndexX && x <= m_maxIndexX &&
        y >= m_minIndexY && y <= m_maxIndexY) {
        QString level = QString::number(m_level);
        QString sx = QString::number(x);
        sx.prepend(QString(m_xPadding-sx.length(), '0'));
        QString sy = QString::number(y);
        sy.prepend(QString(m_yPadding-sy.length(), '0'));
        result = m_baseName;
        result.replace("%z", level).replace("%x", sx).replace("%y", sy);
    }
    return result;
}

QPixmap* MapWidget::loadPixmap(int x, int y)
{
    QPixmap *pix = 0;

    if (x >= m_minIndexX && x <= m_maxIndexX &&
        y >= m_minIndexY && y <= m_maxIndexY) {
        pix = new QPixmap(filename(x, y));
        if (m_networkMode && pix->isNull()) {
            downloadTile(x, y, m_level);
        }
    }

    return pix;
}

void MapWidget::loadMapFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QFileInfo info(filename);
        m_baseName = info.absolutePath()+QDir::separator();

        QTextStream in(&file);
        QString line;
        while (!in.atEnd()) {
            line = in.readLine().trimmed();
            if (!line.startsWith('#') && !line.isEmpty()) {
                if (line.startsWith("BaseName=")) {
                    m_baseName.append(line.section('=', 1));
                } else if (line.startsWith("ZeroPadding=")) {
                    QString tmp = line.section('=', 1);
                    m_xPadding = tmp.section(',', 0, 0).toInt();
                    m_yPadding = tmp.section(',', 1, 1).toInt();
                } else if (line.startsWith("TileWidth=")) {
                    QString tmp = line.section('=', 1);
                    m_pixWidth = tmp.toInt();
                } else if (line.startsWith("TileHeight=")) {
                    QString tmp = line.section('=', 1);
                    m_pixHeight = tmp.toInt();
                } else if (line.startsWith("ZoomLevel=")) {
                    QString tmp = line.section('=', 1);
                    m_zoomLevel << tmp.split(',');
                } else if (line.startsWith("MinX=")) {
                    QString tmp = line.section('=', 1);
                    QStringList list = tmp.split(',');
                    foreach (const QString &s, list) {
                        m_minIndexXList << s.toInt();
                    }
                    m_minIndexX = m_minIndexXList.first();
                } else if (line.startsWith("MaxX=")) {
                    QString tmp = line.section('=', 1);
                    QStringList list = tmp.split(',');
                    foreach (const QString &s, list) {
                        m_maxIndexXList << s.toInt();
                    }
                    m_maxIndexX = m_maxIndexXList.first();
                } else if (line.startsWith("MinY=")) {
                    QString tmp = line.section('=', 1);
                    QStringList list = tmp.split(',');
                    foreach (const QString &s, list) {
                        m_minIndexYList << s.toInt();
                    }
                    m_minIndexY = m_minIndexYList.first();
                } else if (line.startsWith("MaxY=")) {
                    QString tmp = line.section('=', 1);
                    QStringList list = tmp.split(',');
                    foreach (const QString &s, list) {
                        m_maxIndexYList << s.toInt();
                    }
                    m_maxIndexY = m_maxIndexYList.first();
                } else if (line.startsWith("CopyRight=")) {
                    m_copyright = line.section('=', 1);
                }
            }
        }
    }
}

void MapWidget::loadConfig()
{
    QSettings set(QDir::homePath()+"/.nanomap.conf", QSettings::NativeFormat);
    set.beginGroup("map");
    qreal lon = set.value("lon", 0).toReal();
    qreal lat = set.value("lat", 0).toReal();
    int level = set.value("level", 0).toInt();
    m_usage = set.value("usage", true).toBool();
    changeZoomLevel(level - m_level);
    centerOnGeoPos(lon, lat);
    set.endGroup();
    set.beginGroup("marker");
    QStringList m = set.childKeys();
    foreach (const QString &marker, m) {
        QPointF pos = set.value(marker).toPointF();
        m_markerPos << pos;
        m_markerName << marker;
        emit markerAdded(marker);
    }
    set.endGroup();
}

void MapWidget::saveConfig()
{
    QSettings set(QDir::homePath()+"/.nanomap.conf", QSettings::NativeFormat);
    set.beginGroup("map");
    QPointF pos = geoPos();
    set.setValue("lon", pos.x());
    set.setValue("lat", pos.y());
    set.setValue("level", m_level);
    set.setValue("usage", m_usage);
    set.endGroup();
    set.beginGroup("marker");
    set.remove("");
    for (int i = 0; i < m_markerPos.count(); ++i) {
        set.setValue(m_markerName.at(i), m_markerPos.at(i));
    }
    set.endGroup();
}

void MapWidget::downloadTile(int x, int y, int level)
{
    QUrl url(QString("http://tile.openstreetmap.org/%1/%2/%3.png").arg(level).arg(x).arg(y));
    m_manager->get(QNetworkRequest(url));
}

void MapWidget::changeZoomLevel(int diff)
{
    qreal w = width() / 2;
    qreal h = height() / 2;
    qreal px = (qreal) ((m_indexX - m_minIndexX)*m_pixWidth + w - m_pos.x()) /
               (qreal) ((1 + m_maxIndexX - m_minIndexX) * m_pixWidth);
    qreal py = (qreal) ((m_indexY - m_minIndexY)*m_pixHeight + h - m_pos.y()) /
               (qreal) ((1 + m_maxIndexY - m_minIndexY) * m_pixHeight);

    m_level += diff;

    if (m_level < 0 || m_level >= m_zoomLevel.count()) {
        m_level -= diff;
        return;
    }

    m_minIndexX = m_minIndexXList.at(m_level);
    m_maxIndexX = m_maxIndexXList.at(m_level);
    m_minIndexY = m_minIndexYList.at(m_level);
    m_maxIndexY = m_maxIndexYList.at(m_level);

    qreal x = (px * (1 + m_maxIndexX - m_minIndexX)) + m_minIndexX;
    qreal y = (py * (1 + m_maxIndexY - m_minIndexY)) + m_minIndexY;

    m_indexX = (int) floor(x);
    m_indexY = (int) floor(y);
    m_pos.setX(((m_indexX-x) * m_pixWidth) + w);
    m_pos.setY(((m_indexY-y) * m_pixHeight) + h);
}

void MapWidget::centerOnGeoPos(qreal lon, qreal lat)
{
    qreal w = width() / 2.0;
    qreal h = height() / 2.0;

    qreal x = lon2tilex(lon, m_level);
    qreal y = lat2tiley(lat, m_level);

    m_indexX = (int) floor(x);
    m_indexY = (int) floor(y);
    m_pos.setX(((m_indexX-x) * m_pixWidth) + w);
    m_pos.setY(((m_indexY-y) * m_pixHeight) + h);

    reloadPixmaps();
    updatePos();
}

QRectF MapWidget::geoRect() const
{
    qreal partX = (-m_pos.x()) / 256.0;
    qreal partY = (height() - m_pos.y()) / 256.0;
    qreal minLon = tilex2lon(m_indexX + partX, m_level);
    qreal minLat = tiley2lat(m_indexY + partY, m_level);

    partX = (width() - m_pos.x()) / 256.0;
    partY = (-m_pos.y()) / 256.0;
    qreal maxLon = tilex2lon(m_indexX + partX, m_level);
    qreal maxLat = tiley2lat(m_indexY + partY, m_level);

    return QRectF(QPointF(minLon, minLat), QPointF(maxLon, maxLat));
}

QPointF MapWidget::geoPos() const
{
    qreal w = width() / 2.0;
    qreal h = height() / 2.0;
    qreal partX = (w - m_pos.x()) / 256.0;
    qreal partY = (h - m_pos.y()) / 256.0;
    qreal lon = tilex2lon(m_indexX + partX, m_level);
    qreal lat = tiley2lat(m_indexY + partY, m_level);

    return QPointF(lon, lat);
}

QPoint MapWidget::geo2screen(qreal lon, qreal lat) const
{
    qreal tx = lon2tilex(lon, m_level);
    qreal ty = lat2tiley(lat, m_level);

    int x = (tx * m_pixWidth) - ((m_indexX * m_pixWidth) - m_pos.x());
    int y = (ty * m_pixHeight) - ((m_indexY * m_pixHeight) - m_pos.y());

    return QPoint(x, y);
}

qreal MapWidget::lon2tilex(qreal lon, int z) const
{
    return (lon + 180.0) / 360.0 * pow(2.0, z);
}
 
qreal MapWidget::lat2tiley(qreal lat, int z) const
{
    return (1.0 - log(tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z);
}
 
qreal MapWidget::tilex2lon(qreal x, int z) const
{
    return x / pow(2.0, z) * 360.0 - 180;
}
 
qreal MapWidget::tiley2lat(qreal y, int z) const
{
    qreal n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}


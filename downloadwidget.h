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

#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <QtNetwork/QNetworkAccessManager>

class DownloadWidget : public QWidget
{
    Q_OBJECT
public:
    DownloadWidget(QWidget *parent = 0);
    ~DownloadWidget();

    void setStartLevel(int level);
    void setDownloadRect(const QRectF &rect);

signals:
    void back();

private slots:
    void startDownload();
    void replyFinished(QNetworkReply *reply);

private:
    QString lon2string(qreal lon);
    QString lat2string(qreal lat);
    int lon2tilex(qreal lon, int z);
    int lat2tiley(qreal lat, int z);

    QNetworkAccessManager *m_manager;
    int m_startLevel;
    QRectF m_dlRect;
    QStringList m_dlList;
    QString m_prefix;
    QLabel *m_up, *m_left, *m_right, *m_bottom;
    QSpinBox *m_levelSpinBox;
    QProgressBar *m_dlProgress;
    QLineEdit *m_prefixInput;
    QCheckBox *m_skipExisting;

};

#endif // DOWNLOADWIDGET_H

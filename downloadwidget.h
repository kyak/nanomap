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

#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

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
    void loadFile(const QString &filename, const QString &title);

private slots:
    void startDownload();
    void replyFinished(QNetworkReply *reply);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void saveDownload();
    void updateSourceList();

private:
    enum DownloadMode {
        Tiles,
        POIs,
        Packages,
        SourceList
    };

    void startDownloadTiles();
    void replyFinishedTiles(QNetworkReply *reply);
    void startDownloadPOIs();
    void replyFinishedPOIs(QNetworkReply *reply);
    void startDownloadPackages();
    void replyFinishedPackages(QNetworkReply *reply);
    QString lon2string(qreal lon);
    QString lat2string(qreal lat);
    int lon2tilex(qreal lon, int z);
    int lat2tiley(qreal lat, int z);

    QNetworkAccessManager *m_manager;
    DownloadMode m_downloadMode;
    QTabWidget *m_tabWidget;
    QProgressBar *m_dlProgress;
    QPushButton *m_startButton, *m_backButton;
    int m_startLevel;
    QRectF m_dlRect;
    QStringList m_dlList;
    QNetworkReply *m_currentDownload;
    QString m_prefix;
    QLabel *m_up, *m_left, *m_right, *m_bottom;
    QSpinBox *m_levelSpinBox;
    QLineEdit *m_prefixInput;
    QCheckBox *m_skipExisting;
    QComboBox *m_poiType;
    QCheckBox *m_makePOILayer;
    QLineEdit *m_destFilename;
    QListWidget *m_packageList;
    QLineEdit *m_destDir;

};

#endif // DOWNLOADWIDGET_H

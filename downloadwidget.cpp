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

#include "downloadwidget.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtGui/QLayout>
#include <QtNetwork/QNetworkRequest>

DownloadWidget::DownloadWidget(QWidget *parent)
    : QWidget(parent),
    m_manager(new QNetworkAccessManager(this)),
    m_downloadMode(Tiles),
    m_tabWidget(new QTabWidget(this)),
    m_dlProgress(new QProgressBar(this)),
    m_startButton(new QPushButton("&Start download", this)),
    m_backButton(new QPushButton("&Back", this)),
    m_startLevel(0),
    m_dlRect(),
    m_dlList(),
    m_currentDownload(0),
    m_prefix("OSM"),
    m_up(new QLabel("N 0", this)),
    m_left(new QLabel("E 0", this)),
    m_right(new QLabel("E 0", this)),
    m_bottom(new QLabel("N 0", this)),
    m_levelSpinBox(new QSpinBox(this)),
    m_prefixInput(new QLineEdit(this)),
    m_skipExisting(new QCheckBox("S&kip already downloaded tiles", this)),
    m_poiType(new QComboBox(this)),
    m_makePOILayer(new QCheckBox("&Load file after download", this)),
    m_destFilename(new QLineEdit(QDir::homePath()+"/pois.osm", this)),
    m_packageList(new QListWidget(this)),
    m_destDir(new QLineEdit(QDir::homePath(), this))
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setRowStretch(0, 1);

    layout->addWidget(m_tabWidget, 0, 0, 1, 2);

    m_dlProgress->hide();
    layout->addWidget(m_dlProgress, 1, 0, 1, 2);

    m_startButton->setIcon(QIcon(":ok.png"));
    connect(m_startButton, SIGNAL(clicked()), this, SLOT(startDownload()));
    layout->addWidget(m_startButton, 2, 0);

    m_backButton->setIcon(QIcon(":cancel.png"));
    connect(m_backButton, SIGNAL(clicked()), this, SIGNAL(back()));
    layout->addWidget(m_backButton, 2, 1);

    QWidget *widget = new QWidget(this);
    layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setRowStretch(1, 1);

    m_up->setAlignment(Qt::AlignHCenter);
    m_left->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_right->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_bottom->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_up, 0, 0, 1, 4);
    layout->addWidget(m_left, 1, 0);
    layout->addWidget(m_right, 1, 3);
    layout->addWidget(m_bottom, 2, 0, 1, 4);

    QLabel *label = new QLabel(this);
    label->setFrameShape(QLabel::Box);
    layout->addWidget(label, 1, 1, 1, 2);

    label = new QLabel("Download up to level:", this);
    layout->addWidget(label, 3, 0, 1, 2);

    m_levelSpinBox->setRange(0, 18);
    layout->addWidget(m_levelSpinBox, 3, 2, 1, 2);

    label = new QLabel("Download into directory:", this);
    layout->addWidget(label, 4, 0, 1, 2);

    m_prefixInput->setText(m_prefix);
    layout->addWidget(m_prefixInput, 4, 2, 1, 2);

    m_skipExisting->setChecked(false);
    layout->addWidget(m_skipExisting, 5, 0, 1, 0);

    m_tabWidget->addTab(widget, "&Tiles");

    widget = new QWidget(this);
    layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setRowStretch(1, 1);

    label = new QLabel("Type:", this);
    layout->addWidget(label, 0, 0);

    QSettings set(QDir::homePath()+"/Maps/nanomap.conf", QSettings::NativeFormat);
    set.beginGroup("poi");
    QString iconPath = set.value("iconpath", "/usr/share/NanoMap/icons").toString();
    set.endGroup();
    QDir iconDir(iconPath);
    QStringList icons = iconDir.entryList(QStringList() << "*.png");
    foreach (const QString &icon, icons) {
        QString name = icon;
        name.remove(".png");
        m_poiType->addItem(QIcon(iconPath+"/"+icon), name);
    }
    layout->addWidget(m_poiType, 0, 1, 1, 3);

    m_makePOILayer->setChecked(true);
    layout->addWidget(m_makePOILayer , 2, 0, 1, 4);

    label = new QLabel("Save to:", this);
    layout->addWidget(label , 3, 0);

    layout->addWidget(m_destFilename, 3, 1, 1, 3);

    m_tabWidget->addTab(widget, "&Points of Interest");

    widget = new QWidget(this);
    layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setRowStretch(1, 1);

    label = new QLabel("This Page does not work yet!!!", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label, 0, 0, 1, 2);

    QPushButton *updateList = new QPushButton("Update list", this);
    connect(updateList, SIGNAL(clicked()), this, SLOT(updateSourceList()));
    layout->addWidget(updateList, 0, 2);

    layout->addWidget(m_packageList, 1, 0, 1, 3);

    label = new QLabel("Download into directory:", this);
    layout->addWidget(label, 2, 0);

    layout->addWidget(m_destDir, 2, 1, 1, 2);

    m_tabWidget->addTab(widget, "&Routing data");

    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    resize(320, 240);
}

DownloadWidget::~DownloadWidget()
{
}

void DownloadWidget::setStartLevel(int level)
{
    if (level >= 17) {
        level = 17;
    }
    m_startLevel = level + 1;
    m_levelSpinBox->setMinimum(level + 1);
    m_levelSpinBox->setValue(level +  1);
}

void DownloadWidget::setDownloadRect(const QRectF &rect)
{
    m_dlRect = rect;
    m_up->setText(lat2string(rect.bottom()));
    m_left->setText(lon2string(rect.left()));
    m_right->setText(lon2string(rect.right()));
    m_bottom->setText(lat2string(rect.top()));
}

void DownloadWidget::startDownload()
{
    switch (m_tabWidget->currentIndex()) {
        case 0:
            m_downloadMode = Tiles;
            startDownloadTiles();
            break;
        case 1:
            m_downloadMode = POIs;
            startDownloadPOIs();
            break;
        case 2:
            m_downloadMode = Packages;
            startDownloadPackages();
            break;
    }
}

void DownloadWidget::replyFinished(QNetworkReply *reply)
{
    switch (m_downloadMode) {
        case Tiles:
            replyFinishedTiles(reply);
            break;
        case Packages:
            replyFinishedPackages(reply);
            break;
        case POIs:
            replyFinishedPOIs(reply);
            break;
        case SourceList:
            if (reply->error() == QNetworkReply::NoError) {
                QStringList list = QString::fromUtf8(reply->readAll().constData()).split("\n");
                foreach (const QString &pkg, list) {
                    QListWidgetItem *item = new QListWidgetItem(pkg.section('@', 0, 0));
                    item->setData(Qt::UserRole, QVariant(pkg.section('@', 1)));
                    m_packageList->addItem(item);
                }
            }
            break;
    }
    reply->deleteLater();
}

void DownloadWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_dlProgress->setRange(0, bytesTotal);
    m_dlProgress->setValue(bytesReceived);
}

void DownloadWidget::saveDownload()
{
    if (m_currentDownload->error() != QNetworkReply::NoError) {
        return;
    }
    QString path = m_currentDownload->url().path().section("/", -2);
    QByteArray a = m_currentDownload->readAll();

    QFile file(m_destDir->text()+"/"+path);
    if (file.open(QFile::WriteOnly)) {
        file.write(a);
    }
}

void DownloadWidget::updateSourceList()
{
    m_downloadMode = SourceList;
    m_manager->get(QNetworkRequest(QUrl("http://host.domain.tld/path/source.list")));
}

void DownloadWidget::startDownloadTiles()
{
    m_prefix = m_prefixInput->text();
    m_dlProgress->setValue(0);
    for (int level = m_startLevel; level <= m_levelSpinBox->value(); ++level) {
        int max = pow(2, level) - 1;
        int minX = qBound(0, lon2tilex(m_dlRect.left(), level), max);
        int minY = qBound(0, lat2tiley(m_dlRect.bottom(), level), max);
        int maxX = qBound(0, lon2tilex(m_dlRect.right(), level), max);
        int maxY = qBound(0, lat2tiley(m_dlRect.top(), level), max);
        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                m_dlList << QString("http://tile.openstreetmap.org/%1/%2/%3.png").arg(level).arg(x).arg(y);
            }
        }
    }
    if (!m_dlList.isEmpty()) {
        m_dlProgress->show();
        m_dlProgress->setRange(0, m_dlList.count());
        m_dlProgress->setFormat("%v / %m");
        m_tabWidget->setEnabled(false);
        m_startButton->setEnabled(false);
        m_backButton->setEnabled(false);
        QUrl url(m_dlList.takeFirst());
        m_manager->get(QNetworkRequest(url));
    }
}

void DownloadWidget::replyFinishedTiles(QNetworkReply *reply)
{
    if (m_dlList.isEmpty()) {
        m_dlProgress->hide();
        m_tabWidget->setEnabled(true);
        m_startButton->setEnabled(true);
        m_backButton->setEnabled(true);
    }
    if (reply->error() == QNetworkReply::NoError) {
        QString path = reply->url().path();
        int level = path.section('/', 1, 1).toInt();
        int x = path.section('/', 2, 2).toInt();

        QDir base(QDir::homePath()+"/Maps/"+m_prefix);
        base.mkpath(QString("%1/%2").arg(level).arg(x));

        QByteArray data = reply->readAll();
        if (!data.isEmpty()) {
            QFile file(QDir::homePath()+"/Maps/"+m_prefix+path);
            if (file.open(QFile::WriteOnly)) {
                file.write(data);
            }
        }
        while (!m_dlList.isEmpty()) {
            QUrl url(m_dlList.takeFirst());
            if (QFile::exists(QDir::homePath()+"/Maps/"+m_prefix+url.path()) &&
                m_skipExisting->isChecked()) {	    	    
                int n = m_dlProgress->value();
                m_dlProgress->setValue(n+1);
            } else {
                m_manager->get(QNetworkRequest(url));
                break;
            }
        }
        int n = m_dlProgress->value();
        m_dlProgress->setValue(n+1);
    }
}

void DownloadWidget::startDownloadPOIs()
{
    QString baseUrl("http://azure.openstreetmap.org/xapi/api/0.6/node");
    QString key = m_poiType->currentText().section("-", 0, 0);
    QString value = m_poiType->currentText().section("-", 1);
    QString keyValue = QString("[%1=%2]").arg(key, value);
    QString bbox = QString("[bbox=%1,%2,%3,%4]").arg(m_dlRect.left()).arg(m_dlRect.top())
                                                .arg(m_dlRect.right()).arg(m_dlRect.bottom());
    QUrl url(baseUrl+keyValue+bbox);
    QNetworkReply *reply = m_manager->get(QNetworkRequest(url));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(downloadProgress(qint64, qint64)));

    m_dlProgress->show();
    m_dlProgress->setValue(0);
    m_dlProgress->setFormat("%p%");
    m_tabWidget->setEnabled(false);
    m_startButton->setEnabled(false);
    m_backButton->setEnabled(false);
}

void DownloadWidget::replyFinishedPOIs(QNetworkReply *reply)
{
    m_dlProgress->hide();
    m_tabWidget->setEnabled(true);
    m_startButton->setEnabled(true);
    m_backButton->setEnabled(true);

    if (reply->error() != QNetworkReply::NoError) {
        return;
    }
    QByteArray data = reply->readAll();
    if (data.isEmpty()) {
        return;
    }
    QFile file(m_destFilename->text());
    if (!file.open(QFile::WriteOnly)) {
        return;
    }
    file.write(data);
    file.close();

    if (m_makePOILayer->isChecked()) {
        emit loadFile(m_destFilename->text(), m_poiType->currentText().section("-", 1));
    }
}

void DownloadWidget::startDownloadPackages()
{
    QListWidgetItem *item = m_packageList->currentItem();
    if (!item) {
        return;
    }

    QString baseUrl = item->data(Qt::UserRole).toString();
    m_dlList << baseUrl+"Contraction Hierarchies_config";
    m_dlList << baseUrl+"Contraction Hierarchies_edges";
    m_dlList << baseUrl+"Contraction Hierarchies_names";
    m_dlList << baseUrl+"Contraction Hierarchies_paths";
    m_dlList << baseUrl+"Contraction Hierarchies_types";
    m_dlList << baseUrl+"GPSGrid_config";
    m_dlList << baseUrl+"GPSGrid_grid";
    m_dlList << baseUrl+"GPSGrid_index_1";
    m_dlList << baseUrl+"GPSGrid_index_2";
    m_dlList << baseUrl+"GPSGrid_index_3";
    m_dlList << baseUrl+"OSM Renderer_settings";
    m_dlList << baseUrl+"plugins.ini";
    m_dlList << baseUrl+"Unicode Tournament Trie_main";
    m_dlList << baseUrl+"Unicode Tournament Trie_sub";
    m_dlList << baseUrl+"Unicode Tournament Trie_ways";

    m_currentDownload = m_manager->get(QNetworkRequest(QUrl(m_dlList.takeFirst())));
    connect(m_currentDownload, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(downloadProgress(qint64, qint64)));
    connect(m_currentDownload, SIGNAL(readReady()),
            this, SLOT(saveDownload()));

    m_dlProgress->show();
    m_dlProgress->setValue(0);
    m_dlProgress->setFormat("%p%");
    m_tabWidget->setEnabled(false);
    m_startButton->setEnabled(false);
    m_backButton->setEnabled(false);
}

void DownloadWidget::replyFinishedPackages(QNetworkReply *reply)
{
    Q_UNUSED(reply)

    if (m_dlList.isEmpty()) {
        m_dlProgress->hide();
        m_tabWidget->setEnabled(true);
        m_startButton->setEnabled(true);
        m_backButton->setEnabled(true);
    } else {
        QUrl url(m_dlList.takeFirst());
        m_currentDownload = m_manager->get(QNetworkRequest(url));
        connect(m_currentDownload, SIGNAL(downloadProgress(qint64, qint64)),
                this, SLOT(downloadProgress(qint64, qint64)));
        connect(m_currentDownload, SIGNAL(readReady()),
                this, SLOT(saveDownload()));
    }
}

QString DownloadWidget::lon2string(qreal lon)
{
    QString result;

    if (lon < 0) {
        result = "W ";
        lon *= -1;
    } else {
        result = "E ";
    }
    result.append(QString::number(lon));

    return result;
}

QString DownloadWidget::lat2string(qreal lat)
{
    QString result;

    if (lat < 0) {
        result = "S ";
        lat *= -1;
    } else {
        result = "N ";
    }
    result.append(QString::number(lat));

    return result;
}

int DownloadWidget::lon2tilex(qreal lon, int z)
{
    return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}
 
int DownloadWidget::lat2tiley(qreal lat, int z)
{
    return (int)(floor((1.0 - log(tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}
 

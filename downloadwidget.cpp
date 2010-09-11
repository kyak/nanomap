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

#include "downloadwidget.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

DownloadWidget::DownloadWidget(QWidget *parent)
    : QWidget(parent),
    m_manager(new QNetworkAccessManager(this)),
    m_startLevel(0),
    m_dlRect(),
    m_dlList(),
    m_up(new QLabel("N 0", this)),
    m_left(new QLabel("E 0", this)),
    m_right(new QLabel("E 0", this)),
    m_bottom(new QLabel("N 0", this)),
    m_levelSpinBox(new QSpinBox(this)),
    m_dlProgress(new QProgressBar(this)),
    m_skipExisting(new QCheckBox("S&kip already downloaded tiles", this))
{
    QGridLayout *layout = new QGridLayout(this);
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

    label = new QLabel("Download up to level", this);
    layout->addWidget(label, 3, 0, 1, 2);

    m_levelSpinBox->setRange(0, 18);
    layout->addWidget(m_levelSpinBox, 3, 2, 1, 2);

    m_skipExisting->setChecked(false);
    layout->addWidget(m_skipExisting, 4, 0, 1, 0);

    m_dlProgress->setFormat("%v / %m");
    layout->addWidget(m_dlProgress, 5, 0, 1, 4);

    QPushButton *start = new QPushButton("&Start", this);
    connect(start, SIGNAL(clicked()), this, SLOT(startDownload()));
    layout->addWidget(start, 6, 0, 1, 2);

    QPushButton *back = new QPushButton("&Back", this);
    connect(back, SIGNAL(clicked()), this, SIGNAL(back()));
    layout->addWidget(back, 6, 2, 1, 2);

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
        m_dlProgress->setRange(0, m_dlList.count());
        QUrl url(m_dlList.takeFirst());
        m_manager->get(QNetworkRequest(url));
    }
}

void DownloadWidget::replyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString path = reply->url().path();
        int level = path.section('/', 1, 1).toInt();
        int x = path.section('/', 2, 2).toInt();

        QDir base(QDir::homePath()+"/Maps/OSM");
        base.mkpath(QString("%1/%2").arg(level).arg(x));

        QByteArray data = reply->readAll();
        if (!data.isEmpty()) {
            QFile file(QDir::homePath()+"/Maps/OSM"+path);
            if (file.open(QFile::WriteOnly)) {
                file.write(data);
            }
        }
        while (!m_dlList.isEmpty()) {
            QUrl url(m_dlList.takeFirst());
            if (QFile::exists(QDir::homePath()+"/Maps/OSM"+url.path()) &&
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
    reply->deleteLater();
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
 

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

#include "routingwidget.h"

#include "projection.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>

RoutingWidget::RoutingWidget(QWidget *parent)
    : QWidget(parent),
    m_name(new QLineEdit("route.gpx", this)),
    m_transport(new QComboBox(this)),
    m_quickest(new QRadioButton("Quickest", this)),
    m_shortest(new QRadioButton("Shortest", this)),
    m_routino(),
    m_dir(),
    m_prefix(),
    m_profiles(),
    m_swap(),
    m_from(),
    m_to(),
    m_fromLabel(new QLabel(this)),
    m_toLabel(new QLabel(this))
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setRowStretch(6, 1);

    QLabel *label = new QLabel("From:");
    layout->addWidget(label, 0, 0);

    layout->addWidget(m_fromLabel, 0, 1, 1, 2);

    label = new QLabel("To:");
    layout->addWidget(label, 1, 0);

    layout->addWidget(m_toLabel, 1, 1, 1, 2);

    label = new QLabel("Name:");
    layout->addWidget(label, 2, 0);

    layout->addWidget(m_name, 2, 1, 1, 2);

    label = new QLabel("Transport:");
    layout->addWidget(label, 3, 0);

    m_transport->addItems(QStringList() << "motorcar" << "motorbike" << "bicycle" << "foot");
    layout->addWidget(m_transport, 3, 1, 1, 2);

    label = new QLabel("Route:");
    layout->addWidget(label, 4, 0);

    m_shortest->setChecked(true);
    layout->addWidget(m_shortest, 4, 1, 1, 2);

    layout->addWidget(m_quickest, 5, 1, 1, 2);

    QPushButton *back = new QPushButton("&Show map", this);
    back->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S));
    layout->addWidget(back, 7, 1);
    connect(back, SIGNAL(clicked()), this, SIGNAL(back()));

    QPushButton *find = new QPushButton("&Find route", this);
    find->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F));
    layout->addWidget(find, 7, 2);
    connect(find, SIGNAL(clicked()), this, SLOT(findRoute()));

    QFile run("/tmp/findroute.sh");
    if (run.open(QFile::WriteOnly)) {
        QTextStream out(&run);
        out << "#!/bin/sh\n";
        run.close();
    }
    run.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);

    loadConfig();

    resize(320, 240);
}

RoutingWidget::~RoutingWidget()
{
}

void RoutingWidget::setFrom(const QPointF &from)
{
    m_from = from;
    m_fromLabel->setText(Projection::geo2string(from));
}

void RoutingWidget::setTo(const QPointF &to)
{
    m_to = to;
    m_toLabel->setText(Projection::geo2string(to));
}

void RoutingWidget::findRoute()
{
    QFile run("/tmp/findroute.sh");
    if (run.open(QFile::WriteOnly)) {
        QString file;
        QString name = m_name->text();
        QString command;
        QTextStream out(&run);
        out << "#!/bin/sh\n";
        if (!m_swap.isEmpty()) {
            out << "swapon "+m_swap+"\n";
        }
        command = m_routino;
        command.append(QString(" --dir=%1").arg(m_dir));
        command.append(QString(" --prefix=%1").arg(m_prefix));
        command.append(QString(" --profiles=%1").arg(m_profiles));
        command.append(QString(" --lon1=%1 --lat1=%2 --lon2=%3 --lat2=%4")
                .arg(m_from.x()).arg(m_from.y()).arg(m_to.x()).arg(m_to.y()));
        command.append(" --output-gpx-track");
        if (m_shortest->isChecked()) {
            command.append(" --shortest");
            file = "shortest-track.gpx";
        } else {
            command.append(" --quickest");
            file = "quickest-track.gpx";
        }
        out << command+"\n";
        if (!m_swap.isEmpty()) {
            out << "swapon "+m_swap+"\n";
        }
        out << QString("mv %1 %2\n").arg(file).arg(name);;
        out << QString("exec ./NanoMap.sh %1\n").arg(name);
        qApp->quit();
    }
}

void RoutingWidget::loadConfig()
{
    QSettings set(QDir::homePath()+"/Maps/nanomap.conf", QSettings::NativeFormat);

    set.beginGroup("routino");

    m_routino = set.value("router", "router").toString();
    m_dir = set.value("dir", ".").toString();
    m_prefix = set.value("prefix", "world").toString();
    m_profiles = set.value("profiles", "/usr/share/routino/routino-profiles.xml").toString();
    m_swap = set.value("swap", "").toString();

    set.endGroup();
}


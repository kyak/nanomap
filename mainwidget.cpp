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

#include "mainwidget.h"

#include "mapwidget.h"
#include "markerlist.h"

#include <QtGui/QLayout>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),
    m_stack(new QStackedWidget(this)),
    m_map(new MapWidget(this)),
    m_markerList(new MarkerList(this))
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_stack);

    connect(m_map, SIGNAL(switchView()), this, SLOT(showList()));
    connect(m_map, SIGNAL(markerAdded(QString)), this, SLOT(markerAdded(QString)));
    m_stack->insertWidget(0, m_map);
    connect(m_markerList, SIGNAL(back()), this, SLOT(showMap()));
    connect(m_markerList, SIGNAL(centerOnMarker(int)), this, SLOT(centerOnMarker(int)));
    connect(m_markerList, SIGNAL(removeMarker(int)), this, SLOT(removeMarker(int)));
    connect(m_markerList, SIGNAL(markerRenamed(int, QString)), this, SLOT(markerRenamed(int, QString)));
    m_stack->insertWidget(1, m_markerList);

    resize(320, 240);
}

MainWidget::~MainWidget()
{
}

void MainWidget::showList()
{
    m_stack->setCurrentIndex(1);
}

void MainWidget::markerAdded(const QString &name)
{
    m_markerList->addMarker(name);
}

void MainWidget::showMap()
{
    m_stack->setCurrentIndex(0);
}

void MainWidget::centerOnMarker(int row)
{
    m_map->centerOnMarker(row);
    m_stack->setCurrentIndex(0);
}

void MainWidget::removeMarker(int row)
{
    m_map->removeMarker(row);
}

void MainWidget::markerRenamed(int index, const QString &name)
{
    m_map->renameMarker(index, name);
}


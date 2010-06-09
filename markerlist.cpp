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

#include "markerlist.h"

#include <QtGui/QLayout>
#include <QtGui/QPushButton>

MarkerList::MarkerList(QWidget *parent)
    : QWidget(parent),
    m_list(new QListWidget(this)),
    m_edit(false)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_list, 0, 0, 1, 3);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_list, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(center()));

    QPushButton *back = new QPushButton("Show map", this);
    back->setShortcut(QKeySequence(Qt::Key_Tab));
    layout->addWidget(back, 1, 0);
    connect(back, SIGNAL(clicked()), this, SIGNAL(back()));

    QPushButton *remove = new QPushButton("&Delete", this);
    remove->setShortcut(QKeySequence(Qt::ALT + Qt::Key_D));
    layout->addWidget(remove, 1, 1);
    connect(remove, SIGNAL(clicked()), this, SLOT(removeMarker()));

    QPushButton *rename = new QPushButton("&Rename", this);
    rename->setShortcut(QKeySequence(Qt::ALT + Qt::Key_R));
    layout->addWidget(rename, 1, 2);
    connect(rename, SIGNAL(clicked()), this, SLOT(beginRenameMarker()));

    resize(320, 240);
}

MarkerList::~MarkerList()
{
}

void MarkerList::addMarker(const QString &name)
{
    m_list->addItem(name);
}

void MarkerList::center()
{
    if (m_edit) {
        endRenameMarker();
    } else {
        emit centerOnMarker(m_list->currentRow());
    }
}

void MarkerList::removeMarker()
{
    emit removeMarker(m_list->currentRow());
    m_list->takeItem(m_list->currentRow());
}

void MarkerList::beginRenameMarker()
{
    m_edit = true;
    m_list->openPersistentEditor(m_list->currentItem());
}

void MarkerList::endRenameMarker()
{
    m_edit = false;
    m_list->closePersistentEditor(m_list->currentItem());
    emit markerRenamed(m_list->currentRow(), m_list->currentItem()->text());
}


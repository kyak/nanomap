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

#include "searchwidget.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QSettings>
#include <QtGui/QCompleter>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent),
    m_addrLookup(0),
    m_cityCoordinates(),
    m_loaded(false),
    m_city(new QLineEdit(this)),
    m_cityList(new QListWidget(this)),
    m_street(new QLineEdit(this)),
    m_streetList(new QListWidget(this))
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_city->setPlaceholderText("Enter city name");
    layout->addWidget(m_city, 0, 0, 1, 2);
    connect(m_city, SIGNAL(textEdited(QString)), this, SLOT(cityChanged(QString)));
    connect(m_city, SIGNAL(editingFinished()), this, SLOT(cityEntered()));

    layout->addWidget(m_cityList, 1, 0, 1, 2);
    connect(m_cityList, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(citySelected(QListWidgetItem*)));

    m_street->setPlaceholderText("Enter street name");
    layout->addWidget(m_street, 2, 0, 1, 2);
    connect(m_street, SIGNAL(textEdited(QString)), this, SLOT(streetChanged(QString)));

    layout->addWidget(m_streetList, 3, 0, 1, 2);
    connect(m_streetList, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(streetSelected(QListWidgetItem*)));

    QPushButton *back = new QPushButton("&Cancel", this);
    layout->addWidget(back, 4, 1);
    connect(back, SIGNAL(clicked()), this, SIGNAL(back()));

    QSettings set(QDir::homePath()+"/Maps/nanomap.conf", QSettings::NativeFormat);
    set.beginGroup("monav");
    QString dataDir = set.value("datadir").toString();
    QString addrLookupLib = set.value("addresslookup",
            "/usr/lib/monav/libunicodetournamenttrieclient.so").toString();
    set.endGroup();

    QSettings pluginSettings(dataDir+"/plugins.ini", QSettings::IniFormat);
    QString addrLookupName = pluginSettings.value("addressLookup").toString();

    QPluginLoader Loader(addrLookupLib);
    QObject *plugin = Loader.instance();
    if (plugin) {
        m_addrLookup = qobject_cast<IAddressLookup*>(plugin);
        if (m_addrLookup) {
            m_addrLookup->SetInputDirectory(dataDir);
            m_loaded = m_addrLookup->LoadData();
        }
    }

    resize(320, 240);
}

SearchWidget::~SearchWidget()
{
}

void SearchWidget::cityChanged(const QString &city)
{
    if (m_loaded) {
        QStringList suggestions, inputSuggestions;
        bool found = m_addrLookup->GetPlaceSuggestions(city, 20, &suggestions, &inputSuggestions);
        if (found) {
            QCompleter *c = new QCompleter(suggestions, this);
            c->setCompletionMode(QCompleter::PopupCompletion);
            c->setCaseSensitivity(Qt::CaseInsensitive);
            m_city->setCompleter(c);
        }
    }
}

void SearchWidget::cityEntered()
{
    if (m_loaded) {
        QVector<int> placeIDs;
        QVector<UnsignedCoordinate> placeCoordinates;
        if (m_addrLookup->GetPlaceData(m_city->text(), &placeIDs, &placeCoordinates)) {
            QListWidgetItem *item;
            m_cityList->clear();
            m_cityCoordinates.clear();
            for (int i = 0; i < placeIDs.count(); ++i) {
                GPSCoordinate coord = placeCoordinates.at(i).ToGPSCoordinate();
                QString name = QString("%1: %2 / %3").arg(m_city->text())
                                 .arg(coord.latitude).arg(coord.longitude);
                item = new QListWidgetItem(name, m_cityList);
                item->setData(Qt::UserRole, placeIDs.at(i));
                m_cityList->addItem(item);
                m_cityCoordinates.insert(placeIDs.at(i), coord);
            }
        }
    }
}

void SearchWidget::citySelected(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();
    GPSCoordinate c = m_cityCoordinates.value(id);
    emit centerOn(c.longitude, c.latitude);
}

void SearchWidget::streetChanged(const QString &street)
{
    if (m_loaded) {
        m_addrLookup->SelectPlace(m_cityList->currentItem()->data(Qt::UserRole).toInt());
        QStringList suggestions, inputSuggestions;
        bool found = m_addrLookup->GetStreetSuggestions(street, 20, &suggestions, &inputSuggestions);
        if (found) {
            m_streetList->clear();
            m_streetList->insertItems(0, suggestions);
        }
    }
}

void SearchWidget::streetSelected(QListWidgetItem *item)
{
    QVector<int> segmentLength;
    QVector<UnsignedCoordinate> coordinates;
    if (m_addrLookup->GetStreetData(item->text(), &segmentLength, &coordinates)) {
        GPSCoordinate coord = coordinates.first().ToGPSCoordinate();
        emit centerOn(coord.longitude, coord.latitude);
    }
}


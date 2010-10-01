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

#ifndef ABSTRACT_LAYER_H
#define ABSTRACT_LAYER_H

#include <QtCore/QObject>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

class MapWidget;

class AbstractLayer : public QObject
{
    Q_OBJECT
public:
    AbstractLayer(MapWidget *map);

    virtual void load(const QString &filename);
    virtual void zoom(int level);
    virtual void pan(const QPoint &move);
    virtual void triggerAction();
    virtual void keyPressed(QKeyEvent *event);

    void paintLayer(QPainter *painter);

    bool isVisible() const;

public slots:
    void setVisible(bool visible = true);
    void toggleVisibility();

signals:
    void visibilityChanged(bool visible);

protected:
    MapWidget *map() const;
    virtual void paint(QPainter *painter) = 0;

private:
    MapWidget *m_map;
    bool m_visible;

};

#endif // ABSTRACT_LAYER_H

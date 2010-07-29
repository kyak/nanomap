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

#ifndef ROUTINGWIDGET_H
#define ROUTINGWIDGET_H

#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

class RoutingWidget : public QWidget
{
    Q_OBJECT
public:
    RoutingWidget(QWidget *parent = 0);
    ~RoutingWidget();

    void setFrom(const QPointF &from);
    void setTo(const QPointF &to);

signals:
    void back();

private slots:
    void findRoute();

private:
    void loadConfig();

    QLineEdit *m_name;
    QComboBox *m_transport;
    QRadioButton *m_quickest;
    QRadioButton *m_shortest;
    QString m_routino, m_dir, m_prefix, m_profiles, m_swap;
    QPointF m_from, m_to;
    QLabel *m_fromLabel, *m_toLabel;
 
};

#endif // ROUTINGWIDGET_H

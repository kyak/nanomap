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

#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QtCore/QSignalMapper>
#include <QtGui/QFileSystemModel>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

class FileSelector : public QWidget
{
    Q_OBJECT
public:
    FileSelector(QWidget *parent = 0);
    ~FileSelector();

    void setTitle(const QString &title);
    void setFileTypes(const QStringList &types);

signals:
    void fileSelected(const QString &fileName);
    void cancel();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void toggleBookmark();
    void goUp();
    void accept();
    void enter(const QModelIndex &index);
    void setCurrentDirectory(const QString &dir);

private:
    void setCurrentDirectory(const QModelIndex &dir);
    void updateBookmarkMenu();
    void setPathLabel(const QString &dir);
    QString shortText(const QString &text, int width);

    QListView *m_view;
    QFileSystemModel *m_model;
    QLabel *m_title, *m_path;
    QPushButton *m_bookmarkButton;
    QMenu *m_bookmarkMenu;
    QStringList m_bookmarks;
    QSignalMapper *m_signalMapper;

};

#endif // FILESELECTOR_H

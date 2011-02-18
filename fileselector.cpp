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

#include "fileselector.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QKeyEvent>
#include <QtGui/QLayout>

FileSelector::FileSelector(QWidget *parent)
    : QWidget(parent),
    m_view(new QListView(this)),
    m_model(new QFileSystemModel(this)),
    m_title(new QLabel(this)),
    m_path(new QLabel(this)),
    m_bookmarkButton(new QPushButton(this)),
    m_bookmarkMenu(new QMenu(this)),
    m_bookmarks(),
    m_signalMapper(new QSignalMapper(this))
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setRowStretch(5, 1);
    layout->setSpacing(0);

    m_title->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_title, 0, 0, 1, 2);
    layout->addWidget(m_path, 1, 0, 1, 2);

    m_view->setModel(m_model);
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(enter(QModelIndex)));
    layout->addWidget(m_view, 2, 0, 6, 1);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setSelectionMode(QListView::ExtendedSelection);
    QFont font = m_view->font();
    font.setPointSize(8);
    m_view->setFont(font);

    m_model->setNameFilterDisables(false);
    m_model->setRootPath("/");

    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(setCurrentDirectory(QString)));

    m_bookmarkButton->setIcon(QIcon(":nobookmark.png"));
    m_bookmarkButton->setShortcut(QKeySequence(Qt::ALT+Qt::Key_B));
    connect(m_bookmarkButton, SIGNAL(clicked()), this, SLOT(toggleBookmark()));
    layout->addWidget(m_bookmarkButton, 2, 1);

    QPushButton *button = new QPushButton();
    button->setIcon(QIcon(":up.png"));
    button->setShortcut(QKeySequence(Qt::ALT+Qt::Key_Up));
    connect(button, SIGNAL(clicked()), this, SLOT(goUp()));
    layout->addWidget(button, 3, 1);

    button = new QPushButton();
    button->setIcon(QIcon(":ok.png"));
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    layout->addWidget(button, 6, 1);

    button = new QPushButton();
    button->setIcon(QIcon(":cancel.png"));
    button->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(button, SIGNAL(clicked()), this, SIGNAL(cancel()));
    layout->addWidget(button, 7, 1);

    QSettings conf(QDir::homePath()+"Maps/nanomap.conf", QSettings::NativeFormat);
    conf.beginGroup("fileselector");
    m_bookmarks = conf.value("bookmarks").toStringList();
    conf.endGroup();

    setCurrentDirectory(m_model->index(QDir::homePath()));
    updateBookmarkMenu();

    m_view->setFocus(Qt::OtherFocusReason);
    resize(320, 240);
}

FileSelector::~FileSelector()
{
    QSettings conf(QDir::homePath()+"Maps/nanomap.conf", QSettings::NativeFormat);
    conf.beginGroup("fileselector");
    conf.setValue("bookmarks", m_bookmarks);
    conf.endGroup();
}

void FileSelector::setTitle(const QString &title)
{
    m_title->setText("<u>"+title+"</u>");
}

void FileSelector::setFileTypes(const QStringList &types)
{
    m_model->setNameFilters(types);
}

void FileSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_B) {
        m_bookmarkMenu->popup(mapToGlobal(QPoint(0, 0)));
    }
}

void FileSelector::toggleBookmark()
{
    QString path = m_model->filePath(m_view->rootIndex());
    if (m_bookmarks.contains(path)) {
        m_bookmarks.removeAll(path);
        m_bookmarkButton->setIcon(QIcon(":nobookmark.png"));
    } else {
        m_bookmarks.append(path);
        m_bookmarkButton->setIcon(QIcon(":bookmark.png"));
    }
    updateBookmarkMenu();
}

void FileSelector::goUp()
{
    QModelIndex current = m_view->rootIndex();
    if (current.isValid()) {
        QModelIndex up = current.parent();
        if (up.isValid()) {
            setCurrentDirectory(up);
        }
    }
}

void FileSelector::accept()
{
    QModelIndex index = m_view->currentIndex();
    if (index.isValid()) {
        if (!m_model->isDir(index)) {
            emit fileSelected(m_model->filePath(index));
        }
    }
}

void FileSelector::enter(const QModelIndex &index)
{
    if (m_model->isDir(index)) {
        setCurrentDirectory(index);
    } else {
        QModelIndexList selected = m_view->selectionModel()->selectedIndexes();
        QStringList files;
        foreach (const QModelIndex &i, selected) {
            files << m_model->filePath(i);
        }
        emit fileSelected(files.first());
    }
}

void FileSelector::setCurrentDirectory(const QString &dir)
{
    setCurrentDirectory(m_model->index(dir));
}

void FileSelector::setCurrentDirectory(const QModelIndex &dir)
{
    m_view->setRootIndex(dir);
    QString path = m_model->filePath(dir);
    setPathLabel(path);
    if (m_bookmarks.contains(path)) {
        m_bookmarkButton->setIcon(QIcon(":bookmark.png"));
    } else {
        m_bookmarkButton->setIcon(QIcon(":nobookmark.png"));
    }
}

void FileSelector::updateBookmarkMenu()
{
    m_bookmarkMenu->clear();

    if (m_bookmarks.isEmpty()) {
        QAction *action = new QAction("No bookmarks", m_bookmarkMenu);
        action->setIcon(QIcon(":nobookmark.png"));
        m_bookmarkMenu->addAction(action);
        return;
    }

    foreach (const QString &bm, m_bookmarks) {
        QAction *action = new QAction(shortText(bm, 290), m_bookmarkMenu);
        action->setIcon(QIcon(":bookmark.png"));
        m_bookmarkMenu->addAction(action);
        connect(action, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(action, bm);
    }
}

void FileSelector::setPathLabel(const QString &dir)
{
    m_path->setText(shortText(dir, 310));
}

QString FileSelector::shortText(const QString &text, int width)
{
    QFontMetrics fm(m_path->font());

    QString shortText = text;
    int n = 0;
    while (fm.width(shortText) > width) {
        ++n;
        shortText = text;
        shortText.replace(0, n, "...");
    }

    return shortText;
}


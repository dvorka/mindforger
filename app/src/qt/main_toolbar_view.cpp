/*
 main_toolbar_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "main_toolbar_view.h"

namespace m8r {

MainToolbarView::MainToolbarView(MainWindowView* mainWindowView)
    : QToolBar{tr("Main Toolbar"), mainWindowView}, mainWindow{mainWindowView}
{
    // TOOLBAR: L&F driven toolbar icons - dark vs. light

    actionNewOutlineOrNote = addAction(
        QIcon(":/icons/new.svg"),
        //QIcon(":/menu-icons/new.svg"),
        tr("New Notebook"));
    addSeparator();
    actionOpenRepository = addAction(
        QIcon(":/icons/open-repository.svg"),
        //QIcon(":/menu-icons/folder.svg"),
        tr("Open a directory with Markdowns or MindForger repository"));
    actionOpenFile = addAction(
        //QIcon(":/menu-icons/open.svg"),
        QIcon(":/icons/open-file.svg"),
        tr("Open Markdown file"));
    addSeparator();
    actionHomeOutline = addAction(
        QIcon(":/icons/home.svg"),
        //QIcon(":/menu-icons/home.svg"),
        "Open home Notebook");
    actionBackToPreviousNote = addAction(
        QIcon(":/icons/back.svg"),
        "Back to Previous Note");
    addSeparator();
    actionViewEisenhower = addAction(
        QIcon(":/icons/view-eisenhower.svg"),
        tr("View Eisenhower Matrix"));
    actionViewOutlines = addAction(
        QIcon(":/icons/view-outlines.svg"),
        tr("View Notebooks"));
    actionViewNavigator = addAction(
        QIcon(":/icons/view-navigator.svg"),
        tr("View Knowledge Graph Navigator"));
    actionViewTags = addAction(
        QIcon(":/icons/view-tags.svg"),
        tr("View Tags"));
    actionViewRecentNotes = addAction(
        QIcon(":/icons/view-recent-notes.svg"),
        tr("View Recent Notes"));
    addSeparator();
    actionFindFts = addAction(
        QIcon(":/icons/find-fts.svg"),
        "Full-text search");
    actionFindObyName = addAction(
        QIcon(":/icons/find-o-name.svg"),
        "Recall Notebook by Name");
    actionFindNbyName = addAction(
        QIcon(":/icons/find-n-name.svg"),
        "Recall Note by Name");
    actionFindObyTag = addAction(
        QIcon(":/icons/find-o-tag.svg"),
        "Recall Notebook by Tag");
    actionFindNbyTag = addAction(
        QIcon(":/icons/find-n-tag.svg"),
        "Recall Note by Tag");
    addSeparator();
    actionThink = addAction(
        QIcon(":/icons/think.svg"),
        //QIcon(":/menu-icons/on.svg"),
        "Think / Sleep");
    addSeparator();
    actionScope = addAction(
        QIcon(":/icons/scope.svg"),
        //QIcon(":/menu-icons/filter.svg"),
        "Scope Mind");
    actionAdapt = addAction(
        QIcon(":/icons/adapt.svg"),
        //QIcon(":/menu-icons/configure.svg"),
        "Adapt");
    addSeparator();
    actionHelp = addAction(
        QIcon(":/icons/help.svg"),
        "Open Documentation");
}

MainToolbarView::~MainToolbarView()
{
}

} // m8r namespace

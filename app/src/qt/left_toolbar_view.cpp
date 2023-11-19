/*
 left_toolbar_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "left_toolbar_view.h"

namespace m8r {

LeftToolbarView::LeftToolbarView(MainWindowView* mainWindowView)
    : QToolBar{tr("Tool Bar"), mainWindowView},
      mainWindow{mainWindowView}
{
    actionLeftToolbarArxiv = addAction(
        QIcon(":/icons/arxiv.png"),
        "Open arXiv and find papers related to the current context... (Alt-1)");

    actionLeftToolbarWikipedia = addAction(
        QIcon(":/icons/wikipedia.png"),
        "Open Wikipedia and find entry related to the current context... (Alt-2)");

    actionLeftToolbarStackOverflow = addAction(
        QIcon(":/icons/stackoverflow.png"),
        "Open StackOverflow and find entry related to the current context... (Alt-3)");

    actionLeftToolbarH2oGpt= addAction(
        QIcon(":/icons/h2oai.png"),
        "Open h2oGPT and chat about entry related to the current context... (Alt-4)");

    actionLeftToolbarDuckDuckGo = addAction(
        QIcon(":/icons/duckduckgo.png"),
        "Open DuckDuckGo and find entry related to the current context... (Alt-5)");

    // TODO "Open DuckDuckGo and search web for the selected entity..."

    // TODO "Let chatGPT to explaine in simple terms..."
    // TODO "Use Gramarly to check to grammar..."
    // TODO "Use Pandoc to convert MindForger's Markdown documents..."
    // TODO "Build your web with MindForger's Markdown documents and Docusaurus..."

}

LeftToolbarView::~LeftToolbarView()
{
}

} // m8r namespace

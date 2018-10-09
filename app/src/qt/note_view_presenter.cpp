/*
 note_view_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "note_view_presenter.h"

#include "look_n_feel.h"

using namespace std;

namespace m8r {

NoteViewPresenter::NoteViewPresenter(NoteView* view, OrlojPresenter* orloj)
    : config(Configuration::getInstance())
{
    this->mind = orloj->getMind();

    // IMPORTANT: pre-allocate string using reserve() to ensure good append performance
    html = string{};
    html.reserve(10000);
    qHtml = QString();
    qHtml.reserve(10000);

    this->view = view;
    this->orloj = orloj;
    this->model = new NoteViewModel();
    this->view->setModel(this->model);

    this->markdownRepresentation
        = orloj->getMainWindow()->getMarkdownRepresentation();
    this->htmlRepresentation
        = orloj->getMainWindow()->getHtmlRepresentation();

    this->currentNote = nullptr;

#ifdef MF_QT_WEB_ENGINE
    QObject::connect(view->getPage(), SIGNAL(signalLinkClicked(QUrl)), this, SLOT(slotLinkClicked(QUrl)));
#else
    QObject::connect(view, SIGNAL(linkClicked(QUrl)), this, SLOT(slotLinkClicked(QUrl)));
#endif
    QObject::connect(view, SIGNAL(signalMouseDoubleClickEvent()), this, SLOT(slotEditNote()));
    QObject::connect(view, SIGNAL(signalFromViewNoteToOutlines()), orloj, SLOT(slotShowOutlines()));
}

NoteViewPresenter::~NoteViewPresenter()
{
    if(markdownRepresentation) delete markdownRepresentation;
    if(htmlRepresentation) delete htmlRepresentation;
}

// IMPROVE first decorate MD with HTML colors > then MD to HTML conversion
void NoteViewPresenter::refresh(Note* note)
{
    note->makeRead();
    this->currentNote = note;

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_FTS_RESULT) || orloj->isFacetActive(OrlojPresenterFacets::FACET_FTS_VIEW_NOTE)) {
        // FTS result HTML
        html = "<html><body style='";
        htmlRepresentation->fgBgTextColorStyle(html);
        html += "'><pre>";
        qHtml = QString::fromStdString(html);
        markdownRepresentation->to(note, &html);
        qHtml += QString::fromStdString(html);
        qHtml += QString::fromStdString("</pre></body></html>");

        // highlight matches
        QString highlighted = QString::fromStdString("<span style='background-color: red; color: white;'>");
        // IMPROVE instead of searched expression that MAY differ in CASE, here should be original string found in the haystack
        highlighted += searchExpression;
        highlighted += QString::fromStdString("</span>");
        qHtml.replace(searchExpression, highlighted, searchIgnoreCase?Qt::CaseInsensitive:Qt::CaseSensitive);
    } else {
        // HTML
        htmlRepresentation->to(note, &html, Configuration::getInstance().isAutolinking());
        qHtml= QString::fromStdString(html);
    }

    view->setHtml(qHtml);

    // leaderboard
    mind->associate();
}

void NoteViewPresenter::slotLinkClicked(const QUrl& url)
{
    orloj->getMainWindow()->handleNoteViewLinkClicked(url);
}

void NoteViewPresenter::slotEditNote()
{    
    orloj->showFacetNoteEdit(this->currentNote);
}

void NoteViewPresenter::slotRefreshLeaderboardByValue(AssociatedNotes* associations)
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) // leaderboard for edit @ view
    {
        orloj->getOutlineView()->getAssocLeaderboard()->refresh(associations);
        delete associations;
    }
}

} // m8r namespace

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
    : qHtml{}, // IMPROVE stack overflow if HTML too big?
      config(Configuration::getInstance())
{
    this->mind = orloj->getMind();

    this->html = new string{};

    this->view = view;
    this->orloj = orloj;
    this->model = new NoteViewModel();
    this->view->setModel(this->model);

    this->markdownRepresentation
        = orloj->getMainWindow()->getMarkdownRepresentation();
    this->htmlRepresentation
        = orloj->getMainWindow()->getHtmlRepresentation();

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
    note->incReads();
    this->currentNote = note;

    // FTS or HTML
    if(!searchExpression.isEmpty()) {
        // TODO HtmlBuilderClass
        // header
        qHtml = QString::fromUtf8("<html><body style='color: ");
        qHtml += LookAndFeels::getInstance().getTextColor();
        qHtml += QString::fromUtf8("'><pre>");

        // N
        markdownRepresentation->to(note, html);
        qHtml += QString::fromStdString(*html);

        // highlight
        QString highlighted = QString::fromStdString("<span style='background-color: red; color: white;'>");
        // IMPROVE instead of searched expression that MAY differ in CASE, here should be original string found in the haystack
        highlighted += searchExpression;
        highlighted += QString::fromStdString("</span>");
        qHtml.replace(searchExpression, highlighted, searchIgnoreCase?Qt::CaseInsensitive:Qt::CaseSensitive);

        // footer
        qHtml += QString::fromStdString("</pre></body></html>");
    } else {
        htmlRepresentation->to(note, html);
        qHtml= QString::fromStdString(*html);
    }

    view->setHtml(qHtml);

    // leaderboard
    std::vector<std::pair<Note*,float>> associatedNotesLeaderboard;
    shared_future<bool> f = mind->getAssociatedNotes(note, associatedNotesLeaderboard); // move
    if(f.wait_for(chrono::microseconds(0)) == future_status::ready) {
        if(f.get()) {
            orloj->getOutlineView()->getAssocLeaderboard()->refresh(associatedNotesLeaderboard);
        } else {
            orloj->getOutlineView()->getAssocLeaderboard()->getView()->setVisible(false);
        }
    } else {
        orloj->getOutlineView()->getAssocLeaderboard()->getView()->setVisible(false);
        // ask notifications distributor to repaint leaderboard later
        AsyncTaskNotificationsDistributor::Task* task = new AsyncTaskNotificationsDistributor::Task{f,AsyncTaskNotificationsDistributor::TaskType::NOTE_ASSOCIATIONS};
        task->setNote(note);
        orloj->getMainWindow()->getDistributor()->add(task);
    }
}

void NoteViewPresenter::slotEditNote()
{
    orloj->showFacetNoteEdit(this->currentNote);
}

void NoteViewPresenter::slotRefreshLeaderboard(Note* note)
{
    // show leaderboard only if it's needed & it's ready
    std::vector<std::pair<Note*,float>> associatedNotesLeaderboard;
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE) && currentNote == note) {
        shared_future<bool> f = mind->getAssociatedNotes(note, associatedNotesLeaderboard); // move
        if(f.wait_for(chrono::microseconds(0)) == future_status::ready) {
            if(f.get()) {
                orloj->getOutlineView()->getAssocLeaderboard()->refresh(associatedNotesLeaderboard);
            }
        }
    }
}

void NoteViewPresenter::slotRefreshLeaderboardByValue(vector<pair<Note*,float>>* associations)
{
    // show leaderboard only if it's needed & it's ready
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getOutlineView()->getAssocLeaderboard()->refresh(*associations);
    }
    delete associations;
}

} // m8r namespace

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

    QObject::connect(view, SIGNAL(linkClicked(QUrl)), this, SLOT(slotLinkClicked(QUrl)));
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

    if(!searchExpression.isEmpty()) {
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
        htmlRepresentation->to(note, &html);
        qHtml= QString::fromStdString(html);
    }

    view->setHtml(qHtml);

    // leaderboard
    std::vector<std::pair<Note*,float>> associatedNotesLeaderboard;
    shared_future<bool> f = mind->getAssociatedNotes(note, associatedNotesLeaderboard);
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

void NoteViewPresenter::slotLinkClicked(const QUrl& url) {
    MF_DEBUG("HTML click handler: " << url.toString().toStdString() << std::endl);
    orloj->getMainWindow()->getStatusBar()->showInfo(QString(tr("Hyperlink %1 clicked...")).arg(url.toString()));

    if(url.toString().size()) {
        if(url.toString().startsWith("file://")) {
            string key{url.toString().toStdString()};
            key.erase(0,7); // remove file prefix
            size_t offset;
            if((offset = key.find("#")) != string::npos) {
                // it CAN be Note
                key.erase(offset);
                MF_DEBUG("  O lookup using key: " << key << std::endl);

                // IMPROVE find note within outline
                Outline* o=orloj->getMind()->remind().getOutline(key);
                if(o) {
                    // Notebook for hyperlink found
                    orloj->showFacetOutline(o);
                    return;
                } // else fallback to open using desktop services
            } else {
                // it CAN be Outline
                MF_DEBUG("  O lookup using key: " << key << std::endl);
                Outline* o=orloj->getMind()->remind().getOutline(key);
                if(o) {
                    // Notebook for hyperlink found
                    orloj->showFacetOutline(o);
                    return;
                } // else fallback to open using desktop services
            }

            // IMPROVE let Qt to open also directories and external files
            MF_DEBUG("Unable to find Notebook/Note for hyperlink: " << url.toString().toStdString() << " > delegating to OS" << std::endl);
            if(!QDesktopServices::openUrl(url)) {
                MF_DEBUG("FAILED to open hyperlink: " << url.toString().toStdString() << std::endl);
            }
        } else {
            // launch URL in browser
            QDesktopServices::openUrl(url);
        }
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
        shared_future<bool> f = mind->getAssociatedNotes(note, associatedNotesLeaderboard);
        if(f.wait_for(chrono::microseconds(0)) == future_status::ready) {
            if(f.get()) {
                orloj->getOutlineView()->getAssocLeaderboard()->refresh(associatedNotesLeaderboard);
            }
        }
    }
}

void NoteViewPresenter::slotRefreshLeaderboardByValue(vector<pair<Note*,float>>* associations)
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE))
    {
        orloj->getOutlineView()->getAssocLeaderboard()->refresh(*associations);
    }
    delete associations;
}

} // m8r namespace

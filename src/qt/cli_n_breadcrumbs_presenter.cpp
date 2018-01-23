/*
 cli_n_breadcrumbs_presenter.cpp     MindForger thinking notebook

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
#include "cli_n_breadcrumbs_presenter.h"

using namespace std;

namespace m8r {

CliAndBreadcrumbsPresenter::CliAndBreadcrumbsPresenter(
        const MainWindowPresenter* mainPresenter,
        CliAndBreadcrumbsView* view,
        Mind* mind)
    : mainPresenter(mainPresenter), view(view), mind(mind)
{
    // wire signals
    QObject::connect(
        view->cli, SIGNAL(returnPressed()),
        this, SLOT(executeCommand()));
    QObject::connect(
        view->cli, SIGNAL(textChanged(QString)),
        this, SLOT(handleCliTextChanged(QString)));
}

void CliAndBreadcrumbsPresenter::handleCliTextChanged(const QString& text)
{
    // IMPROVE remove parameter text if it's not needed
    UNUSED_ARG(text);

    QString command = view->getCommand();
    if(command.size()) {
        // TODO convert to qstring once
        // TODO commands are constants
        if(command.startsWith(QString::fromUtf8("find outline by name "))) {
            QString prefix(QString::fromStdString(command.toStdString().substr(20)));
            if(prefix.size()) {
                mainPresenter->getStatusBar()->showInfo(prefix);
                if(prefix.size()==1) {
                    // switch suggestions model
                    vector<string> outlineTitles;
                    mind->getOutlineTitles(outlineTitles);
                    QStringList outlineTitlesCompletion = QStringList();
                    if(outlineTitles.size()) {
                        QString qs;
                        for(const string s:outlineTitles) {
                            qs.clear();
                            // TODO commands are constants
                            qs.append("find outline by name ");
                            qs.append(QString::fromStdString(s));
                            outlineTitlesCompletion << qs;
                        }
                    }
                    view->updateCompleterModel(&outlineTitlesCompletion);
                } else {
                }
            } else {
                view->updateCompleterModel();
            }
            return;
        }
    }

    // fallback
    view->forceInitialCompletion();
}

// TODO i18n
void CliAndBreadcrumbsPresenter::executeCommand(void)
{
    QString command = view->getCommand();
    if(command.size()) {
        // TODO convert to qstring once
        if(command.startsWith(QString::fromUtf8("fts "))) {
            executeFts(command);
            view->showBreadcrumb();
            return;
        }
        // TODO constant
        if(command.startsWith(QString::fromUtf8("list outlines"))) {
            executeListOutlines();
            view->showBreadcrumb();
            return;
        }
        if(command.startsWith(view->CMD_FIND_OUTLINE_BY_NAME)) {
            string title = command.toStdString().substr(21);
            unique_ptr<vector<Outline*>> outlines = mind->findOutlineByTitleFts(title);
            if(!outlines || !outlines->size()) {
                // IMPROVE memory leak if outlines && !outlines->size()
                QString firstCompletion = view->getFirstCompletion();
                if(firstCompletion != QString::null) {
                    title = view->getFirstCompletion().toStdString().substr(21);
                    outlines = mind->findOutlineByTitleFts(title);
                }
            }
            if(outlines && outlines->size()) {
                mainPresenter->getOrloj()->showFacetOutline(outlines->front());
                // TODO efficient
                mainPresenter->getStatusBar()->showInfo(QString("Outline ")+QString::fromStdString(outlines->front()->getTitle()));
            } else {
                mainPresenter->getStatusBar()->showInfo(QString("Outline not found: ").append(QString(title.c_str())));
            }
            view->showBreadcrumb();
            return;
        }
        mainPresenter->getStatusBar()->showError(QString::fromUtf8("Unknown command!"));
    } else {
        mainPresenter->getStatusBar()->showError(QString::fromUtf8("No command!"));
    }
}

void CliAndBreadcrumbsPresenter::executeFts(QString& command)
{
    string searchedString = command.toStdString().substr(4);
    if(!searchedString.empty()) {
        // IMPROVE get ignore case from the command and pass it as 2nd parameter
        mainPresenter->executeFts(searchedString);
    }
}

void CliAndBreadcrumbsPresenter::executeListOutlines(void)
{
    mainPresenter->getOrloj()->showFacetOutlineList(mind->remind().getOutlines());
}

void CliAndBreadcrumbsPresenter::executeListNotes(void)
{
    if(mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       mainPresenter->getOrloj()->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        Outline* o = mainPresenter->getOrloj()->getOutlineView()->getCurrentOutline();
        if(o) {
            auto notes = o->getNotes();
            // TODO push notes to CLI completer > filter > ENTER outline's note in view
            if(!notes.empty()) {
                mainPresenter->getOrloj()->showFacetNoteEdit(notes.front());
            }
        }

        // TODO show note on enter (actions handler)
        mainPresenter->getStatusBar()->showInfo(QString("Listing notes..."));
        return;
    }
    // else show all notes in MF (scalability?)
    mainPresenter->getStatusBar()->showInfo(QString("No notes to list!"));
}

}

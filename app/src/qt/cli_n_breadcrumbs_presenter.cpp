/*
 cli_n_breadcrumbs_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
        MainWindowPresenter* mainPresenter,
        CliAndBreadcrumbsView* view,
        Mind* mind)
    : mainPresenter(mainPresenter), view(view), mind(mind)
{
    // widgets
    view->setVisible(Configuration::getInstance().isUiShowBreadcrump());

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
        if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME)) {
            QString prefix(QString::fromStdString(
                 command.toStdString().substr(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME.size()-1)));
            if(prefix.size()) {
                mainPresenter->getStatusBar()->showInfo(prefix);
                if(prefix.size()==1) {
                    // switch suggestions model
                    vector<string> outlineNames;
                    mind->getOutlineNames(outlineNames);
                    QStringList outlineNamesCompletion = QStringList();
                    if(outlineNames.size()) {
                        QString qs;
                        for(const string& s:outlineNames) {
                            qs.clear();
                            // TODO commands are constants
                            qs += CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME;
                            qs += QString::fromStdString(s);
                            outlineNamesCompletion << qs;
                        }
                    }
                    view->updateCompleterModel(&outlineNamesCompletion);
                } else {
                }
            } else {
                view->updateCompleterModel();
            }
            return;
        }
    }

    // fallback
    view->forceFtsHistoryCompletion();
}

// TODO i18n
void CliAndBreadcrumbsPresenter::executeCommand()
{
    QString command = view->getCommand();
    if(command.size()) {
        view->addCompleterItem(command);

        if(command.startsWith(CliAndBreadcrumbsView::CMD_FTS)) {
            executeFts(command);
            view->showBreadcrumb();
            return;
        }
        if(command.startsWith(CliAndBreadcrumbsView::CMD_LIST_OUTLINES)) {
            executeListOutlines();
            view->showBreadcrumb();
            return;
        }
        if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME)) {
            string name = command.toStdString().substr(
                CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME.size());
            unique_ptr<vector<Outline*>> outlines = mind->findOutlineByNameFts(name);
            if(!outlines || !outlines->size()) {
                // IMPROVE memory leak if outlines && !outlines->size()
                QString firstCompletion = view->getFirstCompletion();
                if(firstCompletion.size()) {
                    name = view->getFirstCompletion().toStdString().substr(
                        CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME.size()
                    );
                    outlines = mind->findOutlineByNameFts(name);
                }
            }
            if(outlines && outlines->size()) {
                mainPresenter->getOrloj()->showFacetOutline(outlines->front());
                // TODO efficient
                mainPresenter->getStatusBar()->showInfo(tr("Notebook ")+QString::fromStdString(outlines->front()->getName()));
            } else {
                mainPresenter->getStatusBar()->showInfo(tr("Notebook not found: ") += QString(name.c_str()));
            }
            view->showBreadcrumb();
            return;
        }

        // do FTS as fallback
        mainPresenter->doFts(view->getCommand(), true);
    } else {
        mainPresenter->getStatusBar()->showError(tr("No command!"));
    }
}

void CliAndBreadcrumbsPresenter::executeFts(QString& command)
{
    string searchedString = command.toStdString().substr(
         CliAndBreadcrumbsView::CMD_FTS.size());
    if(!searchedString.empty()) {
        mainPresenter->doFts(QString::fromStdString(searchedString), true);
    }
}

void CliAndBreadcrumbsPresenter::executeListOutlines()
{
    mainPresenter->getOrloj()->showFacetOutlineList(mind->getOutlines());
}

// TODO call main window handler
void CliAndBreadcrumbsPresenter::executeListNotes()
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

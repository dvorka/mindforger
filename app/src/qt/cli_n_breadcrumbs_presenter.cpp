/*
 cli_n_breadcrumbs_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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

    // wire signals (view events to presenter handlers)
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

    // TODO use status bar

    QString command = view->getCommand();
    MF_DEBUG("CLI text changed to: '" << command.toStdString() << "'" << endl);
    if(command.size()) {
        MF_DEBUG("  handling:" << endl);
        if(command.startsWith(CliAndBreadcrumbsView::CHAR_HELP)) {
            MF_DEBUG("    HELP" << endl);
            QMessageBox::information(
                &mainPresenter->getView(),
                tr("Wingman help"),
                tr(
                    // IMPROVE consider <html> prefix, <br/> separator and colors/bold
                    "Use the following commands to use Wingman:\n"
                    "\n"
                    "? ... help\n"
                    "/ ... search\n"
                    "@ ... knowledge recherche\n"
                    "> ... run a command\n"
                    ": ... chat with workspace, Notebook or Note\n"
                    "\n"
                    "or type full-text search phrase\n"
                )
            );
            view->setCommand("");
            mainPresenter->getStatusBar()->showInfo(
                tr("Wingman: ? for help, / search, @ knowledge, > command, : chat, or type FTS phrase"));
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CHAR_FIND)) {
            MF_DEBUG("    / HELP find" << endl);
            view->updateCompleterModel(CliAndBreadcrumbsView::HELP_FIND_CMDS);
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CHAR_KNOW)) {
            MF_DEBUG("    @ HELP knowledge" << endl);
            view->updateCompleterModel(CliAndBreadcrumbsView::HELP_KNOW_CMDS);
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CHAR_CMD)) {
            MF_DEBUG("    > HELP command" << endl);
            view->updateCompleterModel(CliAndBreadcrumbsView::HELP_CMD_CMDS);
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CHAR_KNOW)) {
            MF_DEBUG("    @ EXEC" << endl);
            QString prefix(
                QString::fromStdString(
                    command.toStdString().substr(
                        CliAndBreadcrumbsView::CHAR_KNOW.size()-1)));
            QString phrase{"PHRASE"};
            mainPresenter->doActionOpenRunToolDialog(phrase);
            view->setCommand("");
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME)) {
            QString prefix(
                QString::fromStdString(
                    command.toStdString().substr(
                        CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME.size()-1)));
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
                    view->updateCompleterModel(
                        CliAndBreadcrumbsView::DEFAULT_CMDS,
                        &outlineNamesCompletion);
                } else {
                    // TODO NOT handled
                }
            } else {
                MF_DEBUG("    FALLBACK (default CMDs)" << endl);
                view->updateCompleterModel(
                    CliAndBreadcrumbsView::DEFAULT_CMDS);
            }
            return;
        }
        MF_DEBUG("    NO HANDLING (FTS phrase OR lost focus)" << endl);
        return;
    } else { // empty command
        MF_DEBUG("    EMPTY command > NO handling" << endl);
        return;
    }
}

// TODO i18n
void CliAndBreadcrumbsPresenter::executeCommand()
{
    QString command = view->getCommand();
    MF_DEBUG("CLI command EXEC: '" << command.toStdString() << "'" << endl);
    if(command.size()) {
        view->addCompleterItem(command);

        if(command.startsWith(CliAndBreadcrumbsView::CMD_LIST_OUTLINES)) {
            MF_DEBUG("  executing: list outlines" << endl);
            executeListOutlines();
            view->showBreadcrumb();
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_TAG)) {
            string name = command.toStdString().substr(
                CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_TAG.size());
            MF_DEBUG("  executing: find O by tag '" << name << "'" << endl);
            if(name.size()) {
                mainPresenter->doActionFindOutlineByTag(name);
            }
            mainPresenter->getStatusBar()->showInfo(tr("Notebook not found - please specify tag search phrase (is empty)"));
            return;
        } else if(command.startsWith(CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME)) {
            string name = command.toStdString().substr(
                CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME.size());
            MF_DEBUG("  executing: find O by name '" << name << "'" << endl);
            if(name.size()) {
                mainPresenter->doActionFindOutlineByName(name);
            }
            mainPresenter->getStatusBar()->showInfo(tr("Notebook not found - please specify name search phrase (is empty)"));
            // status handling examples:
            // mainPresenter->getStatusBar()->showInfo(tr("Notebook ")+QString::fromStdString(outlines->front()->getName()));
            // mainPresenter->getStatusBar()->showInfo(tr("Notebook not found: ") += QString(name.c_str()));
            return;
        } else {
            // do FTS as fallback
            mainPresenter->doFts(view->getCommand(), true);
        }
    } else {
        mainPresenter->getStatusBar()->showError(tr("No command!"));
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

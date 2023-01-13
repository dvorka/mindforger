/*
 fts_dialog_presenter.cpp     MindForger thinking notebook

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
#include "fts_dialog_presenter.h"

namespace m8r {

using namespace std;

FtsDialogPresenter::FtsDialogPresenter(FtsDialog* view, Mind* mind, OrlojPresenter* orloj)
    : view{view},
      mind{mind},
      orloj{orloj}
{
    QObject::connect(
        view->getSearchButton(), SIGNAL(clicked()),
        this, SLOT(slotSearch()));
    QObject::connect(
        view->getResultListingView()->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowMatchingNotePreview(const QItemSelection&, const QItemSelection&)));
    QObject::connect(
        view->getOpenButton(), SIGNAL(clicked()),
        orloj->getMainPresenter(), SLOT(slotHandleFts()));
    QObject::connect(
        view->getResultListingView(), SIGNAL(signalShowSelectedNote()),
        orloj->getMainPresenter(), SLOT(slotHandleFts()));
    QObject::connect(
        view->getResultListingView(), SIGNAL(signalEscape()),
        this, SLOT(slotHideDialog()));
    QObject::connect(
        view, SIGNAL(signalNoteScopeSearch()),
        orloj->getMainPresenter(), SLOT(slotHandleFts()));
}

FtsDialogPresenter::~FtsDialogPresenter()
{
}

void FtsDialogPresenter::doSearch()
{
    slotSearch();
}

void FtsDialogPresenter::slotSearch()
{
    doFts(
        view->getSearchPattern().toStdString(),
        view->isExact()?FtsSearch::EXACT:(view->isRegex()?FtsSearch::REGEXP:FtsSearch::IGNORE_CASE),
        view->getScope());
}

QString &FtsDialogPresenter::getNoteWithMatchesAsHtml(const Note* note)
{
    // FTS result HTML
    string html{"<html><body style='"};
    orloj->getMainPresenter()->getHtmlRepresentation()->fgBgTextColorStyle(html);
    html += "'><pre>";
    qHtml = QString::fromStdString(html);
    orloj->getMainPresenter()->getMarkdownRepresentation()->to(note, &html);
    qHtml += QString::fromStdString(html);
    qHtml += QString::fromStdString("</pre></body></html>");

    // highlight matches
    QString highlighted = QString::fromStdString("<span style='background-color: red; color: white;'>");
    // IMPROVE instead of searched expression that MAY differ in CASE, here should be original string found in the haystack
    highlighted += view->getSearchPattern();
    highlighted += QString::fromStdString("</span>");
    qHtml.replace(
         view->getSearchPattern(),
         highlighted,
         view->isCaseInsensitive()?Qt::CaseInsensitive:Qt::CaseSensitive);

    return qHtml;
}

void FtsDialogPresenter::doFts(
        const string& pattern,
        const FtsSearch searchMode,
        Outline* scope) const
{
    vector<Note*>* result = mind->findNoteFts(pattern, searchMode, scope);

    QString info = QString::number(result->size());
    info += QString::fromUtf8(" result(s) found for '");
    info += QString::fromStdString(pattern);
    info += QString::fromUtf8("'");
    orloj->getMainPresenter()->getView().getStatusBar()->showInfo(info);

    if(result && result->size()) {
        // show in view
        view->refreshResult(result);
    } else {
        view->hideResult();
        view->getResultListingPresenter()->getModel()->removeAllRows();
        QMessageBox::information(view, tr("Full-text Search Result"), tr("No matching Notebook or Note found."));
    }

    view->searchAndAddPatternToHistory();
}

void FtsDialogPresenter::slotShowMatchingNotePreview(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indices = selected.indexes();
    if(indices.size()) {
        const QModelIndex& index = indices.at(0);
        QStandardItem* item = view->getResultListingPresenter()->getModel()->itemFromIndex(index);
        // TODO make my role constant
        selectedNote = item->data(Qt::UserRole + 1).value<Note*>();

        view->getOpenButton()->setEnabled(true);

        getNoteWithMatchesAsHtml(selectedNote);
        view->getResultPreview()->setHtml(qHtml);
        view->getResultPreview()->find(view->getSearchPattern());
    } else {
        selectedNote = nullptr;
        orloj->getMainPresenter()->getStatusBar()->showInfo(QString(tr("No Notebook selected!")));
        view->getOpenButton()->setEnabled(false);
    }
}

} // m8r namespace

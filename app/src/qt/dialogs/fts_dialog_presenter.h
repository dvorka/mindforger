/*
 fts_dialog_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_FTS_DIALOG_PRESENTER_H
#define M8RUI_FTS_DIALOG_PRESENTER_H

#include <vector>

#include <QtWidgets>

#include "../../lib/src/mind/mind.h"
#include "../orloj_presenter.h"
#include "fts_dialog.h"

namespace m8r {

class FtsDialogPresenter : public QObject
{
    Q_OBJECT

private:
    FtsDialog* view;

    Mind* mind;
    OrlojPresenter* orloj;

    Note* selectedNote;
    QString qHtml;

public:
    explicit FtsDialogPresenter(FtsDialog* view, Mind* mind, OrlojPresenter* orloj);
    FtsDialogPresenter(const FtsDialogPresenter&) = delete;
    FtsDialogPresenter(const FtsDialogPresenter&&) = delete;
    FtsDialogPresenter &operator=(const FtsDialogPresenter&) = delete;
    FtsDialogPresenter &operator=(const FtsDialogPresenter&&) = delete;
    ~FtsDialogPresenter();

    FtsDialog* getView() const { return view; }
    Note* getSelectedNote() const { return selectedNote; }

    void doSearch();

private:
    QString &getNoteWithMatchesAsHtml(const Note* note);
    void doFts(const std::string& pattern, const FtsSearch searchMode, Outline* scope) const;

private slots:
    void slotSearch();
    void slotShowMatchingNotePreview(const QItemSelection& selected, const QItemSelection& deselected);
    void slotHideDialog() {
        view->hide();
    }
};

}
#endif // M8RUI_FTS_DIALOG_PRESENTER_H

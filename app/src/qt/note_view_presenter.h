/*
 note_view_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_NOTE_VIEW_PRESENTER_H
#define M8RUI_NOTE_VIEW_PRESENTER_H

#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

#include "orloj_presenter.h"
#include "note_view.h"
#include "note_view_model.h"

#if !defined(__APPLE__) && !defined(_WIN32) && !defined(MF_QT_WEB_ENGINE)
  #include <QWebFrame>
#endif

namespace m8r {

class OrlojPresenter;

class NoteViewPresenter : public QObject
{
    Q_OBJECT

private:
    std::string html;

    Configuration& config;
    Mind* mind;

    OrlojPresenter* orloj;

    NoteView* view;
    NoteViewModel* model;

    MarkdownOutlineRepresentation* markdownRepresentation;
    HtmlOutlineRepresentation* htmlRepresentation;

    Note* currentNote;

    // search expression may be a string or regexp
    QString searchExpression;
    bool searchIgnoreCase;

public:
    NoteViewPresenter(NoteView* view, OrlojPresenter* orloj);
    NoteViewPresenter(const NoteViewPresenter&) = delete;
    NoteViewPresenter(const NoteViewPresenter&&) = delete;
    NoteViewPresenter &operator=(const NoteViewPresenter&) = delete;
    NoteViewPresenter &operator=(const NoteViewPresenter&&) = delete;
    ~NoteViewPresenter();

    NoteViewModel* getModel() const { return model; }
    NoteView* getView() const { return view; }
    Note* getCurrentNote() { return currentNote; }

    void refreshLivePreview();
    void refresh(Note* note);

    void clearSearchExpression() { searchExpression.clear(); }
    void setSearchPattern(const std::string& expression) { searchExpression = QString::fromStdString(expression); }
    void setSearchPattern(const QString& expression) { searchExpression = expression; }
    const QString& getFtsExpression() const { return searchExpression; }
    void setSearchIgnoreCase(bool ignoreCase) { searchIgnoreCase = ignoreCase; }

public slots:
    void slotLinkClicked(const QUrl& url);
    void slotEditNote();
    void slotEditNoteDoubleClick();
    void slotRefreshLeaderboardByValue(AssociatedNotes* associations);
};

} // m8r namespace

#endif // M8RUI_NOTE_VIEW_PRESENTER_H

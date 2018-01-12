/*
 note_view_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTE_VIEW_PRESENTER_H
#define M8RUI_NOTE_VIEW_PRESENTER_H

#include "lib/src/mind/mind.h"
#include "lib/src/representations/markdown/markdown_outline_representation.h"
#include "lib/src/representations/html/html_outline_representation.h"

#include <QtWidgets>

#include "orloj_presenter.h"
#include "note_view.h"
#include "note_view_model.h"

namespace m8r {

class OrlojPresenter;

class NoteViewPresenter : public QObject
{
    Q_OBJECT

private:
    OrlojPresenter* orloj;

    NoteView* view;
    NoteViewModel* model;

    MarkdownOutlineRepresentation* markdownRepresentation;
    HtmlOutlineRepresentation* htmlRepresentation;

    Note* currentNote;

    QString ftsExpression;
    bool ftsIgnoreCase;

public:
    NoteViewPresenter(NoteView* view, OrlojPresenter* orloj);
    NoteViewPresenter(const NoteViewPresenter&) = delete;
    NoteViewPresenter(const NoteViewPresenter&&) = delete;
    NoteViewPresenter &operator=(const NoteViewPresenter&) = delete;
    NoteViewPresenter &operator=(const NoteViewPresenter&&) = delete;
    ~NoteViewPresenter();

    NoteViewModel* getModel(void) const { return model; }

    void refresh(Note* note);

    void clearFtsExpression(void) { ftsExpression.clear(); }
    void setFtsExpression(const std::string& expression) { ftsExpression = QString::fromStdString(expression); }
    void setFtsExpression(const QString& expression) { ftsExpression = expression; }
    const QString& getFtsExpression(void) const { return ftsExpression; }
    void setFtsIgnoreCase(bool ignoreCase) { ftsIgnoreCase = ignoreCase; }

public slots:
    void slotEditNote(void);
};

}

#endif // M8RUI_NOTE_VIEW_PRESENTER_H

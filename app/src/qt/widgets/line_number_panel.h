/*
 line_number_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_LINE_NUMBER_PANEL_H
#define M8RUI_LINE_NUMBER_PANEL_H

#include <QtWidgets>

#include "../note_editor_view.h"

namespace m8r {

class NoteEditorView;

class LineNumberPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LineNumberPanel(NoteEditorView* editor);
    LineNumberPanel(const LineNumberPanel&) = delete;
    LineNumberPanel(const LineNumberPanel&&) = delete;
    LineNumberPanel &operator=(const LineNumberPanel&) = delete;
    LineNumberPanel &operator=(const LineNumberPanel&&) = delete;
    virtual ~LineNumberPanel() override {}

    QSize sizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    NoteEditorView* mdEditor;
};

}
#endif // M8RUI_LINE_NUMBER_PANEL_H

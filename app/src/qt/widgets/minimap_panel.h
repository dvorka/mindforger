/*
 minimap_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_MINIMAP_PANEL_H
#define M8RUI_MINIMAP_PANEL_H

#include <QtWidgets>

#include "../../../../lib/src/gear/minimap_geometry.h"

namespace m8r {

class NoteEditorView;

/**
 * @brief Editor minimap.
 *
 * Narrow strip on the right side of the editor which shows the overview
 * of the edited text - every line is painted as tiny bars in the syntax
 * highlighting colors. The part of the text visible in the editor is shown
 * as a rectangle. Click jumps to the text, drag scrolls the editor.
 */
class MinimapPanel : public QWidget
{
    Q_OBJECT

public:
    static const int WIDTH = 80;

    explicit MinimapPanel(NoteEditorView* editor);
    MinimapPanel(const MinimapPanel&) = delete;
    MinimapPanel(const MinimapPanel&&) = delete;
    MinimapPanel &operator=(const MinimapPanel&) = delete;
    MinimapPanel &operator=(const MinimapPanel&&) = delete;
    virtual ~MinimapPanel() override {}

    QSize sizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

private:
    NoteEditorView* mdEditor;

    /**
     * @brief Distance between the mouse and the viewport top when dragged.
     */
    int dragOffset;

    MinimapGeometry createGeometry() const;
    void scrollEditorTo(int viewportTop);
    void paintLine(
        QPainter& painter,
        const QTextBlock& block,
        const QTextLine& line,
        int y,
        const QColor& textColor);
};

}
#endif // M8RUI_MINIMAP_PANEL_H

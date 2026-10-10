/*
 minimap_panel.cpp     MindForger thinking notebook

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
#include "minimap_panel.h"

#include "../note_editor_view.h"
#include "../look_n_feel.h"

namespace m8r {

const int MinimapPanel::WIDTH;

// minimap shows text as 1px per character columns
constexpr const int MINIMAP_TAB_COLUMNS = 4;
constexpr const int MINIMAP_PADDING = 4;

MinimapPanel::MinimapPanel(NoteEditorView* editor)
    : QWidget(editor),
      mdEditor{editor},
      dragOffset{0}
{
    setCursor(Qt::ArrowCursor);
}

QSize MinimapPanel::sizeHint() const
{
    return QSize{WIDTH, 0};
}

MinimapGeometry MinimapPanel::createGeometry() const
{
    // editor scrollbar counts (visual) lines: value is the first visible line
    const QScrollBar* scrollBar = mdEditor->verticalScrollBar();
    const int visibleLines = scrollBar->pageStep();

    MinimapGeometry geometry{};
    geometry.update(
        scrollBar->maximum() + visibleLines,
        visibleLines,
        scrollBar->value(),
        height());
    return geometry;
}

void MinimapPanel::scrollEditorTo(int viewportTop)
{
    mdEditor->verticalScrollBar()->setValue(
        createGeometry().getFirstVisibleLineFor(viewportTop));
}

void MinimapPanel::paintLine(
    QPainter& painter,
    const QTextBlock& block,
    const QTextLine& line,
    int y,
    const QColor& textColor)
{
    const int lineHeight = MinimapGeometry::DEFAULT_LINE_HEIGHT;
    const int maxX = width() - MINIMAP_PADDING;

    const QString text = block.text();
    const int start = line.isValid() ? line.textStart() : 0;
    // every character takes at least 1px - characters beyond the strip are not resolved
    const int end = qMin(
        qMin(text.size(), line.isValid() ? start + line.textLength() : text.size()),
        start + qMax(0, maxX - MINIMAP_PADDING));
    if(end <= start) {
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    const QVector<QTextLayout::FormatRange> formats = block.layout()->formats();
#else
    const QList<QTextLayout::FormatRange> formats = block.layout()->additionalFormats();
#endif

    // resolve colors of the painted span once (not per character) - the last
    // syntax highlighter format w/ the foreground wins
    QVector<QColor> colors(end - start, textColor);
    for(const QTextLayout::FormatRange& range:formats) {
        if(!range.format.hasProperty(QTextFormat::ForegroundBrush)) {
            continue;
        }
        const QColor rangeColor = range.format.foreground().color();
        const int from = qMax(start, range.start);
        const int to = qMin(end, range.start + range.length);
        for(int i = from; i < to; ++i) {
            colors[i - start] = rangeColor;
        }
    }

    int x = MINIMAP_PADDING;
    for(int i = start; i < end && x < maxX; ++i) {
        const QChar c = text.at(i);
        if(c == QLatin1Char{'\t'}) {
            x += MINIMAP_TAB_COLUMNS;
            continue;
        }
        if(c.isSpace()) {
            ++x;
            continue;
        }

        QColor color{colors[i - start]};
        color.setAlphaF(0.6);
        painter.fillRect(x, y, 1, lineHeight, color);
        ++x;
    }
}

void MinimapPanel::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if(!LookAndFeels::getInstance().isThemeNative()) {
        painter.fillRect(event->rect(), LookAndFeels::getInstance().getEditorLineNumbersBackgroundColor());
    }

    const MinimapGeometry geometry = createGeometry();
    const int lineHeight = geometry.getLineHeight();
    const QColor textColor = mdEditor->palette().color(QPalette::Text);
    QColor highlight = mdEditor->palette().color(QPalette::Highlight);
    const int currentBlock = mdEditor->textCursor().blockNumber();

    // paint only the lines which fit the strip - cost doesn't grow w/ the text length
    const int firstLine = geometry.getFirstLine();
    QTextBlock block = mdEditor->document()->findBlockByLineNumber(firstLine);
    int lineInBlock = block.isValid() ? firstLine - block.firstLineNumber() : 0;
    int y = 0;
    while(block.isValid() && y < height()) {
        if(block.isVisible()) {
            // blocks which were not laid out yet are painted as a single line
            const int lines = qMax(1, block.lineCount());
            QTextLayout* layout = block.layout();
            if(block.blockNumber() == currentBlock) {
                highlight.setAlphaF(0.25);
                painter.fillRect(0, y, width(), (lines - lineInBlock) * lineHeight, highlight);
            }
            for(int l = lineInBlock; l < lines && y < height(); ++l) {
                const QTextLine line = layout && l < layout->lineCount() ? layout->lineAt(l) : QTextLine{};
                if(line.isValid() || l == 0) {
                    paintLine(painter, block, line, y, textColor);
                }
                y += lineHeight;
            }
        }
        lineInBlock = 0;
        block = block.next();
    }

    // viewport
    highlight.setAlphaF(0.2);
    QColor border{highlight};
    border.setAlphaF(0.6);
    painter.setPen(border);
    painter.setBrush(highlight);
    painter.drawRect(0, geometry.getViewportTop(), width() - 1, qMax(1, geometry.getViewportHeight() - 1));
}

void MinimapPanel::mousePressEvent(QMouseEvent* event)
{
    if(event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    const MinimapGeometry geometry = createGeometry();
    const int top = geometry.getViewportTop();
    const int y = event->pos().y();
    if(y >= top && y < top + geometry.getViewportHeight()) {
        // grab the viewport rectangle where it was clicked
        dragOffset = y - top;
    } else {
        // jump so that the clicked line is in the middle of the viewport
        dragOffset = geometry.getViewportHeight() / 2;
        scrollEditorTo(y - dragOffset);
    }
    event->accept();
}

void MinimapPanel::mouseMoveEvent(QMouseEvent* event)
{
    if(event->buttons() & Qt::LeftButton) {
        scrollEditorTo(event->pos().y() - dragOffset);
        event->accept();
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void MinimapPanel::wheelEvent(QWheelEvent* event)
{
    // scroll the editor as if the wheel was used in the text
    QCoreApplication::sendEvent(mdEditor->verticalScrollBar(), event);
}

} // m8r namespace

/*
 minimap_geometry.h     MindForger thinking notebook

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
#ifndef M8R_MINIMAP_GEOMETRY_H
#define M8R_MINIMAP_GEOMETRY_H

namespace m8r {

/**
 * @brief Editor minimap geometry.
 *
 * Minimap is a strip which shows every (visual) line of the edited text
 * as a fixed number of pixels high row. If the text does not fit the strip,
 * then the strip scrolls proportionally w/ the editor - the first line of
 * the text is shown when the editor is scrolled to the top and the last line
 * when it is scrolled to the bottom. Viewport is the part of the text which
 * is visible in the editor - it is shown as a rectangle in the strip.
 *
 * All lines are 0-based, all coordinates are in pixels relative to the strip.
 */
class MinimapGeometry
{
public:
    static const int DEFAULT_LINE_HEIGHT = 2;

    explicit MinimapGeometry(int lineHeightPx=DEFAULT_LINE_HEIGHT);
    MinimapGeometry(const MinimapGeometry&) = default;
    MinimapGeometry& operator=(const MinimapGeometry&) = default;
    ~MinimapGeometry();

    /**
     * @brief Set the editor and strip dimensions - invalid values are clamped.
     *
     * @param totalLines        number of lines of the text.
     * @param visibleLines      number of lines visible in the editor.
     * @param firstVisibleLine  first line visible in the editor.
     * @param stripHeightPx     height of the minimap strip.
     */
    void update(int totalLines, int visibleLines, int firstVisibleLine, int stripHeightPx);

    int getLineHeight() const { return lineHeightPx; }
    int getTotalLines() const { return totalLines; }

    /**
     * @brief Get the number of lines which can be shown in the strip.
     */
    int getStripLines() const;

    /**
     * @brief Get the first line of the text which is shown in the strip.
     */
    int getFirstLine() const;

    int getViewportTop() const;
    int getViewportHeight() const;

    /**
     * @brief Get the line of the text shown at the given y coordinate.
     *
     * @return line clamped to the text, 0 if there are no lines.
     */
    int getLineAt(int y) const;

    /**
     * @brief Get the first visible editor line for which the viewport
     * rectangle starts at the given y coordinate (inverse of getViewportTop()).
     *
     * @return line clamped so that the editor does not scroll out of the text.
     */
    int getFirstVisibleLineFor(int viewportTopPx) const;

private:
    int lineHeightPx;
    int totalLines;
    int visibleLines;
    int firstVisibleLine;
    int stripHeightPx;

    /**
     * @brief Get the max first visible line i.e. editor scrolled to the bottom.
     */
    int getMaxFirstVisibleLine() const;
};

}
#endif // M8R_MINIMAP_GEOMETRY_H

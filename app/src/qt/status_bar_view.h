/*
 status_bar_view.cpp     MindForger thinking notebook

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
#ifndef M8RUI_STATUS_BAR_VIEW_H
#define M8RUI_STATUS_BAR_VIEW_H

#include <QtWidgets>

#include "look_n_feel.h"

namespace m8r {

/**
 * @brief Status indicators ("diodes") shown in the lower right corner.
 *
 * Indicators are shown in the order of this enum, which is alphabetical
 * by indicator letter.
 */
enum StatusBarIndicator {
    INDICATOR_AUTOLINKING,      // A
    INDICATOR_DIAGRAMS,         // D
    INDICATOR_LIVE_SPELL_CHECK, // L
    INDICATOR_MATH,             // M
    INDICATOR_SRC_HIGHLIGHT,    // S
    INDICATOR_THINKING,         // T
    INDICATOR_WINGMAN,          // W

    INDICATOR_COUNT
};

class StatusBarView
{
private:
    static const char* INDICATORS_PANEL_OBJECT_NAME;
    static const char* INDICATOR_OBJECT_NAME;

    QStatusBar* statusBar;

    // indicators are shown as segments of one capsule shaped panel
    QFrame* indicatorsPanel;
    QLabel* indicators[INDICATOR_COUNT];
    // indicator style sheets derived from the active theme: [indicator][enabled]
    QString indicatorStyles[INDICATOR_COUNT][2];

    LookAndFeels& lookAndFeel;
    QFont font;

public:
    StatusBarView(QStatusBar* qStatusBar, LookAndFeels& lookAndFeel);
    StatusBarView(const StatusBarView&) = delete;
    StatusBarView(const StatusBarView&&) = delete;
    StatusBarView &operator=(const StatusBarView&) = delete;
    StatusBarView &operator=(const StatusBarView&&) = delete;
    virtual ~StatusBarView();

    void setLookAndFeel(LookAndFeels& lookAndFeel);

    void showInfo(const QString& message) const;
    void showWarning(const QString& message) const;
    void showError(const QString& message) const;

    void showIndicator(StatusBarIndicator indicator, bool enabled) const;

private:
    static QString createIndicatorStyle(
        int indicator,
        const QColor& textColor,
        const QColor& backgroundColor,
        const QColor& separatorColor);
    static QString toCssColor(const QColor& color);
};

}

#endif // M8RUI_STATUS_BAR_VIEW_H

/*
 status_bar_view.cpp     MindForger thinking notebook

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
#ifndef M8RUI_STATUS_BAR_VIEW_H
#define M8RUI_STATUS_BAR_VIEW_H

#include <QtWidgets>

#include "look_n_feel.h"

namespace m8r {

class StatusBarView
{
private:
    QStatusBar* statusBar;

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
};

}

#endif // M8RUI_STATUS_BAR_VIEW_H

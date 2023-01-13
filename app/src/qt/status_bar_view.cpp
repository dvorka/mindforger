/*
 status_bar_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "status_bar_view.h"

namespace m8r {

StatusBarView::StatusBarView(QStatusBar* qStatusBar, LookAndFeels& lookAndFeel)
    : lookAndFeel(lookAndFeel)
{
    this->statusBar = qStatusBar;

    font.setFamily(statusBar->font().family());
    font.setPointSize(lookAndFeel.getFontPointSize());
    statusBar->setFont(font);
}

void StatusBarView::showInfo(const QString& message) const
{
    // IMPROVE set style sheet - code below causes SIGSEGV (lookAndFeel nullptr)
    QString style;
    style += "color: ";
    style += lookAndFeel.getTextColor();
    style += ";";
    statusBar->setStyleSheet(style);

    statusBar->showMessage(message);
}

void StatusBarView::showWarning(const QString& message) const
{
    // IMPROVE set style sheet > ORANGE
    statusBar->showMessage(message);
}

void StatusBarView::showError(const QString& message) const
{
    // IMPROVE set style sheet > RED
    statusBar->showMessage(message);
}

StatusBarView::~StatusBarView()
{
}

} // namespace

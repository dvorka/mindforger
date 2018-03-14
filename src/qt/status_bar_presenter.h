/*
 status_bar_presenter.cpp     MindForger thinking notebook

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
#ifndef M8RUI_STATUS_BAR_PRESENTER_H
#define M8RUI_STATUS_BAR_PRESENTER_H

#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

#include "status_bar_view.h"

namespace m8r {

class StatusBarPresenter : public QObject
{
    Q_OBJECT

private:
    QString status;

    const StatusBarView* view;
    Mind* mind;

public:
    StatusBarPresenter(const StatusBarView* view, Mind* mind);
    StatusBarPresenter(const StatusBarPresenter&) = delete;
    StatusBarPresenter(const StatusBarPresenter&&) = delete;
    StatusBarPresenter &operator=(const StatusBarPresenter&) = delete;
    StatusBarPresenter &operator=(const StatusBarPresenter&&) = delete;

    void showMindStatistics();

    void showInfo(const char* message) { showInfo(QString::fromUtf8(message)); }
    void showInfo(const std::string& message) { showInfo(QString::fromStdString(message)); }
    void showInfo(const QString& message);
    void showWarning(const char* message) { showWarning(QString::fromUtf8(message)); }
    void showWarning(const std::string& message) { showWarning(QString::fromStdString(message)); }
    void showWarning(const QString& message);
    void showError(const char* message) { showError(QString::fromUtf8(message)); }
    void showError(const std::string& message) { showError(QString::fromStdString(message)); }
    void showError(const QString& message);

    const StatusBarView* getView() const { return view; }
};

}
#endif // M8RUI_STATUS_BAR_PRESENTER_H

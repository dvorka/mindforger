/*
 cli_n_breadcrumbs_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_CLI_N_BREADCRUMBS_PRESENTER_H
#define M8RUI_CLI_N_BREADCRUMBS_PRESENTER_H

#include <vector>

#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

#include "cli_n_breadcrumbs_view.h"
#include "main_window_presenter.h"

namespace m8r {

class MainWindowPresenter;
class CliAndBreadcrumbsView;

class CliAndBreadcrumbsPresenter : public QObject
{
    Q_OBJECT

private:
    const MainWindowPresenter* mainPresenter;
    CliAndBreadcrumbsView* view;
    Mind* mind;

public:
    CliAndBreadcrumbsPresenter(
            const MainWindowPresenter* mainPresenter,
            CliAndBreadcrumbsView* view,
            Mind* mind);

    void executeListOutlines();
    void executeListNotes();
    void executeFts(QString& command);

private slots:
    void executeCommand();
    void handleCliTextChanged(const QString& text);
};

}
#endif // M8RUI_CLI_N_BREADCRUMBS_PRESENTER_H

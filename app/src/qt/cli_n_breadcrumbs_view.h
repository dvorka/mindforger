/*
 cli_n_breadcrumbs_view.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_CLI_N_BREADCRUMBS_VIEW_H
#define M8RUI_CLI_N_BREADCRUMBS_VIEW_H

#include <QtWidgets>

#include "look_n_feel.h"
#include "cli_n_breadcrumbs_presenter.h"

namespace m8r {

class CliAndBreadcrumbsView;

class CliView : public QLineEdit
{
    Q_OBJECT

private:
    CliAndBreadcrumbsView* cliAndBreadcrumps;
public:
    explicit CliView(CliAndBreadcrumbsView* cliAndBreadcrumps, QWidget* parent);
    void keyPressEvent(QKeyEvent* event) override;
signals:
    void keyReleased(QKeyEvent* event);
};

class CliAndBreadcrumbsView : public QWidget
{
    Q_OBJECT

    friend class CliAndBreadcrumbsPresenter;
private:
    static const QStringList DEFAULT_CMDS;

    // IMPROVE horizontal container w/ buttons names and / labels to navigate easily up/down
    QLabel* breadcrumbsLabel;
    CliView* cli;
    QCompleter* cliCompleter;
    QStringList cliCompleterHistoryList;

    bool zenMode;

public:
    static const QString CMD_FTS;
    static const QString CMD_FIND_OUTLINE_BY_NAME;
    static const QString CMD_LIST_OUTLINES;

public:
    explicit CliAndBreadcrumbsView(QWidget* parent, bool zenMode=true);

    void addCompleterItem(const QString& item) {
        cliCompleterHistoryList.insert(0, item);
    }
    void updateCompleterModel(const QStringList* list=nullptr);
    void forceFtsHistoryCompletion();
    QString getFirstCompletion() const;
    void setBreadcrumbPath(const QString& path);
    void setCommand(const char* command);
    const QString getCommand() const;
    void show();
    void hide();
    void showBreadcrumb();
    void showCli(bool selectAll=true);
};

}
#endif // M8RUI_CLI_N_BREADCRUMBS_VIEW_H

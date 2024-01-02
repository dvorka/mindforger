/*
 cli_n_breadcrumbs_view.h     MindForger thinking notebook

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
#ifndef M8RUI_CLI_N_BREADCRUMBS_VIEW_H
#define M8RUI_CLI_N_BREADCRUMBS_VIEW_H

#include <QtWidgets>

#include "look_n_feel.h"
#include "cli_n_breadcrumbs_presenter.h"

namespace m8r {

class CliAndBreadcrumbsView;

/**
 * @brief Custom line edit with key and color handling.
 */
class CliView : public QLineEdit
{
    Q_OBJECT

private:
    CliAndBreadcrumbsView* cliAndBreadcrumps;

protected:
    void focusOutEvent(QFocusEvent*) override;

public:
    QPalette PALETTE_DISABLED_TEXT;
    QPalette PALETTE_ENABLED_TEXT;
    QPalette PALETTE_ERROR_TEXT;

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
    // IMPROVE horizontal container w/ buttons names and / labels to navigate easily up/down
    QLabel* breadcrumbsLabel;
    CliView* cli;
    QCompleter* cliCompleter;

    QStringList cliCompleterHistoryList;

    bool zenMode;

public:
    static const QStringList DEFAULT_CMDS; // TODO remove

    static const QStringList EMPTY_CMDS;

    // help

    static const QString CHAR_HELP;

    static const QString CMD_HELP_HELP;
    static const QString CMD_HELP_SEARCH;
    static const QString CMD_HELP_KNOWLEDGE;
    static const QString CMD_HELP_CMD;
    static const QString CMD_HELP_CHAT;
    static const QString CMD_HELP_FTS;

    static const QStringList HELP_CMDS;

    // search

    static const QString CHAR_FIND;

    static const QString CMD_FIND_OUTLINE_BY_NAME;
    static const QString CMD_FIND_OUTLINE_BY_TAG;
    static const QString CMD_FIND_NOTE_BY_NAME;
    static const QString CMD_FIND_NOTE_BY_TAG;

    static const QStringList HELP_FIND_CMDS;

    // knowledge

    static const QString CHAR_KNOW;

    static const QString CMD_KNOW_WIKIPEDIA;
    static const QString CMD_KNOW_ARXIV;
    static const QString CMD_KNOW_STACK_OVERFLOW;
    static const QString CMD_KNOW_DUCK;
    static const QString CMD_KNOW_GITHUB;
    static const QString CMD_KNOW_BARD;

    static const QStringList HELP_KNOW_CMDS;

    // command

    static const QString CHAR_CMD;

    static const QString CMD_HOME;
    static const QString CMD_TERMINAL;
    static const QString CMD_LIST_OUTLINES;

    static const QStringList HELP_CMD_CMDS;

    // chat

    static const QString CHAR_CHAT;

public:
    explicit CliAndBreadcrumbsView(QWidget* parent, bool zenMode=true);

    /**
     * @brief Keyboard shortcut handler (Alt-x).
     */
    void showCli(bool selectAll=true);

    void addCompleterItem(const QString& item) {
        cliCompleterHistoryList.insert(0, item);
    }
    void updateCompleterModel(const QStringList& helpList, const QStringList* list=nullptr);
    void forceFtsHistoryCompletion();
    QString getFirstCompletion() const;
    void setBreadcrumbPath(const QString& path);
    void setCommand(const char* command);
    const QString getCommand() const;
    void show();
    void complete() { cliCompleter->complete(); }
    void hide();
    void showBreadcrumb();
};

}
#endif // M8RUI_CLI_N_BREADCRUMBS_VIEW_H

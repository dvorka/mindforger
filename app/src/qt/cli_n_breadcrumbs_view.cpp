/*
 cli_n_breadcrumbs_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "cli_n_breadcrumbs_view.h"

namespace m8r {

// "Enter a prompt - \"? .\" for help, \"> .\" to chat (Alt-x), \"/ .\" run command (Ctrl-/), or type a phrase to find."
constexpr const auto CLI_HELP_SHADOW_TEXT
    = "Enter a prompt, command or phrase - type ? for help.";


CliView::CliView(CliAndBreadcrumbsView* cliAndBreadcrumps, QWidget* parent)
    : QLineEdit(parent),
      PALETTE_DISABLED_TEXT(this->palette()),
      PALETTE_ENABLED_TEXT(this->palette()),
      PALETTE_ERROR_TEXT(this->palette())
{
    this->cliAndBreadcrumps = cliAndBreadcrumps;

    PALETTE_DISABLED_TEXT.setColor(
        QPalette::Text,
        QColor(125, 125, 125));

    PALETTE_ENABLED_TEXT.setColor(
        QPalette::Text,
        LookAndFeels::getInstance().getCliTextColor());

    PALETTE_ERROR_TEXT.setColor(
        QPalette::Text,
        QColor(125, 0, 0));

#if !defined(__APPLE__)
    // changing pallette @ macOS w/ dark model @ Qt 5.15.x+ causes edit line to be unreadable
    setPalette(PALETTE_ENABLED_TEXT);
#endif

    setToolTip(CLI_HELP_SHADOW_TEXT);
}

void CliView::keyPressEvent(QKeyEvent* event)
{
    switch(event->key()) {
    case Qt::Key_Escape:
        cliAndBreadcrumps->showBreadcrumb();
        break;
    default:
        break;
    }

    QLineEdit::keyPressEvent(event);
}

void CliView::focusInEvent(QFocusEvent* event)
{
    MF_DEBUG("CLI: on focus acquired" << std::endl);

    if(text() == CLI_HELP_SHADOW_TEXT) {
        clear();
    }

    QLineEdit::focusInEvent(event);
}

void CliView::focusOutEvent(QFocusEvent* event)
{
    MF_DEBUG("CLI: on focus lost" << std::endl);

    if(text().size() == 0) {
        setText(CLI_HELP_SHADOW_TEXT);
    }

    QLineEdit::focusOutEvent(event);
}

/*
 * CLI and breadcrumbs view.
 */

const QStringList CliAndBreadcrumbsView::EMPTY_CMDS = QStringList();

// help

const QString CliAndBreadcrumbsView::CHAR_HELP
    = "?";

const QString CliAndBreadcrumbsView::CMD_HELP_HELP
    = "? ... ? for help";
const QString CliAndBreadcrumbsView::CMD_HELP_SEARCH
    = "?     / to search";
const QString CliAndBreadcrumbsView::CMD_HELP_KNOWLEDGE
    = "?     @ for knowledge recherche";
const QString CliAndBreadcrumbsView::CMD_HELP_CMD
    = "?     > to run a command";
const QString CliAndBreadcrumbsView::CMD_HELP_CHAT
    = "?     : to chat with workspace, Notebook or Note";
const QString CliAndBreadcrumbsView::CMD_HELP_FTS
    = "?     full-text search phrase";

const QStringList CliAndBreadcrumbsView::HELP_CMDS = QStringList()
        << CMD_HELP_HELP
        << CMD_HELP_SEARCH
 //       << CMD_HELP_KNOWLEDGE
        << CMD_HELP_CMD
 //       << CMD_HELP_CHAT
 //       << CMD_HELP_FTS
        ;

const QString CliAndBreadcrumbsView::CHAR_FIND
    = "/";

const QString CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME
    = "/ find notebook by name ";
const QString CliAndBreadcrumbsView::CMD_FIND_NOTE_BY_NAME
    = "/ find note by name ";
const QString CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_TAG
    = "/ find notebook by tag ";
const QString CliAndBreadcrumbsView::CMD_FIND_NOTE_BY_TAG
    = "/ find note by tag ";

const QStringList CliAndBreadcrumbsView::HELP_FIND_CMDS = QStringList()
        << CMD_FIND_OUTLINE_BY_NAME
        << CMD_FIND_OUTLINE_BY_TAG
//        << CMD_FIND_NOTE_BY_NAME
//        << CMD_FIND_NOTE_BY_TAG
        ;

const QString CliAndBreadcrumbsView::CHAR_KNOW
    = "@";

// knowledge recherche uses current context / selected entity to lookup the knowledge
const QString CliAndBreadcrumbsView::CMD_KNOW_WIKIPEDIA
    = "@wikipedia";
const QString CliAndBreadcrumbsView::CMD_KNOW_ARXIV
    = "@arxiv";
const QString CliAndBreadcrumbsView::CMD_KNOW_STACK_OVERFLOW
    = "@stackoverflow";
const QString CliAndBreadcrumbsView::CMD_KNOW_DUCK
    = "@duckduckgo";
const QString CliAndBreadcrumbsView::CMD_KNOW_GITHUB
    = "@github";
const QString CliAndBreadcrumbsView::CMD_KNOW_BARD
    = "@bard";

const QStringList CliAndBreadcrumbsView::HELP_KNOW_CMDS = QStringList()
//    << CMD_KNOW_WIKIPEDIA
//    << CMD_KNOW_ARXIV
    ;

const QString CliAndBreadcrumbsView::CHAR_CMD
    = ">";

const QString CliAndBreadcrumbsView::CMD_HOME
    = "> home"; // go to home O
const QString CliAndBreadcrumbsView::CMD_EMOJIS
    = "> emojis";
const QString CliAndBreadcrumbsView::CMD_TERMINAL
    = "> terminal";
const QString CliAndBreadcrumbsView::CMD_LIST_OUTLINES
    = "> list notebooks";

const QStringList CliAndBreadcrumbsView::HELP_CMD_CMDS = QStringList()
//        << CMD_HOME
//        << CMD_TERMINAL
        << CMD_EMOJIS
        << CMD_LIST_OUTLINES
        ;

const QString CliAndBreadcrumbsView::CHAR_CHAT
    = ":";

// TODO migrate all commands to constants
const QStringList CliAndBreadcrumbsView::DEFAULT_CMDS = QStringList()
        /*
        << CMD_EXIT
        // home tools
        //<< "home"
        //<< "navigator"
        // outlines
        << "list outlines"
        << CMD_FIND_OUTLINE_BY_NAME
        << "show outline" // "abc"
        << "edit outline" // "abc"
        // << "[outline "abc"] list associations
        // notes
        << "list notes"
        << "find note by name " // "def"
        << "show note " // "abc"."def"
        // << "[outline "abc"]  show note "efg"
        // << "edit note "abc"."efg"
        // << "[outline "abc"] list notes
        // << "[note "abc"."efg"] list associations

        << "fts " // "expr"
        // << "[outline "abc"] fts "expr"
        // << "[note "abc"."efg"] fts "expr"

        << "forget 25%";

        // TODO new outline
        // TODO new note
        */
        << CMD_LIST_OUTLINES
        << CMD_FIND_OUTLINE_BY_NAME
        ;


CliAndBreadcrumbsView::CliAndBreadcrumbsView(QWidget* parent, bool zenMode)
    : QWidget(parent),
      zenMode{zenMode}
{
    setFixedHeight(this->fontMetrics().height()*1.5);

    QHBoxLayout* layout = new QHBoxLayout(this);
    // ensure that wont be extra space around member widgets
    layout->setContentsMargins(QMargins(0, 0, 0, 0));
    setLayout(layout);

    breadcrumbsLabel = new QLabel(this);
    breadcrumbsLabel->setText("$");
    if(zenMode) {
        breadcrumbsLabel->hide();
    }
    layout->addWidget(breadcrumbsLabel);

    this->cli = new CliView(this, parent);
    this->cliCompleter = new QCompleter(new QStandardItemModel{}, parent);
    this->cliCompleter->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    this->cliCompleter->setCompletionMode(QCompleter::PopupCompletion);
    this->cli->setCompleter(cliCompleter);
    this->cli->setText(CLI_HELP_SHADOW_TEXT);
    layout->addWidget(cli);

    showBreadcrumb();
}

void CliAndBreadcrumbsView::showCli(bool selectAll)
{
    MF_DEBUG("CLI view: SHOW (select ALL = " << std::boolalpha << selectAll << ")" << std::endl);

    // if help presents, then clear it AND change color
    if(cli->text().startsWith(CLI_HELP_SHADOW_TEXT)) {
        cli->clear();
    }
#if !defined(__APPLE__)
    setPalette(this->cli->PALETTE_ENABLED_TEXT);
#endif

    // show
    show();
    cli->setFocus();
    if(selectAll) {
        cli->selectAll();
    }
}

void appendToStandardModel(const QStringList& list, QStandardItemModel* completerModel) {
    for(const auto& i:list) {
        QStandardItem* item = new QStandardItem(i);

        // TODO icons are not shown on certain platforms (Linux/x86)
        /*
        if(i.startsWith(".")) {
            item->setIcon(QIcon(":/menu-icons/cli.svg"));
        } else {
            item->setIcon(QIcon(":/menu-icons/find.svg"));
        }
        */

        // TODO IMPROVE item->setToolTip("tool tip");
        completerModel->appendRow(item);
    }
}

void CliAndBreadcrumbsView::updateCompleterModel(const QStringList& helpList, const QStringList* list)
{
    QStandardItemModel* completerModel=(QStandardItemModel*)cliCompleter->model();
    if(completerModel==nullptr) {
        completerModel = new QStandardItemModel();
    } else {
        QModelIndex start = completerModel->index(0, 0);
        for(int r=0; r < completerModel->rowCount(start); ++r) {
            QModelIndex idx = completerModel->index(r, 0, start);
            QStandardItem* i = completerModel->itemFromIndex(idx);
            delete i;
        }

        completerModel->clear();
    }

    appendToStandardModel(cliCompleterHistoryList, completerModel);
    if(list!=nullptr) {
        appendToStandardModel(*list, completerModel);
    }
    appendToStandardModel(helpList, completerModel);
}

void CliAndBreadcrumbsView::forceFtsHistoryCompletion()
{
    updateCompleterModel(DEFAULT_CMDS);

    // ensure completion is shown despite there is NO filtering character
    cliCompleter->complete();
}

QString CliAndBreadcrumbsView::getFirstCompletion() const
{
    if(cliCompleter->completionCount()) {
        return cliCompleter->currentCompletion();
    } else {
        return QString();
    }
}

void CliAndBreadcrumbsView::setBreadcrumbPath(const QString& path)
{
    if(zenMode) {
        breadcrumbsLabel->setText("");
    } else {
        breadcrumbsLabel->setText(path);
    }
}

void CliAndBreadcrumbsView::setCommand(const char* command)
{
    cli->setText(command);
}

const QString CliAndBreadcrumbsView::getCommand() const
{
    return cli->text();
}

void CliAndBreadcrumbsView::show()
{
    if(!zenMode) {
        breadcrumbsLabel->show();
    }
    cli->show();
    cliCompleter->complete();
}

void CliAndBreadcrumbsView::hide()
{
    if(!zenMode) {
        breadcrumbsLabel->hide();
        cli->hide();
    }
}

void CliAndBreadcrumbsView::showBreadcrumb()
{
    if(!zenMode) {
        breadcrumbsLabel->show();
        cli->hide();
    }
}

} // m8r namespace

/*
 cli_n_breadcrumbs_view.cpp     MindForger thinking notebook

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
#include "cli_n_breadcrumbs_view.h"

namespace m8r {

CliView::CliView(CliAndBreadcrumbsView* cliAndBreadcrumps, QWidget* parent)
    : QLineEdit(parent)
{
    this->cliAndBreadcrumps = cliAndBreadcrumps;
    QPalette *palette = new QPalette();
    palette->setColor(QPalette::Text, LookAndFeels::getInstance().getCliTextColor());
    setPalette(*palette);
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

const QString CliAndBreadcrumbsView::CMD_HELP = "help";
const QString CliAndBreadcrumbsView::CMD_EXIT = "exit";
const QString CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME = "find outline by name ";

// TODO migrate all commands to constants
const QStringList CliAndBreadcrumbsView::DEFAULT_CMDS = QStringList()
        << CMD_HELP
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


CliAndBreadcrumbsView::CliAndBreadcrumbsView(QWidget* parent)
    : QHBoxLayout(parent)
{    
    breadcrumbsLabel = new QLabel();
    breadcrumbsLabel->setText("$");
    // IMPROVE let UI to build automatically w/o fixed geometry
    breadcrumbsLabel->setFixedHeight(25);
    addWidget(breadcrumbsLabel);

    cli = new CliView(this, parent);
    cliCompleter = new QCompleter(DEFAULT_CMDS, parent);
    cliCompleter->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    cliCompleter->setCompletionMode(QCompleter::PopupCompletion);
    cli->setCompleter(cliCompleter);    
    addWidget(cli);    

    goButton = new QPushButton(tr("Run"));
    addWidget(goButton);

    showBreadcrumb();
}

void CliAndBreadcrumbsView::forceInitialCompletion(void)
{
    QStringListModel* completerModel=(QStringListModel*)cliCompleter->model();
    if(completerModel==nullptr) {
        completerModel = new QStringListModel();
    }
    completerModel->setStringList(DEFAULT_CMDS);
    // ensure completion is shown despite there is NO filtering character
    cliCompleter->complete();
}

QString CliAndBreadcrumbsView::getFirstCompletion(void) const
{
    if(cliCompleter->completionCount()) {
        return cliCompleter->currentCompletion();
    } else {
        return QString::null;
    }
}

void CliAndBreadcrumbsView::updateCompleterModel(const QStringList *list)
{
    QStringListModel* completerModel=(QStringListModel*)cliCompleter->model();
    if(completerModel==nullptr) {
        completerModel = new QStringListModel();
    }
    if(list==nullptr) {
        completerModel->setStringList(DEFAULT_CMDS);
    } else {
        completerModel->setStringList(*list);
    }
}

void CliAndBreadcrumbsView::setBreadcrumbPath(const QString& path)
{
    breadcrumbsLabel->setText(path);
}

}

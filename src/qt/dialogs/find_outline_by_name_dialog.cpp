/*
 find_outline_by_name_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "find_outline_by_name_dialog.h"

namespace m8r {

using namespace std;

FindOutlineByNameDialog::FindOutlineByNameDialog(QWidget *parent)
    : FtsDialog(parent)
{
    label->setText("Outline name:");
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

    // dialog
    setWindowTitle(tr("Find Outline by Name"));
}

void FindOutlineByNameDialog::show(vector<string>* outlineNames)
{
    qDebug() << "NNN " << outlineNames->size();

    if(outlineNames && outlineNames->size()) {
        for(string n:*outlineNames) {
            completerModel << QString::fromStdString(n);
        }
        ((QStringListModel*)completer->model())->setStringList(completerModel);
    }

    FtsDialog::show();
}

FindOutlineByNameDialog::~FindOutlineByNameDialog()
{
}

}

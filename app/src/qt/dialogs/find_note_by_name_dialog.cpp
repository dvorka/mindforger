/*
 find_note_by_name_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "find_note_by_name_dialog.h"

namespace m8r {

using namespace std;

FindNoteByNameDialog::FindNoteByNameDialog(QWidget *parent)
    : FindOutlineByNameDialog(parent)
{
    // widgets
    label->setText(tr("Note &name:"));
    findButton->setText(tr("&Open Note"));

    // dialog
    setWindowTitle(tr("Find Note by Name"));
}

FindNoteByNameDialog::~FindNoteByNameDialog()
{
}

void FindNoteByNameDialog::show(vector<Note*>& notes)
{
    // use parent dialog to handle names + keep shadow vector of (filtered) Notes in here for choice
    // IMPROVE performance - consider pointer instead of string copy
    vector<string> noteNames{};
    if(notes.size()) {
        if(!scope) {
            for(Note* n:notes) {
                // IMPROVE make this Note's method: getScopedName()
                string s{n->getName()};
                s += " (";
                s += n->getOutline()->getName();
                s += ")";
                noteNames.push_back(s);
            }
        } else {
            for(Note* n:notes) {
                noteNames.push_back(n->getName());
            }
        }
    }

    vector<Thing*> es{notes.begin(), notes.end()};
    FindOutlineByNameDialog::show(es, &noteNames, scope!=nullptr);
}

} // m8r namespace

/*
 find_note_by_tag_dialog.cpp     MindForger thinking notebook

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
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "find_note_by_tag_dialog.h"

namespace m8r {

using namespace std;

FindNoteByTagDialog::FindNoteByTagDialog(Ontology& ontology, QWidget *parent)
    : FindOutlineByTagDialog(ontology, parent)
{
    mode = ThingsMode::NOTES;

    // widgets
    outlinesGroup->setTitle(tr("Notes:"));
    editTagsGroup->setTitle(tr("Note tags:"));
    switchOutlineNoteDialogsButton->setText(tr("Find Notebook"));
    findButton->setText(tr("&Open Note"));    

    // dialog
    setWindowTitle(tr("Find Note by Tags"));
}

FindNoteByTagDialog::~FindNoteByTagDialog()
{
}

void FindNoteByTagDialog::show(vector<Note*> notes, vector<const Tag*>* tags)
{
    // use parent dialog to handle names + keep shadow vector of (filtered) Notes in here for choice
    // IMPROVE performance - consider pointer instead of string copy
    vector<string> noteNames{};
    if(notes.size()) {
        if(!scope) {
            for(Note* n:notes) {
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
    FindOutlineByTagDialog::show(es, tags, &noteNames);
}

} // m8r namespace

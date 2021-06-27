/*
 associated_notes.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "associated_notes.h"

namespace m8r {

using namespace std;

AssociatedNotes::AssociatedNotes(ResourceType source, Outline* outline)
    : sourceType(source),
      outline(outline)
{
    associations = new vector<pair<Note*,float>>{};
    note = nullptr;
    word.clear();
}

AssociatedNotes::AssociatedNotes(ResourceType source, Note* note)
    : sourceType(source),
      note(note)
{
    associations = new vector<pair<Note*,float>>{};
    outline = nullptr;
    word.clear();
}

AssociatedNotes::AssociatedNotes(ResourceType source, const string& word, Note* self)
    : sourceType(source),
      note(self),
      word(word)
{
    associations = new vector<pair<Note*,float>>{};
    outline = nullptr;
}

AssociatedNotes::~AssociatedNotes()
{
    delete associations;
}

bool AssociatedNotes::equals(const AssociatedNotes* a) const
{
    if(a) {
        if(sourceType == a->sourceType) {
            switch(sourceType) {
            case OUTLINE:
                if(outline == a->outline) return true;
                break;
            case NOTE:
                if(note == a->note) return true;
                break;
            case WORD:
                if(!word.compare(a->word)) return true;
                break;
            default:
                break;
            }
        }
    }

    return false;
}

} // m8r namespace

/*
 associated_notes.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_ASSOCIATED_NOTES_H
#define M8R_ASSOCIATED_NOTES_H

#include <string>
#include <vector>

#include "../model/resource_types.h"
#include "../model/outline.h"

namespace m8r {

class AssociatedNotes
{
    ResourceType sourceType;

    Outline* outline;
    Note* note;
    std::string word;

    std::vector<std::pair<Note*,float>>* associations;

public:
    explicit AssociatedNotes(ResourceType source, Outline* outline);
    explicit AssociatedNotes(ResourceType source, Note* note);
    explicit AssociatedNotes(ResourceType source, const std::string& word, Note* self=nullptr);
    AssociatedNotes(const AssociatedNotes&) = delete;
    AssociatedNotes(const AssociatedNotes&&) = delete;
    AssociatedNotes &operator=(const AssociatedNotes&) = delete;
    AssociatedNotes &operator=(const AssociatedNotes&&) = delete;
    ~AssociatedNotes();

    ResourceType getSourceType() const { return sourceType; }
    Outline* getOutline() const { return outline; }
    Note* getNote() const { return note; }
    std::string& getWord() { return word; }
    std::vector<std::pair<Note*,float>>* getAssociations() const { return associations; }
    void setAssociations(std::vector<std::pair<Note*,float>>* associations) { this->associations = associations; }

    bool equals(const AssociatedNotes* a) const;
};

}
#endif // M8R_ASSOCIATED_NOTES_H

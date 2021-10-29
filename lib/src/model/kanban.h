/*
 kanban.h     MindForger thinking notebook

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
#ifndef M8R_KANBAN_H
#define M8R_KANBAN_H

#include <string>
#include <vector>

#include "organizer.h"
#include "../gear/datetime_utils.h"
#include "../mind/ontology/thing_class_rel_triple.h"

namespace m8r {

/**
 * @brief Kanban as Outlines and Notes sorter.
 */
class Kanban : public Organizer
{
public:
    static std::string createKanbanKey();

public:
    explicit Kanban(const std::string& name);
    explicit Kanban(const Kanban& k);
    Kanban(const Kanban&&) = delete;
    Kanban& operator=(const Kanban&) = delete;
    Kanban& operator=(const Kanban&&) = delete;
    ~Kanban();
};

}
#endif // M8R_KANBAN_H

/*
 mind_listener.h     MindForger thinking notebook

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
#ifndef M8R_MIND_LISTENER_H
#define M8R_MIND_LISTENER_H

#include "../model/outline.h"
#include "../model/note.h"

namespace m8r {

/**
 * @brief Listener used for subscriptions to mind events.
 */
class MindListener
{
public:
    virtual void forget(Outline* outline) = 0;
    virtual void forget(Note* note) = 0;
};

}
#endif // M8R_MIND_LISTENER_H

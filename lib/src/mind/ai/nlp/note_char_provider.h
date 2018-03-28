/*
 note_char_provider.h     MindForger thinking notebook

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
#ifndef M8R_NOTE_CHAR_PROVIDER_H
#define M8R_NOTE_CHAR_PROVIDER_H

#include <string>

#include "../../../debug.h"
#include "string_char_provider.h"
#include "../../../model/note.h"

namespace m8r {

/**
 * @brief Make N stream of chars.
 *
 * N name and then description is turned to continous char stream.
 */
// IMPROVE consider template
class NoteCharProvider : public CharProvider
{
private:
    Note* n;
    std::string s;
    StringCharProvider* p;


public:
    explicit NoteCharProvider(Note* note, char delimiter = '\n');
    NoteCharProvider(const NoteCharProvider&) = delete;
    NoteCharProvider(const NoteCharProvider&&) = delete;
    NoteCharProvider &operator=(const NoteCharProvider&) = delete;
    NoteCharProvider &operator=(const NoteCharProvider&&) = delete;
    ~NoteCharProvider();

    virtual bool hasNext() { return p->hasNext(); }
    virtual const char& get() { return p->get(); }
    virtual const char& getLookahead() { return p->getLookahead(); }
    virtual const char& next() { return p->next(); }
};

}
#endif // M8R_NOTE_CHAR_PROVIDER_H

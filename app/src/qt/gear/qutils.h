/*
 qutils.h     MindForger thinking notebook

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
#ifndef M8R_QUTILS_H
#define M8R_QUTILS_H

#include "../../lib/src/model/tag.h"
#include "../../lib/src/model/outline.h"
#include "../../lib/src/model/note_type.h"
#include "../../lib/src/gear/string_utils.h"

#include <QtWidgets>

namespace m8r {

void initRandomizer();

// int
QString stringFormatIntAsUs(int i);

// string
bool stringMatchByKeywords(const QString& keywords, const QString& s, bool caseSensitive=true);

// data and time
void timetToQDate(const time_t t, QDate& qdate);
void qdateToTm(const QDate& qdate, struct tm& t);

} // m8r namespace

#endif // M8R_QUTILS_H

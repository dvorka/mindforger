/*
 model_meta_definitions.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_MODEL_META_DEFINITIONS_H
#define M8RUI_MODEL_META_DEFINITIONS_H

#include <QtWidgets>

#include "../../lib/src/model/outline.h"
#include "../../lib/src/model/note.h"
#include "../../lib/src/model/tag.h"
#include "../../lib/src/model/stencil.h"
#include "../../lib/src/model/outline_type.h"
#include "../../lib/src/model/note_type.h"
#include "../../lib/src/model/organizer.h"
#include "../../lib/src/model/kanban.h"

/*
 * Registration of custom Qt types allowing to use them in QVariant and Qt's widget models:
 *
 *  - register metatype
 *  - set it as data of standard item:
 *    QStandardItem.setData(QVariant::fromValue(...))
 *  - get it from data:
 *    widget->itemData().value<...>();
 */

Q_DECLARE_METATYPE(m8r::Outline*)
Q_DECLARE_METATYPE(m8r::OutlineType*)
Q_DECLARE_METATYPE(const m8r::OutlineType*)

Q_DECLARE_METATYPE(m8r::Note*)
Q_DECLARE_METATYPE(const m8r::Note*)
Q_DECLARE_METATYPE(m8r::NoteType*)
Q_DECLARE_METATYPE(const m8r::NoteType*)

Q_DECLARE_METATYPE(m8r::Tag*)
Q_DECLARE_METATYPE(const m8r::Tag*)

Q_DECLARE_METATYPE(m8r::Stencil*)
Q_DECLARE_METATYPE(const m8r::Stencil*)

Q_DECLARE_METATYPE(m8r::Organizer*)
Q_DECLARE_METATYPE(const m8r::Organizer*)

#endif // M8RUI_MODEL_META_DEFINITIONS_H

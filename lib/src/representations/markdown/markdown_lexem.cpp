/*
 markdown_lexem.cpp     MindForger thinking notebook

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
#include "markdown_lexem.h"

namespace m8r {

// IMPROVE constexpr
unsigned short int MarkdownLexem::NO_TEXT = USHRT_MAX;
unsigned short int MarkdownLexem::WHOLE_LINE = USHRT_MAX;

MarkdownLexem::MarkdownLexem(MarkdownLexemType type)
    : type(type), depth(0)
{
    idx = lng = depth = 0;
    off = 0;
}

MarkdownLexem::MarkdownLexem(
        MarkdownLexemType type,
        unsigned int offset,
        unsigned short int index,
        unsigned short int lenght)
{
    this->depth = 0;
    this->type = type;
    this->off = offset;
    this->idx = index;
    this->lng = lenght;
}

MarkdownLexem::MarkdownLexem(MarkdownLexemType type, unsigned short int depth)
    : type(type), depth(depth)
{
    off = NO_TEXT;
    idx = lng = 0;
}

MarkdownLexem::~MarkdownLexem()
{
}

MarkdownLexemType MarkdownLexem::getType() const
{
    return type;
}

void MarkdownLexem::setType(MarkdownLexemType type)
{
    this->type = type;
}

unsigned MarkdownLexem::getDepth() const
{
    return depth;
}

void MarkdownLexem::setIdx(unsigned short int idx)
{
    this->idx = idx;
}

void MarkdownLexem::setLng(unsigned short int lng)
{
    this->lng = lng;
}

void MarkdownLexem::setOff(unsigned int off)
{
    this->off = off;
}

void MarkdownLexem::setDepth(unsigned depth)
{
    this->depth = depth;
}

} // m8r namespace

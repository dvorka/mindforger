/*
 markdown_tokenizer.cpp     MindForger thinking notebook

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
#include "markdown_tokenizer.h"

namespace m8r {

MarkdownTokenizer::MarkdownTokenizer()
{
}

MarkdownTokenizer::~MarkdownTokenizer()
{
}

// https://www.ibm.com/developerworks/community/blogs/nlp/entry/tokenization?lang=en
// :-)
// write own TOKENIZER(character provider, separators "xyz", blacklist, vector<string::words>)
// character provider: it's a class that can process string, description, note, ... and getChar() will give a stream of chars
// tokenizer will work like PIPELINE - it will filter out certain characters (*[]\/), it will detect begin/end of tokens
//   perhaps filter out all non-alpha with certain exceptions like - inside
// first pass will be character based
// second pass will be words based (all words w/ lenght 1 will be killed, blacklist words will be killed
// finally tokenizer provides a list of reasonable words

} // m8r namespace

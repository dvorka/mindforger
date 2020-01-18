/*
 autolinking_preprocessor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "autolinking_preprocessor.h"

namespace m8r {

using namespace std;

const string AutolinkingPreprocessor::CODE_BLOCK = string{"```"};
const string AutolinkingPreprocessor::MATH_BLOCK = string{"$$"};
const string AutolinkingPreprocessor::MF_URL_PROTOCOL = string{"mindforger://"};
const string AutolinkingPreprocessor::MF_URL_HOST = string{"links.mindforger.com"};
const string AutolinkingPreprocessor::MF_URL_PREFIX = AutolinkingPreprocessor::MF_URL_PROTOCOL + AutolinkingPreprocessor::MF_URL_HOST + "/";
const string AutolinkingPreprocessor::FILE_URL_PROTOCOL = string{"file://"};

AutolinkingPreprocessor::AutolinkingPreprocessor(Mind& mind)
    : insensitive{true},
      mind{mind}
{
}

AutolinkingPreprocessor::~AutolinkingPreprocessor()
{
}

} // m8r namespace

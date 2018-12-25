/*
 autolinking_preprocessor.cpp     MindForger thinking notebook

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
#include "autolinking_preprocessor.h"

namespace m8r {

using namespace std;

// see editor highligting regexps, test it at https://www.regextester.com
const string AutolinkingPreprocessor::PATTERN_LINK = string{"\\[(:?[\\S\\s]+)\\]\\(\\S+\\)"};
const string AutolinkingPreprocessor::PATTERN_CODE = string{"`[\\S\\s]+`"};
const string AutolinkingPreprocessor::PATTERN_MATH = string{"\\$[\\S\\s]+\\$"};
const string AutolinkingPreprocessor::PATTERN_HTTP = string{"https?://"};

AutolinkingPreprocessor::AutolinkingPreprocessor()
    : linkRegex{PATTERN_LINK},
      codeRegex{PATTERN_CODE},
      mathRegex{PATTERN_MATH},
      httpRegex{PATTERN_HTTP}
{
}

AutolinkingPreprocessor::~AutolinkingPreprocessor()
{
}

bool AutolinkingPreprocessor::containsLinkCodeMath(const string* line)
{
    std::smatch matchedString;
    if(std::regex_search(*line, matchedString, linkRegex)
         ||
       std::regex_search(*line, matchedString, codeRegex)
         ||
       std::regex_search(*line, matchedString, mathRegex)
        ||
       std::regex_search(*line, matchedString, httpRegex))
    {
        return true;
    }

    return false;
}

bool AutolinkingPreprocessor::aliasSizeComparator(const Thing* t1, const Thing* t2)
{
    return t1->getAutolinkingAlias().size() > t2->getAutolinkingAlias().size();
}

} // m8r namespace

/*
 common_words_blacklist.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "common_words_blacklist.h"

namespace m8r {

using namespace std;

CommonWordsBlacklist::CommonWordsBlacklist()
    : wordBlacklist{}
{
    MF_DEBUG("Common words blacklist:" << endl);
    wordBlacklist.addWord("the");
    wordBlacklist.addWord("of");
    wordBlacklist.addWord("and");
    wordBlacklist.addWord("to");
    wordBlacklist.addWord("a");
    wordBlacklist.addWord("in");
    wordBlacklist.addWord("for");
    wordBlacklist.addWord("is");
    wordBlacklist.addWord("on");
    wordBlacklist.addWord("that");
    wordBlacklist.addWord("by");
    wordBlacklist.addWord("this");
    wordBlacklist.addWord("with");
    wordBlacklist.addWord("I");
    wordBlacklist.addWord("you");
    wordBlacklist.addWord("it");
    wordBlacklist.addWord("not");
    wordBlacklist.addWord("or");
    wordBlacklist.addWord("be");
    wordBlacklist.addWord("are");
    wordBlacklist.addWord("from");
    wordBlacklist.addWord("at");
    wordBlacklist.addWord("as");
    wordBlacklist.addWord("your");
    wordBlacklist.addWord("all");
    wordBlacklist.addWord("have");
    wordBlacklist.addWord("new");
    wordBlacklist.addWord("more");
    wordBlacklist.addWord("an");
    wordBlacklist.addWord("was");
    wordBlacklist.addWord("we");
    wordBlacklist.addWord("will");
    wordBlacklist.addWord("can");
    wordBlacklist.addWord("us");
    wordBlacklist.addWord("about");
    wordBlacklist.addWord("if");
    wordBlacklist.addWord("my");
    wordBlacklist.addWord("has");
    wordBlacklist.addWord("but");
    wordBlacklist.addWord("our");
    wordBlacklist.addWord("one");
    wordBlacklist.addWord("other");
    wordBlacklist.addWord("do");
    wordBlacklist.addWord("no");
    wordBlacklist.addWord("they");
    wordBlacklist.addWord("he");
    wordBlacklist.addWord("may");
    wordBlacklist.addWord("what");
    wordBlacklist.addWord("which");
    wordBlacklist.addWord("their");
    wordBlacklist.addWord("any");
    wordBlacklist.addWord("there");
    wordBlacklist.addWord("so");
    wordBlacklist.addWord("his");
    wordBlacklist.addWord("when");
    wordBlacklist.addWord("who");
    wordBlacklist.addWord("also");
    wordBlacklist.addWord("get");
    wordBlacklist.addWord("am");
    wordBlacklist.addWord("been");
    wordBlacklist.addWord("would");
    wordBlacklist.addWord("how");
    wordBlacklist.addWord("were");
    wordBlacklist.addWord("me");
    wordBlacklist.addWord("some");
    wordBlacklist.addWord("these");
    wordBlacklist.addWord("its");
    wordBlacklist.addWord("like");
    wordBlacklist.addWord("than");
    wordBlacklist.addWord("had");
    wordBlacklist.addWord("should");
    wordBlacklist.addWord("her");
    wordBlacklist.addWord("such");
    wordBlacklist.addWord("then");
    wordBlacklist.addWord("where");
    wordBlacklist.addWord("does");
    wordBlacklist.addWord("could");
    wordBlacklist.addWord("did");
    wordBlacklist.addWord("those");
    wordBlacklist.addWord("want");
}

CommonWordsBlacklist::~CommonWordsBlacklist()
{
}

} // m8r namespace

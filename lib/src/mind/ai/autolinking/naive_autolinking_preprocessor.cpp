/*
 naive_autolinking_preprocessor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "naive_autolinking_preprocessor.h"

namespace m8r {

using namespace std;

// see editor highligting regexps, test it at https://www.regextester.com
const string NaiveAutolinkingPreprocessor::PATTERN_LINK = string{"\\[(:?[\\S\\s]+)\\]\\(\\S+\\)"};
const string NaiveAutolinkingPreprocessor::PATTERN_CODE = string{"`[\\S\\s]+`"};
const string NaiveAutolinkingPreprocessor::PATTERN_MATH = string{"\\$[\\S\\s]+\\$"};
const string NaiveAutolinkingPreprocessor::PATTERN_HTTP = string{"https?://"};

NaiveAutolinkingPreprocessor::NaiveAutolinkingPreprocessor(Mind& mind)
    : AutolinkingPreprocessor{mind},
      linkRegex{PATTERN_LINK},
      codeRegex{PATTERN_CODE},
      mathRegex{PATTERN_MATH},
      httpRegex{PATTERN_HTTP}
{
}

NaiveAutolinkingPreprocessor::~NaiveAutolinkingPreprocessor()
{
}

bool NaiveAutolinkingPreprocessor::containsLinkCodeMath(const string* line)
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

void NaiveAutolinkingPreprocessor::reindex()
{
    updateThingsIndex();
}

void NaiveAutolinkingPreprocessor::process(const vector<string*>& md, string &amd)
{
    MF_DEBUG("[Autolinking] NAIVE" << endl);

    insensitive = Configuration::getInstance().isAutolinkingCaseInsensitive();

    reindex();

    std::vector<std::string*> amdl;

    // IMPROVE ORDER of Ns determines what will be found > have active O Ns in head, etc.

    if(md.size()) {
        bool inCodeBlock=false, inMathBlock=false;
        for(string* l:md) {
            // every line is autolinked SEPARATELY

            string* nl = new string{};

            // skip code/math/... blocks
            if(stringStartsWith(*l, CODE_BLOCK)) {
                inCodeBlock = !inCodeBlock;                

                nl->append(*l);
                amdl.push_back(nl);
                continue;
            } else if(stringStartsWith(*l, MATH_BLOCK)) {
                inMathBlock= !inMathBlock;

                nl->append(*l);
                amdl.push_back(nl);
                continue;
            }

            // NAIVE skip lines where is either MD link or inline code presents
            // to preserve syntax correctness (potential matches might be lost)
            if(containsLinkCodeMath(l)) {
                nl->append(*l);
                amdl.push_back(nl);
                continue;
            }

            if(l && l->size()) {
                string w{*l}, chop{};
                MF_DEBUG(">>" << w << "<<" << endl);

                while(w.size()>0) {
                    // find match which is PREFIX of chopped line
                    bool linked = false;

                    // IMPROVE loop to be changed to Aho-Corasic trie

                    // inject Os, then Ns
                    for(Thing* t:things) {
                        size_t found;
                        bool match, insensitiveMatch;
                        string lowerAlias{};

                        if((found=w.find(t->getAutolinkingAlias()))!=string::npos
                              &&
                            !found)
                        {
                            match = true; insensitiveMatch = false;
                        } else {
                            lowerAlias.assign(t->getAutolinkingAlias());
                            lowerAlias[0] = std::tolower(t->getAutolinkingAlias()[0]);

                            if(insensitive
                                 &&
                               (found=w.find(lowerAlias))!=string::npos
                                 &&
                               !found)
                            {
                                match = insensitiveMatch = true;
                            } else {
                                match = false;
                            }
                        }

                        if(match) {
                            // avoid word PREFIX matches ~ ensure that WHOLE world is matched

                            string m{" \t,:;.!?<>{}&()-+/*"};
                            char c{w.size()==t->getAutolinkingAlias().size()?' ':w.at(t->getAutolinkingAlias().size())};
                            MF_DEBUG("  c: '" << c << "'" << endl);
                            if(w.size()==t->getAutolinkingAlias().size()
                                 ||
                               m.find(c)!=string::npos)
                            {
                                linked = true;

                                MarkdownOutlineRepresentation::toLink(
                                    insensitiveMatch?lowerAlias:t->getAutolinkingAlias(),
                                    t->getKey(),
                                    nl);

                                *nl += c;

                                // chop linked prefix word
                                w = w.substr(t->getAutolinkingAlias().size()+(w.size()==t->getAutolinkingAlias().size()?0:1));

                                break;
                            }
                        }
                    }

                    // chop one world from the beginning
                    MF_DEBUG("   l>" << std::boolalpha << linked << endl);
                    if(linked) {
                        // prefix has been linked + chopped
                        // IMPROVE SPACE vs. TAB
                        nl->append(" ");
                    } else {
                        // current w prefix was NOT linked > chop it and append it
                        size_t begin = w.find_first_of(" \t");
                        if(begin != string::npos) {
                            chop = w.substr(0, begin);
                            w = w.substr(begin+1);
                            nl->append(chop);
                            nl->append(" ");

                            MF_DEBUG("  -c>" << chop << endl);
                            MF_DEBUG("   w>" << w << endl);
                            MF_DEBUG("   <<" << *nl << endl);
                        } else {
                            // no more words (prefix already checked) > DONE
                            nl->append(w);

                            MF_DEBUG("   w>" << w << endl);
                            MF_DEBUG("   <<" << *nl << endl);

                            break;
                        }
                    }
                }

                MF_DEBUG("<<" << *nl << "<<" << endl);
                amdl.push_back(nl);
            } else {
                amdl.push_back(nl);
            }
        }
    }

    toString(amdl, amd);
}

} // m8r namespace

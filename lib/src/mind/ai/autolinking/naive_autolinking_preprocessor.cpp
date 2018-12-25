/*
 naive_autolinking_preprocessor.cpp     MindForger thinking notebook

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
#include "naive_autolinking_preprocessor.h"

namespace m8r {

using namespace std;

NaiveAutolinkingPreprocessor::NaiveAutolinkingPreprocessor(Mind& mind)
    : AutolinkingPreprocessor{},
      mind(mind),
      things{}
{
}

NaiveAutolinkingPreprocessor::~NaiveAutolinkingPreprocessor()
{
}

void NaiveAutolinkingPreprocessor::updateIndices()
{
    things.clear();

    // Os
    std::vector<Outline*> outlines;
    const vector<Outline*>& os=mind.getOutlines();
    for(Outline* o:os) outlines.push_back(o);
    std::sort(outlines.begin(), outlines.end(), aliasSizeComparator);
    for(Thing* t:outlines) things.push_back(t);

    // Ns
    std::vector<Note*> notes;
    mind.getAllNotes(notes);
    // sort names from longest to shortest (to have best ~ longest matches)
    std::sort(notes.begin(), notes.end(), aliasSizeComparator);
    for(Thing* t:notes) things.push_back(t);
}

void NaiveAutolinkingPreprocessor::process(const std::vector<std::string*>& md, std::vector<std::string*>& amd)
{
    MF_DEBUG("Autolinker:" << endl);

    bool insensitive = Configuration::getInstance().isAutolinkingCaseInsensitive();

    // IMPROVE consider synchronization ONLY in case that it's really needed
    updateIndices();

    // IMPROVE ORDER of Ns determines what will be found > have active O Ns in head, etc.

    if(md.size()) {
        bool inCodeBlock=false, inMathBlock=false;
        static const string CODE_BLOCK{"```"};
        static const string MATH_BLOCK{"$$"};
        for(string* l:md) {
            // every line is autolinked SEPARATELY

            string* nl = new string{};

            // skip code/math/... blocks
            if(stringStartsWith(*l,CODE_BLOCK)) {
                inCodeBlock = !inCodeBlock;                

                nl->append(*l);
                amd.push_back(nl);
                continue;
            } else if(stringStartsWith(*l,MATH_BLOCK)) {
                inMathBlock= !inMathBlock;

                nl->append(*l);
                amd.push_back(nl);
                continue;
            }

            // IMPROVE before Aho-Corasic is available rather skip lines where
            // either MD link or inline code presents to preserve syntax correctness.
            if(containsLinkCodeMath(l)) {

                nl->append(*l);
                amd.push_back(nl);
                continue;
            }

            if(l && l->size()) {
                string w{*l}, chop{};
                MF_DEBUG(">>" << w << ">>" << endl);

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
                amd.push_back(nl);
            } else {
                amd.push_back(nl);
            }
        }
    }
}

} // m8r namespace

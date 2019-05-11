/*
 cmark_aho_corasick_autolinking_preprocessor.cpp     MindForger thinking notebook

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
#include "cmark_aho_corasick_autolinking_preprocessor.h"

/*
 * Plan:
 *
 * - ensure correctness FIRST:
 *    - no trailing spaces
 *    - protection of bullet lists
 *    - protection of links/images/...
 *    - protection of inlined MATH $..$
 *    - blacklist ~ don't autolink e.g. http (to protect cmark's URLs autolinking)
 *
 * - polish correct version
 *    - code to methods
 *    - extra debugs away
 *
 * - performance
 *    - avoid autolinking whole O on its load - it's not needed
 *    - map search structure instead of Aho
 *    - benchmark on C
 *    - configurable time limit on autolinking and leave on exceeding it
 *
 */

namespace m8r {

using namespace std;

CmarkAhoCorasickAutolinkingPreprocessor::CmarkAhoCorasickAutolinkingPreprocessor(Mind& mind)
    : AutolinkingPreprocessor{mind}
{
}

CmarkAhoCorasickAutolinkingPreprocessor::~CmarkAhoCorasickAutolinkingPreprocessor()
{
}

void CmarkAhoCorasickAutolinkingPreprocessor::process(const std::vector<std::string*>& md, std::vector<std::string*>& amd)
{
#ifdef MF_MD_2_HTML_CMARK

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] begin CMARK-AHO" << endl);
    string ds{};
    toString(md, ds);
    MF_DEBUG("[Autolinking] input:" << endl << ">>" << ds << "<<" << endl);

    auto begin = chrono::high_resolution_clock::now();
#endif

    updateIndices();
    insensitive = Configuration::getInstance().isAutolinkingCaseInsensitive();

    if(md.size()) {

        // IMPROVE measure time in here and if over give limit, than STOP injecting
        // and leave i.e. what happens is that a time SLA will be fulfilled and
        // some part (prefix) of the input MD will be autolinked.

        bool inCodeBlock=false, inMathBlock=false;
        for(string* l:md) {
            // every line is autolinked SEPARATELY

            string* nl = new string{};

            // skip code/math/... blocks
            if(stringStartsWith(*l, CODE_BLOCK)) {
                inCodeBlock = !inCodeBlock;

                nl->append(*l);
                amd.push_back(nl);
                continue;
            } else if(stringStartsWith(*l, MATH_BLOCK)) {
                inMathBlock= !inMathBlock;

                nl->append(*l);
                amd.push_back(nl);
                continue;
            }

            if(l && l->size()) {
                parseMarkdownLine(l, nl);
                amd.push_back(nl);
            } else {
                amd.push_back(l);
            }
        }
    }
#ifdef DO_MF_DEBUG
    ds.clear();
    toString(amd, ds);
    MF_DEBUG("[Autolinking] output:" << endl << ">>" << ds << "<<" << endl);

    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("[Autolinking] MD autolinked in: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
#endif

#else
    amd = md;
#endif
}

void CmarkAhoCorasickAutolinkingPreprocessor::parseMarkdownLine(std::string* md, std::string* amd)
{
#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] parsing line:" << endl << ">>" << *md << "<<" << endl);
#endif

#ifdef MF_MD_2_HTML_CMARK
    // cmark identifies '    * my bullet' as code block, which is wrong...
    bool notCodeBlock=false;
    if(stringStartsWith(*md, "    ")) {
        (*md)[0] = '@';
        notCodeBlock = true;
        MF_DEBUG("[Autolinking] NO CODE FIX:" << endl << ">>" << *md << "<<" << endl);
    }

    const char* smd = md->c_str();
    cmark_node* document = cmark_parse_document(
        smd,
        strlen(smd),
        CMARK_OPT_DEFAULT);

    // AST iteration
    cmark_iter *i = cmark_iter_new(document);
    cmark_event_type eventType;
    cmark_node* zombieNode{};

    bool inLinkImgOrCode = false;

    while ((eventType = cmark_iter_next(i)) != CMARK_EVENT_DONE) {
        cmark_node *node = cmark_iter_get_node(i);

        if(zombieNode) {
            cmark_node_unlink(zombieNode);
            cmark_node_free(zombieNode);
            zombieNode = nullptr;
        }

        // IMPROVE make this a debug method
        // do something with `cur` and `ev_type`
        switch(eventType) {
        case CMARK_EVENT_ENTER:
            MF_DEBUG("ENTER");
            break;
        case CMARK_EVENT_EXIT:
            MF_DEBUG("LEAVE");
            break;
        case CMARK_EVENT_DONE:
            MF_DEBUG("DONE");
            break;
        case CMARK_EVENT_NONE:
            MF_DEBUG("NONE");
            break;
        default:
            MF_DEBUG(".");
        }

        // Nodes must only be modified after an `EXIT` event,
        // or an `ENTER` event for leaf nodes.

        // inlined autolinking constructions to avoid - auto-link in:
        // - link
        // - image
        // - code
        // ... iterate nodes and skip <text/> if UNDER link/image/code,
        // otherwise trim text and try to autolink it.

        switch(cmark_node_get_type(node)) {
        case CMARK_NODE_CODE:
            MF_DEBUG(" code");
        case CMARK_NODE_LINK:
            MF_DEBUG(" link");
        case CMARK_NODE_IMAGE:
            MF_DEBUG(" image");
            if(eventType == CMARK_EVENT_ENTER) {
                inLinkImgOrCode = true;
            } else if (eventType == CMARK_EVENT_EXIT) {
                inLinkImgOrCode = false;
            }
            break;
        case CMARK_NODE_TEXT:
            MF_DEBUG(" text '" << cmark_node_get_literal(node) << "'" << endl);
            if(!inLinkImgOrCode) {
                // replace text node w/ sequence of text and link nodes
                injectThingsLinks(node);
                zombieNode = node;
            }
            break;
        default:
            MF_DEBUG(" .");
        }

        MF_DEBUG(endl);
    }

    cmark_iter_free(i);

    // TODO set correct
    char* cmm = cmark_render_commonmark(document, 0, 0);
    amd->assign(cmm);
    if(notCodeBlock) {
        amd[0] = ' ';
    }
    amd->pop_back();

#ifdef DO_MF_DEBUG
    char* xml = cmark_render_xml(document, 0);
    MF_DEBUG("[Autolinking] Line's cmark AST as XML:" << endl << endl);
    MF_DEBUG(xml << endl);
    free(xml);

    MF_DEBUG("[Autolinking] Line's cmark AST as MD:" << endl << ">>" << *amd << "<<" << endl);
#endif

    free(cmm);

#else
    // TODO copy md to amd
#endif
}

void CmarkAhoCorasickAutolinkingPreprocessor::injectThingsLinks(cmark_node* origNode)
{
    // copy w to t as it will be chopped word/match by word/match from head to tail
    string txt{cmark_node_get_literal(origNode)}, at{}, chop{};

    cmark_node* node{};
    cmark_node* linkNode{};
    cmark_node* txtNode{};

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Injecting links to: '" << txt << "'" << endl);
#endif

    while(txt.size()>0) {
        // find match which is PREFIX of chopped line
        bool linked = false;

        // IMPROVE rewrite string search to Aho-Corasic trie

        // IMPROVE non-Aho-Corasic, but TRIE search to speed up search - search
        // input prefix in trie made of all strings

        // IMPROVE existing (non-Aho-Corasic) algorithm can be optimized:
        // - sort Os/Ns alphabetically
        //     - search structure: map w/ leading char as key and list of
        //       Os/Ns names ordered by length as value
        //         - have this for both case in/sensitive cases ~ 2 structs
        //     - sort words with the same 1st word by length
        //     - remember begin-end of words starting with the char
        // - determine first w character
        //     - search only Os/Ns w/ that particular 1st character only
        // ... this will speed up search significantly, perhaps it can be
        // 10x faster than search for all Os/Ns names

        // check all Os/Ns for being prefix of txt (Os first, Ns then)
        for(Thing* t:things) {
            size_t found;
            bool match, insensitiveMatch;
            string lowerAlias{};

            // FIND: accept occurences in the HEAD of word only
            if((found=txt.find(t->getAutolinkingAlias()))!=string::npos
                  &&
                !found)
            {
                match = true; insensitiveMatch = false;
            } else {
                lowerAlias.assign(t->getAutolinkingAlias());
                lowerAlias[0] = std::tolower(t->getAutolinkingAlias()[0]);

                // FIND: accept occurences in the HEAD of word only
                if(insensitive
                     &&
                   (found=txt.find(lowerAlias))!=string::npos
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

                // allowed trailing characters (_... added)
                string tMatch{" \t,:;.!?<>{}&()-+/*_=%~#$^[]'\""};
                // determine trailing char c
                char tChar{txt.size()==t->getAutolinkingAlias().size()?' ':txt.at(t->getAutolinkingAlias().size())};
                MF_DEBUG("  trailing char: '" << tChar << "'" << endl);

                if(txt.size()==t->getAutolinkingAlias().size()
                     ||
                   tMatch.find(tChar)!=string::npos)
                {
                    linked = true;

                    // IMPROVE make this method
                    // AST: add text node w/ content preceding link
                    if(at.size()) {
                        txtNode = cmark_node_new(CMARK_NODE_TEXT);
                        cmark_node_set_literal(txtNode, at.c_str());
                        // IMPROVE make this private method
                        if(node) {
                            cmark_node_insert_after(node, txtNode);
                        } else {
                            cmark_node_insert_before(origNode, txtNode);
                        }
                        node = txtNode;

                        at.clear();
                    }

                    // IMPROVE make this method
                    // AST: add link
                    linkNode = cmark_node_new(CMARK_NODE_LINK);
                    cmark_node_set_url(linkNode, t->getKey().c_str());
                    txtNode = cmark_node_new(CMARK_NODE_TEXT);
                    cmark_node_set_literal(txtNode, insensitiveMatch?lowerAlias.c_str():t->getAutolinkingAlias().c_str());
                    cmark_node_append_child(linkNode, txtNode);
                    if(node) {
                        cmark_node_insert_after(node, linkNode);
                    } else {
                        cmark_node_insert_before(origNode, linkNode);
                    }
                    node = linkNode;

                    // append trailing char
                    // MD11 at += c;

                    // chop linked prefix word from input word w
                    txt = txt.substr(t->getAutolinkingAlias().size()+(txt.size()==t->getAutolinkingAlias().size()?0:1));

                    // leave Os/Ns loop on the first match
                    break;
                }
            }
        }

        MF_DEBUG("   l>" << std::boolalpha << linked << endl);
        if(linked) {
            // prefix has been linked + chopped
            // IMPROVE trailing should be already appended above
            // IMPROVE SPACE vs. TAB
            at.append(" ");
        } else {
            // current w prefix was NOT linked > chop it and append it
            size_t begin = txt.find_first_of(" \t");
            if(begin != string::npos) {
                chop = txt.substr(0, begin);
                txt = txt.substr(begin+1);
                at.append(chop);
                at.append(" ");

                MF_DEBUG("  -c>" << chop << endl);
                MF_DEBUG("   w>" << txt << endl);
                MF_DEBUG("   <<" << at << endl);
            } else {
                // no more words (prefix already checked) > DONE
                at.append(txt);

                MF_DEBUG("   w>" << txt << endl);
                MF_DEBUG("   <<" << at << endl);

                break;
            }
        }
    }

    // AST: add text node w/ content preceding link
    if(at.size()) {
        txtNode = cmark_node_new(CMARK_NODE_TEXT);
        cmark_node_set_literal(txtNode, at.c_str());
        if(node) {
            cmark_node_insert_after(node, txtNode);
        } else {
            cmark_node_insert_before(origNode, txtNode);
        }
        node = txtNode;

        // not required: at.clear();
    }
}

} // m8r namespace

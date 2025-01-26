/*
 cmark_aho_corasick_block_autolinking_preprocessor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "cmark_aho_corasick_block_autolinking_preprocessor.h"
// cmark-gfm headers must NOT be included in header - Win builds fail
#ifdef MF_MD_2_HTML_CMARK
  #include <cmark-gfm.h>
#endif

/*
 * High priority tasks:
 *
 * - unit tests first, manual tests then
 * - integrity
 *    - finish navigation after clicking autolinked link
 *      - 1 match > does nothing
 *      - multiple matches > SIGSEGV in choice dialog
 *    - N title not rendered if section depth too high >6 (make it GH bug)
 *    - HtmlOutlineRepresentation#271 - trailing new line
 *    - SIGSEGV click autolinked link w/ one match > dialog > Show > SIGSEGV
 *    - broken find outline by name dialog w/ autolinking customization: WRONG title & search
 *    - protect multiline MATH blocks $$ (track $$ literasl + skip literals starting w/ $$)
 *    - autolink tags: if no N/O found on click, then open tags dialog
 *    - better matching: consider lowercasing of first characters of all words in title
 *      (JavaScript algorithm library uses upper case words as title convention - no matches)
 * - bugs
 *    - cmark leaks when ALL unit tests are run (not just cmark test case)
 * - performance
 *    - move trie to memory (or mind) and keep it up to date there (based on mind state)
 *    - O or N edit: always DELETE old name and add new name to TRIE
 *
 * Plan:
 *
 * - ensure correctness FIRST ~ unit tests w/ asserts:
 *    - protection of bullet lists
 *    - protection of deep bullet lists
 *    - no trailing spaces
 *    - protection of links/images/...
 *    - protection of inlined MATH $..$
 *    - blacklist ~ don't autolink e.g. http (to protect cmark's URLs autolinking)
 *
 * - performance
 *    - avoid trie rebuild - on N save():
 *      ! implement delete operation on trie:
 *       - enhance trie w/ refcount for words so that delete knows whether to remove or not
 *       - implement delete() operation:
 *        - finds word mark, decreases refcount, if it's leaf than it goes up deletes orphan
 *      OR:
 *      1) synchronous: add new N name to trie on its save()
 *      2) async: after save let trie rebuild async in other
 *         thread to update it (trie access must be synchronized)
 *    - avoid autolinking whole O on its load - it's not needed > debug why it happens
 *    - map search structure instead of Aho
 *    - benchmark on C++ repo
 *    ! configurable time limit on autolinking and leave on exceeding it
 *      (measure time from autlinking start and stop document transformation)
 */

namespace m8r {

using namespace std;

/*
 * OOC methods to avoid the need for having cmark-gfm.h in the header which causes
 * problems with Windows build.
 */

cmark_node* injectAstLinkNode(
    cmark_node* srcNode,
    cmark_node* node,
    string& text
) {
    cmark_node* linkNode{cmark_node_new(CMARK_NODE_LINK)};
    cmark_node* txtNode{};

    string link{AutolinkingPreprocessor::MF_URL_PREFIX};
    link.append(text);
    cmark_node_set_url(linkNode, link.c_str());
    txtNode = cmark_node_new(CMARK_NODE_TEXT);
    cmark_node_set_literal(txtNode, text.c_str());
    cmark_node_append_child(linkNode, txtNode);
    if(node) {
        cmark_node_insert_after(node, linkNode);
    } else {
        cmark_node_insert_before(srcNode, linkNode);
    }
    return linkNode;
}

cmark_node* injectAstTxtNode(
    cmark_node* srcNode,
    cmark_node* node,
    string& text
) {
    cmark_node* txtNode{cmark_node_new(CMARK_NODE_TEXT)};

    MF_DEBUG("       Inject TXT: >>>" << text << "<<<" << endl);

    cmark_node_set_literal(txtNode, text.c_str());
    if(node) {
        cmark_node_insert_after(node, txtNode);
    } else {
        cmark_node_insert_before(srcNode, txtNode);
    }
    text.clear();

    MF_DEBUG("         TXT node: >>>" << txtNode << "<<<" << endl);
    return txtNode;
}

void injectThingsLinks(cmark_node* srcNode, Mind& mind)
{
    // copy w to t as it will be chopped word/match by word/match from head to tail
    string txt{cmark_node_get_literal(srcNode)};
    string pre{}, at{}, chop{};
    size_t preSize{};

    cmark_node* node{};

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Injecting links to: '" << txt << "'" << endl);
#endif

    while(txt.size()>0) {
        // skip trailing chars and append them
        preSize = 0;
        while(preSize < txt.size()) {
            // check $$ to detect begin/end of math (MathJax) section to ignore it
            if(txt.size()>=2 && '$'==txt.at(0) && '$'==txt.at(1)) {
                MF_DEBUG("MATH matched" << endl);
                // TODO toggle math
                preSize+=2;
                if(txt.size()==2) {
                    break;
                }
            }

            if(CmarkAhoCorasickBlockAutolinkingPreprocessor::TRAILING_CHARS.find(txt.at(preSize)) != string::npos) {
                preSize++;
            } else {
                break;
            }
        }
        if(preSize) {
            // chop trailing chars prefix from input and append it to result
            at.append(txt.substr(0, preSize));
            txt = txt.substr(preSize);

            MF_DEBUG("  Skipping trailing chars: '" << txt.substr(0, preSize) << "'" << endl);
            MF_DEBUG("     txt: '" << txt << "'" << endl);
            MF_DEBUG("     at : '" << at  << "'" << endl);
        }

        // try to match word
        pre.clear();

        MF_DEBUG("  Trie search txt: '" << txt << "'" << endl);
        if(mind.autolinkFindLongestPrefixWord(txt, pre)) {
            MF_DEBUG("    Matched prefix: '" << pre << "'" << endl);

            // avoid word PREFIX matches ~ ensure that WHOLE world is matched:
            // - match followed by trailing char
            // - match until EOL

            // determine trailing char
            char tChar{txt.size()==pre.size()?' ':txt.at(pre.size())};
            MF_DEBUG("    Match's trailing char: '" << tChar << "'" << endl);
            if(CmarkAhoCorasickBlockAutolinkingPreprocessor::TRAILING_CHARS.find(tChar) != string::npos
               || txt.size() == pre.size()
            ) {
                // AST: add text node w/ content preceding link
                // IMPROVE make this method
                if(at.size()) {
                    node = injectAstTxtNode(srcNode, node, at);
                }

                // AST: add link
                node = injectAstLinkNode(srcNode, node, pre);

                // chop linked prefix word from input word w
                if(txt.size() == pre.size()) {
                    txt.clear();
                } else {
                    txt = txt.substr(pre.size());
                }
                // trailing char will be handled later
                at.clear();

                MF_DEBUG("    txt: '" << txt << "'" << endl);
                MF_DEBUG("    at : '" << at  << "'" << endl);
            } else {
                // invalid trailing char > matched a NOT-whole-world prefix > skip and append one word

                // TODO make it method
                // current w prefix was NOT linked > chop it and append it
                size_t begin = txt.find_first_of(" \t");
                if(begin != string::npos) {
                    chop = txt.substr(0, begin);
                    txt = txt.substr(begin+1);
                    at.append(chop);
                    // TODO IMPORTANT append what was found - space or tab! simply index there
                    at.append(" ");

                    MF_DEBUG("  Skiping word: '" << chop << "'" << endl);
                    MF_DEBUG("     txt: '" << txt << "'" << endl);
                    MF_DEBUG("     at : '" << at  << "'" << endl);
                } else {
                    // no more words (prefix already checked) > DONE
                    at.append(txt);

                    MF_DEBUG("  DONE no-more words: '" << chop << "'" << endl);
                    MF_DEBUG("     txt: '" << txt << "'" << endl);
                    MF_DEBUG("     at : '" << at  << "'" << endl);

                    break;
                }
            }
        } else {
            // didn't match prefix > skip and append one word

            // TODO make it method
            // current w prefix was NOT linked > chop it and append it
            size_t begin = txt.find_first_of(" \t");
            if(begin != string::npos) {
                chop = txt.substr(0, begin);
                txt = txt.substr(begin+1);
                at.append(chop);
                // TODO IMPORTANT append what was found - space or tab! simply index there
                at.append(" ");

                MF_DEBUG("  Skiping word: '" << chop << "'" << endl);
                MF_DEBUG("     txt: '" << txt << "'" << endl);
                MF_DEBUG("     at : '" << at  << "'" << endl);
            } else {
                // no more words (prefix already checked) > DONE
                at.append(txt);

                MF_DEBUG("  DONE no-more words: '" << chop << "'" << endl);
                MF_DEBUG("     txt: '" << txt << "'" << endl);
                MF_DEBUG("     at : '" << at  << "'" << endl);

                break;
            }
        }
    }

    // AST: add text node w/ content preceding link
    if(at.size()) {
        node = injectAstTxtNode(srcNode, node, at);
    }
}

/*
 * Preprocessor.
 */

const string CmarkAhoCorasickBlockAutolinkingPreprocessor::TRAILING_CHARS
    = string{" \t,:;.!?<>{}&()-+/*\\_=%~#$^[]'\""};

CmarkAhoCorasickBlockAutolinkingPreprocessor::CmarkAhoCorasickBlockAutolinkingPreprocessor(Mind& mind)
    : AutolinkingPreprocessor{mind}
{
}

CmarkAhoCorasickBlockAutolinkingPreprocessor::~CmarkAhoCorasickBlockAutolinkingPreprocessor()
{
}

void CmarkAhoCorasickBlockAutolinkingPreprocessor::process(
    const vector<string*>& md,
    string& amd
) {
#ifdef MF_MD_2_HTML_CMARK

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] begin CMARK" << endl);
    string ds{};
    toString(md, ds);
    MF_DEBUG("[Autolinking] input:" << endl << ">>>" << ds << "<<<" << endl);

    auto begin = chrono::high_resolution_clock::now();
#endif

    insensitive = Configuration::getInstance().isAutolinkingCaseInsensitive();

    // IMPROVE measure time in here and if over give limit, than STOP injecting
    // and leave i.e. what happens is that a time SLA will be fulfilled and
    // some part (prefix) of the input MD will be autolinked.

    if(md.size()) {
        string mds{};
        toString(md, mds);
        const char* mdsc{mds.c_str()};

        cmark_node* document = cmark_parse_document(
            mdsc,
            strlen(mdsc),
            CMARK_OPT_DEFAULT
        );

        cmark_iter* astWalker = cmark_iter_new(document);

        vector<cmark_node*> zombies{};

        while (cmark_iter_next(astWalker) != CMARK_EVENT_DONE) {
            cmark_node* node = cmark_iter_get_node(astWalker);

            // process TEXT nodes whose parent is PARAGRAPH
            if(CMARK_NODE_TEXT == cmark_node_get_type(node)
                 &&
               CMARK_NODE_PARAGRAPH == cmark_node_get_type(cmark_node_parent(node)))
            {
                MF_DEBUG("[Autolinking] text node: '" << cmark_node_get_literal(node) << "'" << endl);
                injectThingsLinks(node, mind);
                zombies.push_back(node);
            }
        }

        cmark_iter_free(astWalker);

        MF_DEBUG("[Autolinking] killing zombies:" << endl);
        for(cmark_node* zombieNode: zombies) {
            MF_DEBUG("    " << cmark_node_get_literal(zombieNode) << endl);
            cmark_node_unlink(zombieNode);
            cmark_node_free(zombieNode);
        }
        MF_DEBUG("[Autolinking] DONE zombies" << endl);

        char* cmm = cmark_render_commonmark(document, 0, 0);
        if(cmm) {
            amd.assign(cmm);
            // TODO valgrind delete cmm;
            free(cmm);
            amd.pop_back();
        } else {
            amd.clear();
        }

        cmark_node_free(document);
    } else {
        amd.clear();
    }

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] output:" << endl << "  >>>" << amd << "<<<" << endl);

    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("[Autolinking] MD autolinked in: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
#endif

#else
    // cmark-gfm not available - returning Markdown as is
    toString(md, amd);
#endif
}

} // m8r namespace

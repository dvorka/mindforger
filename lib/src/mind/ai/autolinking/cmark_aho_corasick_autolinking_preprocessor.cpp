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
    MF_DEBUG("[Autolinking] CMARK-AHO" << endl);

    updateIndices();
    insensitive = Configuration::getInstance().isAutolinkingCaseInsensitive();

    if(md.size()) {
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
                MF_DEBUG(">>" << *l << ">>" << endl);
                parseMarkdownLine(l, nl);
                MF_DEBUG("<<" << *nl << "<<" << endl);
                amd.push_back(nl);
            } else {
                amd.push_back(nl);
            }
        }
    }
#else
    amd = md;
#endif
}

void CmarkAhoCorasickAutolinkingPreprocessor::parseMarkdownLine(std::string* md, std::string* amd)
{
    /*
     * TODO ... setting link is WRONG > AST must be modified instead
     */

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] parse line '" << *md << "'" << endl);
#endif
#ifdef MF_MD_2_HTML_CMARK
    const char* smd = md->c_str();
    cmark_node* document = cmark_parse_document(
        smd,
        strlen(smd),
        CMARK_OPT_DEFAULT);

    MF_DEBUG(endl);

    // AST iteration

    cmark_event_type ev_type;
    cmark_iter *iter = cmark_iter_new(document);

    string itxt{}, otxt{};

    cmark_node* linkNode{};
    cmark_node* txtNode{};
    cmark_node* zombieNode{};

    while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
        cmark_node *cur = cmark_iter_get_node(iter);

        if(zombieNode) {
            cmark_node_unlink(zombieNode);
            cmark_node_free(zombieNode);
            zombieNode = nullptr;
        }

        // do something with `cur` and `ev_type`
        switch(ev_type) {
        case CMARK_EVENT_ENTER:
            cout << "ENTER";
            break;
        case CMARK_EVENT_EXIT:
            cout << "LEAVE";
            break;
        case CMARK_EVENT_DONE:
            cout << "DONE";
            break;
        case CMARK_EVENT_NONE:
            cout << "NONE";
            break;
        default:
            cout << ".";
        }

        switch(cmark_node_get_type(cur)) {
        case CMARK_NODE_CODE:
            cout << " code";
            break;
        case CMARK_NODE_LINK:
            cout << " link";
            break;
        case CMARK_NODE_IMAGE:
            cout << " image";
            break;
        case CMARK_NODE_TEXT:
            cout << " text " << cmark_node_get_literal(cur);

            // replace text node w/ sequence of text and link nodes

            // inject link
            linkNode = cmark_node_new(CMARK_NODE_LINK);
            cmark_node_set_url(linkNode,"http://acme.com#ANCHOR");
            cmark_node_set_literal(linkNode, "MY-LITERAL");

            txtNode = cmark_node_new(CMARK_NODE_TEXT);
            cmark_node_set_literal(txtNode, "1-LINK-LITERAL");
            cmark_node_append_child(linkNode, txtNode);

            cmark_node_insert_before(cur, linkNode);

            // inject text
            txtNode = cmark_node_new(CMARK_NODE_TEXT);
            cmark_node_set_literal(txtNode, "2-TXT-NODE");
            cmark_node_insert_after(linkNode, txtNode);

            zombieNode = cur;

            //itxt.assign(cmark_node_get_literal(cur));
            //otxt.clear();
            //injectThingsLinks(&itxt, &otxt);
            //cmark_node_set_literal(cur, otxt.c_str());
            break;
        default:
            cout << " .";
        }

        cout << endl;
    }

    cmark_iter_free(iter);

    // Nodes must only be modified after an `EXIT` event,
    // or an `ENTER` event for leaf nodes.

    // inlined autolinking constructions to avoid - auto-link in:
    // - link
    // - image
    // - code
    // ... iterate nodes and skip <text/> if UNDER link/image/code,
    // otherwise trim text and try to autolink it.
    //
    // Set autolinked text and then try to serialize to MD, it should
    // stay there.

    // TODO serialize AST to amd
    char* txt = cmark_render_commonmark(document, 0, 100);
    MF_DEBUG("OUTPUT cmark AST as MD:" << endl << endl);
    MF_DEBUG(txt << endl);

    char* xml = cmark_render_xml(document, 0);
    MF_DEBUG("cmark AST as XML:" << endl << endl);
    MF_DEBUG(xml << endl);

    txt = cmark_render_plaintext(document, 0, 100);
    MF_DEBUG("OUTPUT cmark AST as TXT:" << endl << endl);
    MF_DEBUG(txt << endl);

#else
    // TODO copy md to amd
#endif
}

void CmarkAhoCorasickAutolinkingPreprocessor::injectThingsLinks(string* t, string* at)
{
#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Injecting to '" << *t << "'" << endl);
#endif

    string w{*t}, chop{};

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
                        at);

                    *at += c;

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
            at->append(" ");
        } else {
            // current w prefix was NOT linked > chop it and append it
            size_t begin = w.find_first_of(" \t");
            if(begin != string::npos) {
                chop = w.substr(0, begin);
                w = w.substr(begin+1);
                at->append(chop);
                at->append(" ");

                MF_DEBUG("  -c>" << chop << endl);
                MF_DEBUG("   w>" << w << endl);
                MF_DEBUG("   <<" << *at << endl);
            } else {
                // no more words (prefix already checked) > DONE
                at->append(w);

                MF_DEBUG("   w>" << w << endl);
                MF_DEBUG("   <<" << *at << endl);

                break;
            }
        }
    }

    MF_DEBUG("<<" << *at << "<<" << endl);
}

} // m8r namespace

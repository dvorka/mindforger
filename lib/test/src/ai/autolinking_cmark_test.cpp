/*
 autolinkin_cmark_test.cpp     MindForger application test

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <vector>
#include <string>
#include <string.h>

#include "../test_gear.h"

#include <gtest/gtest.h>

#ifdef MF_MD_2_HTML_CMARK
#include <cmark-gfm.h>

#include "../../../src/mind/ai/autolinking/cmark_aho_corasick_autolinking_preprocessor.h"

using namespace std;

extern char* getMindforgerGitHomePath();

TEST(AutolinkingCmarkTestCase, DISABLED_CmarkAst)
{
    const char* examples[5];
    examples[0] = "Hello *world*!";
    examples[1] = "* bullet [link label](http://acme)";
    examples[2] = "   * level 1 bullet, **bold** `code`";
    examples[3] = "> quote text w/ *italic* text";
    examples[4] = "1. numbered section w/ `code` text";

    // AST rendering

    for(int i=0; i<5; i++) {
        cmark_node* document = cmark_parse_document(
            examples[i],
            strlen(examples[i]),
            CMARK_OPT_DEFAULT);

        cout << endl;

        char* xml = cmark_render_xml(document, 0);
        cout << "cmark AST as XML:" << endl << endl;
        cout << xml << endl;

        char* txt = cmark_render_commonmark(document, 0, 100);
        cout << "cmark AST as MD:" << endl << endl;
        cout << txt << endl;

        // AST iteration

        cmark_event_type ev_type;
        cmark_iter *iter = cmark_iter_new(document);

        while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
            cmark_node *cur = cmark_iter_get_node(iter);

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
                cout << " text";
                break;
            default:
                cout << " .";
            }

            cout << endl;
        }

        cmark_iter_free(iter);

        // Nodes must only be modified after an `EXIT` event,
        // or an `ENTER` event for leaf nodes.
    }

    // inlined autolinking constructions to avoid - auto-link in:
    // - link
    // - image
    // - code
    // ... iterate nodes and skip <text/> if UNDER link/image/code,
    // otherwise trim text and try to autolink it.
    //
    // Set autolinked text and then try to serialize to MD, it should
    // stay there.
}

TEST(AutolinkingCmarkTestCase, MicroRepo)
{
    string repositoryPath{"/lib/test/resources/autolinking-micro-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-act-amr.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(1, mind.remind().getOutlinesCount());

    // autolinking
    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::CmarkAhoCorasickAutolinkingPreprocessor autolinker{mind};
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    vector<string*> autolinkedMd{};
    autolinker.process(n->getDescription(), autolinkedMd);

    string autolinkedString{};
    m8r::toString(autolinkedMd, autolinkedString);
    cout << "= BEGIN AUTO MD =" << endl << autolinkedString << endl << "= END AUTO MD =" << endl;

    for(string* s:autolinkedMd) {
        delete s;
    }
}

TEST(AutolinkingCmarkTestCase, DISABLED_BasicRepo)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-act-br.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(3, mind.remind().getOutlinesCount());
    m8r::CmarkAhoCorasickAutolinkingPreprocessor autolinker{mind};

    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    vector<string*> autolinkedMd{};
    autolinker.process(n->getDescription(), autolinkedMd);
    string autolinkedString{};
    m8r::toString(autolinkedMd, autolinkedString);
    cout << "= BEGIN AUTO MD =" << endl << autolinkedString << endl << "= END AUTO MD =" << endl;

    for(string* s:autolinkedMd) {
        delete s;
    }
}

#endif // MF_MD_2_HTML_CMARK

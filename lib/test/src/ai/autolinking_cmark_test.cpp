/*
 autolinkin_cmark_test.cpp     MindForger application test

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <stack>
#include <string>
#include <string.h>

#include "../test_utils.h"

#include <gtest/gtest.h>

// cmark-gfm include is broken on Windows
#ifndef _WIN32
#ifdef MF_MD_2_HTML_CMARK
#include <cmark-gfm.h>

#include "../../../src/gear/file_utils.h"
#include "../../../src/mind/ai/autolinking/cmark_aho_corasick_block_autolinking_preprocessor.h"

using namespace std;

extern char* getMindforgerGitHomePath();

TEST(AutolinkingCmarkTestCase, CmarkAstRowWalker)
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
        free(xml);

        char* txt = cmark_render_commonmark(document, 0, 100);
        cout << "cmark AST as MD:" << endl << endl;
        cout << txt << endl;
        free(txt);

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

        cmark_node_free(document);
    }
}

/**
 * @brief Test whether postponed autolinking AST transformation is iterator and mem integrity OK.
 */
// Transformer observations:
// - <text> nodes to be autolinked only
// - <text> node is ALWAYS child of <paragraph> node
//    - <text> node w/ different parent, like <link>, <emph> or <strong>,
//      must NOT be autolinked
// - autolinking modifies AST in a way that original tree:
//     <paragraph>
//       ...
//       <text>...</text>
//       ...
//     </paragraph>
//   is changed to AST like:
//     <paragraph>
//       ...
//       <text>...</text>
//       <link>...</link>
//       <text>...</text>
//       <link>...</link>
//       ...
//     </paragraph>
//
// Transformer strategy:
//
// Insert new nodes BEFORE obsolete one to protect iterator
// and delete obsolete node(s) when iterator moves to next item
// OR introduce GARBAGE COLLECTOR and delete all obsolete nodes
// at the end when AST is walked and iterator released.
//
// Open questions:
// - is AST iterator invalidated on AST modification?
TEST(AutolinkingCmarkTestCase, CmarkAstBlockTransformer)
{
    const char* text =
        "Hello *world*!\n"
        "* bullet [link label](http://acme)\n"
        "   * level 1 bullet, **bold** `code`\n"
        "> quote text w/ *italic* text\n"
        "1. numbered FIRST section w/ `code` text\n"
        "   1. [numbered SECOND level](http://mf)\n"
        "       1. numbered THIRD level\n"
        "\n"
        "AVOID code:\n"
        "```\n"
        "...source code...\n"
        "```\n"
        "AVOID math:\n"
        "$$\n"
        "...math...\n"
        "$$\n"
        "DONE";

    // AST rendering
    cmark_node* document = cmark_parse_document(
        text,
        strlen(text),
        CMARK_OPT_DEFAULT);

    cout << endl;

    char* xml = cmark_render_xml(document, 0);
    cout << "cmark AST as XML:" << endl << endl;
    cout << xml << endl;
    free(xml);

    char* txt = cmark_render_commonmark(document, 0, 100);
    cout << "cmark AST as MD:" << endl << endl;
    cout << txt << endl;
    free(txt);

    //
    // AST dump
    //

    cmark_event_type ev_type;
    cmark_iter *iter = cmark_iter_new(document);

    while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
        cmark_node *cur = cmark_iter_get_node(iter);

        // do something with `cur` and `ev_type`
        switch(ev_type) {
        case CMARK_EVENT_ENTER:
            cout << "->";
            break;
        case CMARK_EVENT_EXIT:
            cout << "<-";
            break;
        case CMARK_EVENT_DONE:
            cout << "DONE";
            break;
        case CMARK_EVENT_NONE:
            cout << "NONE";
            break;
        default:
            cout << ":";
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
        case CMARK_NODE_PARAGRAPH:
            cout << " paragraph";
            break;
        default:
            cout << " .";
        }

        cout << endl;
    }

    cmark_iter_free(iter);

    // Nodes must only be modified after an `EXIT` event,
    // or an `ENTER` event for leaf nodes.

    //
    // TRANSFORMER
    //

    cout << endl << endl << "TRANSFORMATION:" << endl;

    iter = cmark_iter_new(document);

    string s{};

    cmark_node* txtNodeToReplace;
    cmark_node* linkNode;
    cmark_node* linkTextNode;
    cmark_node* txtPreNode;
    cmark_node* txtPostNode;

    vector<cmark_node*> zombies{};

    while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {
        cmark_node *cur = cmark_iter_get_node(iter);

        switch(ev_type) {
        case CMARK_EVENT_ENTER:
            cout << "->";
            break;
        case CMARK_EVENT_EXIT:
            cout << "<-";
            break;
        case CMARK_EVENT_DONE:
            cout << "DONE";
            break;
        case CMARK_EVENT_NONE:
            cout << "NONE";
            break;
        default:
            cout << ":";
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

            txtNodeToReplace = cur;

            if(CMARK_NODE_PARAGRAPH != cmark_node_get_type(cmark_node_parent(txtNodeToReplace))) {
                cout << " SKIPPING: " << cmark_node_get_literal(txtNodeToReplace);
                break;
            }

            s.clear();
            s.append("PREFIX:");
            s.append(cmark_node_get_literal(txtNodeToReplace));

            // split orig txt node to: txt, link, txt nodes

            txtPreNode = cmark_node_new(CMARK_NODE_TEXT);
            cmark_node_set_literal(txtPreNode, s.c_str());
            cmark_node_insert_before(txtNodeToReplace, txtPreNode);

            linkNode = cmark_node_new(CMARK_NODE_LINK);
            cmark_node_set_url(linkNode, "http://acme");
            linkTextNode = cmark_node_new(CMARK_NODE_TEXT);
            cmark_node_set_literal(linkTextNode, "LINK-TEXT");
            cmark_node_append_child(linkNode, linkTextNode);
            cmark_node_insert_before(txtNodeToReplace, linkNode);

            txtPostNode = cmark_node_new(CMARK_NODE_TEXT);
            cmark_node_set_literal(txtPostNode, "POSTFIX-TEXT");
            cmark_node_insert_before(txtNodeToReplace, txtPostNode);

            zombies.push_back(txtNodeToReplace);

            break;
        case CMARK_NODE_PARAGRAPH:
            cout << " paragraph";
            break;
        default:
            cout << " .";
        }

        cout << endl;
    }

    cmark_iter_free(iter);

    // IMPORTANT: remove all garbage nodes from document
    cout << "Removing zombie nodes:" << endl;
    for(cmark_node* zombieNode: zombies) {
        cout << "    " << cmark_node_get_literal(zombieNode) << endl;
        cmark_node_unlink(zombieNode);
        cmark_node_free(zombieNode);
    }
    cout << "DONE zombies" << endl;

    xml = cmark_render_xml(document, 0);
    cout << endl << "TRANSFORMED AST as XML:" << endl << endl;
    cout << xml << endl;
    free(xml);

    txt = cmark_render_commonmark(document, 0, 100);
    cout << "TRANSFORMED AST as MD:" << endl << endl;
    cout << txt << endl;
    free(txt);

    cmark_node_free(document);
}

TEST(AutolinkingCmarkTestCase, NanoRepo)
{
    // GIVEN
    // autolink 1 file with 1 section
    string repositoryPath{"/lib/test/resources/autolinking-nano-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-act-anr.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(1, mind.remind().getOutlinesCount());

    // WHEN
    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::CmarkAhoCorasickBlockAutolinkingPreprocessor autolinker{mind};
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    string autolinkedMd{};
    // multiple iterations to check that there are no memory leaks
    for(int i=0; i<1; i++) {
        autolinker.process(n->getDescription(), autolinkedMd);
    }

    // THEN
    cout << "= BEGIN AUTO MD =" << endl << autolinkedMd << endl << "= END AUTO MD =" << endl;
    ASSERT_STREQ("Text of [AAA](mindforger://links.mindforger.com/AAA).", autolinkedMd.c_str());
}

TEST(AutolinkingCmarkTestCase, MicroRepo)
{
    // GIVEN
    // autolink 1 file with 1 section
    string repositoryPath{"/lib/test/resources/autolinking-micro-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-act-anr.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(1, mind.remind().getOutlinesCount());

    // WHEN
    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::CmarkAhoCorasickBlockAutolinkingPreprocessor autolinker{mind};
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    string autolinkedMd{};
    autolinker.process(n->getDescription(), autolinkedMd);

    // THEN
    cout << "= BEGIN AUTO MD =" << endl << autolinkedMd.c_str() << endl << "= END AUTO MD =" << endl;
    ASSERT_STREQ("Text of [AAA](mindforger://links.mindforger.com/AAA).", autolinkedMd.c_str());
}

TEST(AutolinkingCmarkTestCase, BasicRepo)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-act-br.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(3, mind.remind().getOutlinesCount());
    m8r::CmarkAhoCorasickBlockAutolinkingPreprocessor autolinker{mind};

    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    string autolinkedMd{};
    autolinker.process(n->getDescription(), autolinkedMd);
    cout << "= BEGIN AUTO MD =" << endl << autolinkedMd << endl << "= END AUTO MD =" << endl;
}

/**
 * @brief Unit test which autolinks copy of production MF repository
 *
 * Autolinking is performed if MF repo exists in predefined location.
 */
TEST(AutolinkingCmarkTestCase, Monster)
{
    string repositoryPath{"/tmp/mindforger-unit-test/monster-repository"};
    if(m8r::isDirectoryOrFileExists(repositoryPath.c_str())) {
        m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
        m8r::Configuration& config = m8r::Configuration::getInstance();
        config.clear();
        config.setConfigFilePath("/tmp/cfg-act-monster.md");
        config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)), repositoryConfigRepresentation);
        m8r::Mind mind(config);
        mind.learn();
        mind.think().get();
        cout << endl << "Statistics:";
        cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
        cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
        ASSERT_LE(370, mind.remind().getOutlinesCount());
        m8r::CmarkAhoCorasickBlockAutolinkingPreprocessor autolinker{mind};

        cout << endl << endl << "Testing MONSTER autolinking:" << endl;
        for(m8r::Outline*const& o:mind.remind().getOutlines()) {
            for(m8r::Note*const& n:o->getNotes()) {
                cout << "  " << n->getName() << endl;
                string autolinkedMd{};
                autolinker.process(n->getDescription(), autolinkedMd);
                cout << "    DONE: " << autolinkedMd.size() << endl;
            }
        }
    } else {
        cout << "Skipping MONSTER autolinking: MF repository doesn't exist" << repositoryPath << endl;
    }
}

#endif // MF_MD_2_HTML_CMARK
#endif // !WINDOWS

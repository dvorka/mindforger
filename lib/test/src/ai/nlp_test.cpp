/*
 nlp_test.cpp     MindForger application test

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>

#include "../../../src/config/configuration.h"
#include "../../../src/mind/mind.h"
#include "../../../src/mind/ai/ai.h"
#include "../../../src/mind/ai/nlp/string_char_provider.h"
#include "../../../src/mind/ai/nlp/note_char_provider.h"
#include "../../../src/mind/ai/nlp/markdown_tokenizer.h"
#include "../../../src/mind/ai/nlp/lexicon.h"
#include "../../../src/mind/ai/nlp/word_frequency_list.h"
#include "../../../src/mind/ai/nlp/bag_of_words.h"
#include "../../../src/mind/ai/association_assessment_model.h"

#include <gtest/gtest.h>

extern char* getMindforgerGitHomePath();

using namespace std;

TEST(AiNlpTestCase, Lexicon)
{
    m8r::Lexicon lexicon{};

    lexicon.add("a5");
    ASSERT_EQ(1, lexicon.size());
    ASSERT_EQ(1, lexicon.get("a5")->frequency);

    lexicon.add("a5");
    ASSERT_EQ(1, lexicon.size());
    ASSERT_EQ(2, lexicon.get("a5")->frequency);

    string s{"a5"};
    lexicon.add(s);
    ASSERT_EQ(1, lexicon.size());
    ASSERT_EQ(3, lexicon.get(s)->frequency);
    lexicon.add(&s);
    ASSERT_EQ(1, lexicon.size());
    ASSERT_EQ(4, lexicon.get(&s)->frequency);

    // adding more words for better weight calculation 5/3/2
    lexicon.add("a5");
    lexicon.add("a3");
    lexicon.add("a3");
    lexicon.add("a3");
    lexicon.add("a2");
    lexicon.add("a2");

    // weights
    lexicon.recalculateWeights();
    lexicon.print();

    ASSERT_FLOAT_EQ(0.01, lexicon.get("a5")->weight);
    ASSERT_FLOAT_EQ(0.4, lexicon.get("a3")->weight);
    ASSERT_FLOAT_EQ(0.6, lexicon.get("a2")->weight);

    // TODO weights: increase scale

}

TEST(AiNlpTestCase, Outline)
{
    // FOO outline
    m8r::OutlineType oType{m8r::OutlineType::KeyOutline(),nullptr,m8r::Color::RED()};
    m8r::Outline o{&oType};
    o.setName("Outline Name");
    // FOO outline as MD
    string markdown;
    markdown.assign(
        "Outline Name\n"
        "========\n"
        "O text begin [LINK-LABEL](http://link-1.com) text end.\n"
        "\n"
        "First Section\n"
        "-------------\n"
        "N1 text `N1 inline code` N1 text end.\n"
        "\n"
        "## Second Section\n"
        "S2 intro text:\n"
        "```\n"
        "N2 text codeblock.\n"
        "```\n"
        "\n"
        "Note 3\n"
        "-------------\n"
        "N2 text.\n"
        "\n");

    /*
     * STEP: parse O/N to Lexicon and BoW(matrix Things x frequencies)
     */

    m8r::Lexicon lexicon{};
    m8r::Trie wordBlaclist{};
    wordBlaclist.addWord("text");
    m8r::MarkdownTokenizer tokenizer{lexicon, wordBlaclist};
    m8r::StringCharProvider chars{markdown};
    m8r::WordFrequencyList* wfl = new m8r::WordFrequencyList{&lexicon};
    cout << "Tokenizing MD string to word frequency list..." << endl;
    tokenizer.tokenize(chars, *wfl);
    wfl->sort();

    // assert wfl
    wfl->print();
    ASSERT_EQ(19, wfl->size());
    // assert lexicon
    lexicon.print();
    ASSERT_EQ(19, lexicon.size());

    /*
     * STEP: build BoW i.e. matrix of Things x frequencies
     */

    m8r::BagOfWords bow{};
    bow.add(&o, wfl);

    bow.print();
    ASSERT_EQ(1, bow.size());
}

TEST(AiNlpTestCase, Tokenizer)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Mind mind(config);
    mind.think();
    cout << "\nStatistics:";
    cout << "\n  Outlines: " << mind.remind().getOutlinesCount();
    cout << "\n  Bytes   : " << mind.remind().getOutlineMarkdownsSize();

    ASSERT_EQ(3, mind.remind().getOutlinesCount());

    // test N narrowing to string using char provider
    cout << endl << endl << "Testing M NARROWING using tokenizer:" << endl;
    m8r::Note* n=mind.remind().getOutlines()[0]->getNotes()[0];
    cout << "- BEGIN original ---" << endl << n->getName() << endl << n->getDescriptionAsString() << endl
         << "- END original >>>>> BEGIN char stream ---" << endl;
    m8r::NoteCharProvider nNarrower{n};
    string narrowed{};
    while(nNarrower.hasNext()) {
        narrowed += nNarrower.next();
    }
    cout << narrowed << endl << "- END char stream --" << endl;
    ASSERT_EQ(182, narrowed.size());
}

TEST(AiNlpTestCase, Repository)
{
    string repositoryPath{"/lib/test/resources/universe-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Mind mind(config);
    mind.think();
    cout << "Statistics:" << endl
    << "  Outlines: " << mind.remind().getOutlinesCount() << endl
    << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize() << endl;

    ASSERT_LE(1, mind.remind().getOutlinesCount());

    /*
     * Tokenize repository > make AI to think > find the most similar Notes pair
     */

    mind.dream();

    // get the best associations of N
    // TODO find Note by name and use it...
    // TODO consider AI repository for assoc experiments
    m8r::Note* n=mind.remind().getOutlines()[0]->getNotes()[0];
    std::vector<m8r::Note*> lb{};
    mind.getAssociationsLeaderboard(n, lb);
    m8r::Ai::print(n,lb);
}

/*
 ai_aa_bm25_test.cpp     MindForger application test

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <string>
#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include "../../../src/mind/mind.h"
#include "../../../src/mind/ai/ai_aa_bm25.h"

using namespace std;

extern char* getMindforgerGitHomePath();

/*
 * Helpers
 */

static m8r::Mind* learnAaBm25Repository(const string& configFileName)
{
    string repositoryPath{"/lib/test/resources/aa-bm25-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/" + configFileName);
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)),
        repositoryConfigRepresentation
    );
    config.setAaAlgorithm(m8r::Configuration::AssociationAssessmentAlgorithm::BM25);

    m8r::Mind* mind = new m8r::Mind(config);
    mind->learn();
    mind->think().get();
    return mind;
}

static m8r::Note* findNote(m8r::Mind& mind, const string& name)
{
    vector<m8r::Note*> notes{};
    mind.remind().getAllNotes(notes);
    for(m8r::Note* n: notes) {
        if(n->getName() == name) {
            return n;
        }
    }
    return nullptr;
}

static m8r::Outline* findOutline(m8r::Mind& mind, const string& name)
{
    for(m8r::Outline* o: mind.remind().getOutlines()) {
        if(o->getName() == name) {
            return o;
        }
    }
    return nullptr;
}

static int rankOf(const vector<pair<m8r::Note*,float>>& associations, const string& name)
{
    for(size_t i=0; i<associations.size(); i++) {
        if(associations[i].first->getName() == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

static void printAssociations(const string& query, const vector<pair<m8r::Note*,float>>& associations)
{
    cout << "Associations of '" << query << "' [" << associations.size() << "]:" << endl;
    for(size_t i=0; i<associations.size(); i++) {
        cout << "  #" << i << " " << associations[i].first->getName() << " ~ " << associations[i].second << endl;
    }
}

/*
 * Tests
 */

TEST(AiAaBm25TestCase, SelectQueryTerms)
{
    // GIVEN
    m8r::CommonWordsBlacklist commonWords{};
    vector<string> terms{};
    vector<string> commonOnlyTerms{};

    // WHEN
    m8r::AiAaBm25::selectQueryTerms(
        "<a name=\"x\"></a>The Art of Pointer Arithmetic and pointer SAFETY", commonWords, terms);
    m8r::AiAaBm25::selectQueryTerms("The", commonWords, commonOnlyTerms);

    // THEN
    for(const string& t: terms) {
        cout << "term '" << t << "'" << endl;
    }
    // unique, lower case, w/o common words, the longest first
    ASSERT_EQ(m8r::AiAaBm25::MAX_QUERY_TERMS, terms.size());
    ASSERT_EQ("arithmetic", terms[0]);
    ASSERT_EQ("pointer", terms[1]);
    ASSERT_EQ("safety", terms[2]);
    // common words only query falls back to the longest word
    ASSERT_EQ(1u, commonOnlyTerms.size());
    ASSERT_EQ("the", commonOnlyTerms[0]);
}

TEST(AiAaBm25TestCase, WordsReturnOnlyMatchingNotes)
{
    // GIVEN: multi-word query whose phrase is nowhere (was a fallback to "" terms matching ALL Ns)
    unique_ptr<m8r::Mind> mind{learnAaBm25Repository("cfg-aabm-womn.md")};
    m8r::AssociatedNotes associations{m8r::WORD, "pointer xylophone", nullptr};

    // WHEN
    bool found = mind->getAssociatedNotes(associations).get();

    // THEN
    printAssociations("pointer xylophone", *associations.getAssociations());
    ASSERT_TRUE(found);
    ASSERT_EQ(2u, associations.getAssociations()->size());
    ASSERT_EQ("Pointer arithmetic", associations.getAssociations()->at(0).first->getName());
    ASSERT_EQ("Smart pointers", associations.getAssociations()->at(1).first->getName());
    ASSERT_FLOAT_EQ(1.f, associations.getAssociations()->at(0).second);
}

TEST(AiAaBm25TestCase, WordsMatchWholeWordsOnly)
{
    // GIVEN
    unique_ptr<m8r::Mind> mind{learnAaBm25Repository("cfg-aabm-wmwwo.md")};
    m8r::AssociatedNotes associations{m8r::WORD, "art", nullptr};

    // WHEN
    bool found = mind->getAssociatedNotes(associations).get();

    // THEN: "start" and "party" are NOT "art"
    printAssociations("art", *associations.getAssociations());
    ASSERT_TRUE(found);
    ASSERT_EQ(1u, associations.getAssociations()->size());
    ASSERT_EQ("Art class", associations.getAssociations()->at(0).first->getName());
    ASSERT_EQ(-1, rankOf(*associations.getAssociations(), "Party"));
}

TEST(AiAaBm25TestCase, RareTermOutranksCommonTerm)
{
    // GIVEN: "kiwi" is in 1 N, "fruit" in 4 Ns
    unique_ptr<m8r::Mind> mind{learnAaBm25Repository("cfg-aabm-rtoct.md")};
    m8r::AssociatedNotes associations{m8r::WORD, "fruit kiwi", nullptr};

    // WHEN
    bool found = mind->getAssociatedNotes(associations).get();

    // THEN
    printAssociations("fruit kiwi", *associations.getAssociations());
    ASSERT_TRUE(found);
    ASSERT_EQ(5u, associations.getAssociations()->size());
    ASSERT_EQ("Alpha", associations.getAssociations()->at(0).first->getName());
    ASSERT_LT(associations.getAssociations()->at(1).second, associations.getAssociations()->at(0).second);
}

TEST(AiAaBm25TestCase, TitleHitOutranksLongDescriptionMention)
{
    // GIVEN
    unique_ptr<m8r::Mind> mind{learnAaBm25Repository("cfg-aabm-thold.md")};
    m8r::AssociatedNotes associations{m8r::WORD, "compost", nullptr};

    // WHEN
    bool found = mind->getAssociatedNotes(associations).get();

    // THEN
    printAssociations("compost", *associations.getAssociations());
    ASSERT_TRUE(found);
    ASSERT_EQ(2u, associations.getAssociations()->size());
    ASSERT_EQ("Compost", associations.getAssociations()->at(0).first->getName());
    ASSERT_EQ("Long diary", associations.getAssociations()->at(1).first->getName());
}

TEST(AiAaBm25TestCase, NoteAssociationsUseSharedTags)
{
    // GIVEN: "Seeds catalogue" shares the "vegetables" tag w/ "Tomato growing", but no words
    unique_ptr<m8r::Mind> mind{learnAaBm25Repository("cfg-aabm-naust.md")};
    m8r::Note* tomato = findNote(*mind, "Tomato growing");
    ASSERT_NE(nullptr, tomato);
    m8r::AssociatedNotes noteAssociations{m8r::NOTE, tomato};
    m8r::AssociatedNotes wordAssociations{m8r::WORD, "tomato growing", tomato};

    // WHEN
    bool noteFound = mind->getAssociatedNotes(noteAssociations).get();
    bool wordFound = mind->getAssociatedNotes(wordAssociations).get();

    // THEN
    printAssociations("N: Tomato growing", *noteAssociations.getAssociations());
    printAssociations("W: tomato growing", *wordAssociations.getAssociations());
    ASSERT_TRUE(noteFound);
    ASSERT_EQ(0, rankOf(*noteAssociations.getAssociations(), "Seeds catalogue"));
    // self is never associated
    ASSERT_EQ(-1, rankOf(*noteAssociations.getAssociations(), "Tomato growing"));
    // tags are used for O/N associations only - not when writing (words)
    ASSERT_FALSE(wordFound);
    ASSERT_EQ(0u, wordAssociations.getAssociations()->size());
}

TEST(AiAaBm25TestCase, OutlineAssociationsSkipSelfAndSiblings)
{
    // GIVEN
    unique_ptr<m8r::Mind> mind{learnAaBm25Repository("cfg-aabm-oasss.md")};
    m8r::Outline* gardening = findOutline(*mind, "Gardening");
    ASSERT_NE(nullptr, gardening);
    m8r::AssociatedNotes associations{m8r::OUTLINE, gardening};

    // WHEN
    bool found = mind->getAssociatedNotes(associations).get();

    // THEN: siblings don't match "gardening" and O descriptor (self) is skipped
    printAssociations("O: Gardening", *associations.getAssociations());
    ASSERT_TRUE(found);
    ASSERT_EQ(1u, associations.getAssociations()->size());
    ASSERT_EQ("Gardening in Japan", associations.getAssociations()->at(0).first->getName());
}

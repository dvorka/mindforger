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
    m8r::MarkdownTokenizer tokenizer{lexicon};
    m8r::StringCharProvider chars{markdown};
    m8r::WordFrequencyList* wfl = new m8r::WordFrequencyList{};
    cout << "Tokenizing MD string to word frequency list..." << endl;
    tokenizer.tokenize(chars, *wfl);
    wfl->print();

    // assert wfl
    ASSERT_EQ(20, wfl->size());
    // assert lexicon
    lexicon.print();
    ASSERT_EQ(20, lexicon.size());

    /*
     * STEP: build BoW i.e. matrix of Things x frequencies
     */

    m8r::BagOfWords bow{};
    bow.add(&o, wfl);

    ASSERT_EQ(1, bow.size());
}

/*
TEST(AiBowTestCase, Repository)
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

    ASSERT_EQ(mind.remind().getOutlinesCount(),3);

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

    // test N to BoW
    m8r::MarkdownTokenizer tokenizer{};
    m8r::NoteCharProvider chars{n};
    m8r::BagOfWords bow{};
    cout << "Tokenizing MD string to Bag of Words (BoW)..." << endl;
    tokenizer.tokenize(chars, bow);
    cout << "BoW[" << bow.size() << "]:" << endl;
    for(auto s:bow.iterable()) {
        cout << "  " << (s.first) << " [" << s.second << "] " << endl;
    }

    ASSERT_EQ(19, bow.size());







    *
     * STEP: parse O/N to Lexicon and BoW(matrix Things x frequencies)
     *

    m8r::Ai ai{};

    *
     * STEP: create feature(N1,N2) for notes N1 and N2
     *

    m8r::Note* n1;
    m8r::Note* n2;

    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    // IMPLEMENT THIS :)
    m8r::AssociationAssessmentNotesFeature& nnFeature
        = createAaFeature(n1, n2);

    // TODO ...

    *
     * STEP: pass feature(N1,N2) to NN to determine whether they ARE/AREN'T associated
     *

    m8r::AssociationAssessmentModel nn{};
    nn.predict(nnFeature);

    // TODO ...





    // Having BoW next steps are:
    //   - create similarityEmbedding ~ for every Ni vector w/ [[bow],stemmed tags,relationship,type]
    //   - for every Ni and Nj create notesSimilarityFeature ~ smart unit using similarityEmbedding[Ni] and se[Nj]
    //     [# matching words in total, avg of at most words TF-IDF, # matching tags, have relationship, # relationship to same target, type matches]
    //   - now ask NN(notesSimilarityFeature[Ni,Nj]) = {1/0} ... similar/not similar (associated each other/don't ...)

    // Create model - consult it w/ A.Ng notes

    // TODO: class AssociatedNotesDetector*Model* : public class NN { ... }
}

*/

// TODO TEST ... O to OutlineBoW


/*
 bow_test.cpp     MindForger application test

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
#include "../../../src/mind/ai/nlp/string_char_provider.h"
#include "../../../src/mind/ai/nlp/note_char_provider.h"
#include "../../../src/mind/ai/nlp/markdown_tokenizer.h"

#include <gtest/gtest.h>

extern char* getMindforgerGitHomePath();

using namespace std;

TEST(AiBowTestCase, TokenizeO)
{
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

    m8r::MarkdownTokenizer tokenizer{};
    m8r::StringCharProvider chars{markdown};
    m8r::BagOfWords bow{};
    cout << "Tokenizing MD string to Bag of Words (BoW)..." << endl;
    tokenizer.tokenize(chars, bow);
    cout << "BoW[" << bow.size() << "]:" << endl;
    for(auto s:bow.iterable()) {
        cout << "  " << (s.first) << " [" << s.second << "] " << endl;
    }

    ASSERT_EQ(9, bow.size());

    // Having BoW next steps are:
    //   - create similarityEmbedding ~ for every Ni vector w/ [[bow],stemmed tags,relationship,type]
    //   - for every Ni and Nj create notesSimilarityFeature ~ smart unit using similarityEmbedding[Ni] and se[Nj]
    //     [# matching words in total, avg of at most words TF-IDF, # matching tags, have relationship, # relationship to same target, type matches]
    //   - now ask NN(notesSimilarityFeature[Ni,Nj]) = {1/0} ... similar/not similar (associated each other/don't ...)

    // Create model - consult it w/ A.Ng notes

    // TODO: class AssociatedNotesDetector*Model* : public class NN { ... }
}


TEST(AiBowTestCase, TokenizeRepository)
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
}

// TODO TEST ... O to OutlineBoW


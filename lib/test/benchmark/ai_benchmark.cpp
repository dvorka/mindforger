/*
 ai_benchmark.cpp     MindForger markdown test

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

#include <chrono>
#include <string>
#include <iostream>

#include <gtest/gtest.h>

#include "../../src/mind/mind.h"

using namespace std;
using namespace m8r;

extern char* getMindforgerGitHomePath();

/*
 * Performance improvements ideas:
 *   - IF |notes|>1000 THEN split aaMatrix rows to launch(CPU-1) tasks > start thread for each task > join threads (thread x down ~ 80% down)
 *   - compute above aaMatrix diagonal values only (2x faster ~ 50% down)
 *   - use trie instead of map in lexicon (10% faster ~ 10% down)
 *   - heuristics:
 *     - codereview calculateSimilarityByWords() > WordFrequencyList::evalUnion/Intersection must be MUCH faster
 *     - skip Ns w/ empty description
 *
 * Conclusion:
 *   - can be made 10x faster
 *   - 6' (5k Ns) to 36s ... which is still slow
 */
/*
 * Measurements
 *
 * 2018/03/31 ...  46s (<1') , 5.000 Ns, 12.560.072 rankings in aaMatrix (5k^2 / 2) ... added stemmer (less words), only 10 relevant words compared
 * 2018/03/31 ... 129s (2')  , 5.000 Ns, 12.560.072 rankings in aaMatrix (5k^2 / 2) ... 1/2 of matrix, simplified vectors weight computation
 * 2018/03/31 ... 338s (5'30), 5.000 Ns, 12.560.072 rankings in aaMatrix (5k^2 / 2)
 */
TEST(AiBenchmark, DISABLED_AaMatrix)
{
    string repositoryPath{"/lib/test/resources/benchmark-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-aib-am.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.think();
    cout << "Statistics:" << endl
    << "  Outlines: " << mind.remind().getOutlinesCount() << endl
    << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize() << endl;

    ASSERT_LE(1, mind.remind().getOutlinesCount());

    /*
     * Tokenize repository > make AI to think > find the most similar Notes pair
     */

    auto beginDream = chrono::high_resolution_clock::now();

    // TODO to be rewritten mind.dream();

    auto endDream = chrono::high_resolution_clock::now();
    MF_DEBUG(endl << "Dream DONE in " << chrono::duration_cast<chrono::microseconds>(endDream-beginDream).count()/1000.0 << "ms" << endl);

    // get the best associations of N
    m8r::Note* n=mind.remind().getOutlines()[0]->getNotes()[0];
    UNUSED_ARG(n);
    std::vector<std::pair<m8r::Note*,float>> lb{};
    // TODO to be rewritten mind.getAssociationsLeaderboard(n, lb);
    // TODO to be rewritten m8r::Ai::print(n,lb);
}

/*
 * Associations assessment benchmark: benchmark-repository ~ 5.000 Ns, 100 N + 2 O + 10 word queries, MF_DEBUG enabled
 *
 * 2026/09/25 ... BM25         ~ 3.5ms/query ... single scan, whole word matching, BM25 ranking w/ tags
 * 2026/09/25 ... weighted FTS ~ 7.7ms/query ... phrase substring scan + fallback scan (first 3 words)
 */
static void benchmarkAssociations(m8r::Configuration::AssociationAssessmentAlgorithm algorithm, const string& name)
{
    // GIVEN
    string repositoryPath{"/lib/test/resources/benchmark-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-aib-aa.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)), repositoryConfigRepresentation);
    config.setAaAlgorithm(algorithm);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    vector<m8r::Note*> notes{};
    mind.remind().getAllNotes(notes);
    ASSERT_LE(100, notes.size());
    const size_t step = notes.size()/100;
    const vector<string> words{"pointer", "exception", "const", "template", "lifetime", "Resource Acquisition", "RAII", "owner", "thread", "interface"};

    // WHEN
    size_t queries = 0;
    size_t failures = 0;
    auto begin = chrono::high_resolution_clock::now();
    for(size_t i=0; i<notes.size(); i+=step) {
        m8r::AssociatedNotes associations{m8r::NOTE, notes[i]};
        mind.getAssociatedNotes(associations);
        queries++;
        if(associations.getAssociations()->size() > 10) failures++;
        for(auto& a:*associations.getAssociations()) {
            if(a.first == notes[i]) failures++;
        }
    }
    for(m8r::Outline* o:mind.remind().getOutlines()) {
        m8r::AssociatedNotes associations{m8r::OUTLINE, o};
        mind.getAssociatedNotes(associations);
        queries++;
        if(associations.getAssociations()->size() > 10) failures++;
    }
    for(const string& w:words) {
        m8r::AssociatedNotes associations{m8r::WORD, w, notes[0]};
        mind.getAssociatedNotes(associations);
        queries++;
        if(associations.getAssociations()->size() > 10) failures++;
    }
    auto end = chrono::high_resolution_clock::now();

    // THEN
    double ms = chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0;
    cout << "AA " << name << " benchmark: " << notes.size() << " Ns, "
         << queries << " queries in " << ms << "ms ~ " << ms/queries << "ms/query" << endl;
    EXPECT_EQ(0, failures);
}

TEST(AiBenchmark, AaBm25)
{
    benchmarkAssociations(m8r::Configuration::AssociationAssessmentAlgorithm::BM25, "BM25");
}

TEST(AiBenchmark, AaWeightedFts)
{
    benchmarkAssociations(m8r::Configuration::AssociationAssessmentAlgorithm::WEIGHTED_FTS, "weighted FTS");
}

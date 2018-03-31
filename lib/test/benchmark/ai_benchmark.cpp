/*
 ai_benchmark.cpp     MindForger markdown test

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
 * 2018/03/31 ... 338s, 5.000 Ns, 12.560.072 rankings in aaMatrix (5k^2 / 2)
 *
 *
 */
TEST(AiBenchmark, AaMatrix)
{
    string repositoryPath{"/lib/test/resources/benchmark-repository"};
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

    auto beginDream = chrono::high_resolution_clock::now();

    mind.dream();

    auto endDream = chrono::high_resolution_clock::now();
    MF_DEBUG(endl << "Dream DONE in " << chrono::duration_cast<chrono::microseconds>(endDream-beginDream).count()/1000.0 << "ms" << endl);

    // get the best associations of N
    m8r::Note* n=mind.remind().getOutlines()[0]->getNotes()[0];
    std::vector<m8r::Note*> lb{};
    mind.getAssociationsLeaderboard(n, lb);
    m8r::Ai::print(n,lb);
}

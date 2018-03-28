/*
 association_assessment_notes_feature.h     MindForger thinking notebook

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
#ifndef M8R_ASSOCIATION_ASSESSMENT_NOTES_FEATURE_H
#define M8R_ASSOCIATION_ASSESSMENT_NOTES_FEATURE_H

#include <map>

#include "../../model/note.h"

namespace m8r {

/**
 * @brief Notes Association Assessment NN input feature.
 *
 * Being associated is *symmetric* relation.
 */
class AssociationAssessmentNotesFeature
{
public:
    // 30%
    static constexpr int IDX_HAVE_MUTUAL_REL = 0;
    // 10% (x0% remains)
    const int IDX_TYPE_MATCHES = 4;
    // 20% (x0% remains)
    const int IDX_SIMILARITY_BY_TAGS = 2;

    // x0% (x0% remains)
    const int IDX_SIMILARITY_BY_TITLES= 2;
    // x0% (x0% remains)
    const int IDX_SIMILARITY_BY_RELEVANT_WORDS_IN_DESCS = 2;
    // x0% (x0% remains)
    const int IDX_SIMILARITY_BY_TITLES_IN_DESCS = 2;
    // x0% (x0% remains)
    const int IDX_SIMILARITY_BY_RELS_W_SAME_TARGETS = 3;

private:
    std::pair<m8r::Note*,m8r::Note*> notes;
    float features[7];

public:
    explicit AssociationAssessmentNotesFeature();
    AssociationAssessmentNotesFeature(const AssociationAssessmentNotesFeature&) = delete;
    AssociationAssessmentNotesFeature(const AssociationAssessmentNotesFeature&&) = delete;
    AssociationAssessmentNotesFeature &operator=(const AssociationAssessmentNotesFeature&) = delete;
    AssociationAssessmentNotesFeature &operator=(const AssociationAssessmentNotesFeature&&) = delete;
    ~AssociationAssessmentNotesFeature();

    void setHaveMutualRel(bool haveRel) {
        features[IDX_HAVE_MUTUAL_REL] = haveRel?1.:0.;
    }

    /**
     * @brief Set similarity computed just by tags.
     *
     * Consider the following examples:
     *  [important, ai, ml] and [ai] -> ~80% related
     *  [important, ai], [important, running] -> 5% related
     *  [important, ai], [cool, ai] -> ~100% related
     *
     * Hints:
     *  - distinquish VALUABLE tags (ai) and invaluable tags (cool)
     *  - give tags WEIGHTS by their value
     *  - formula
     *    - unionTags(N1,N2) = [split 100% among all tags found in N1 or N2 by tag value]
     *    - intersectionTags(N1,N2) = sum(% of tags in both Ns - relevant bring more, worthless less)
     *
     * @param similarityByTags percentage of similarity by tags [0,1]
     *
     * IMPROVE:
     *  - semantic word vector make solve tag importance problem (wordVec)
     *  - TF-IDF of tags
     */
    void setSimilarityByTags(float similarityByTags) {
        features[IDX_SIMILARITY_BY_TAGS] = similarityByTags;
    }

    void setTypeMatches(bool typeMatches) {
        features[IDX_HAVE_MUTUAL_REL] = typeMatches?1.:0.;
    }

    /**
     * @brief Set similarity computed by words in title.
     */

    // TODO N1 title occurs in body of N2, N2 in N1, mutual title containment ... means related

    /**
     * @brief Set similarity computed by words in description.
     */
    void setSimilarityByWords(float similarityByTags) {
        features[IDX_SIMILARITY_BY_RELEVANT_WORDS_IN_DESCS] = similarityByTags;
    }

    /**
     * @brief Metric that estimates similarity w/o NN i.e. it's an exact formula I proposed w/ range=[0,1]
     *
     * I start from 100% i.e. what makes Ns to be associations. I alocate portions of 100% to
     * each similarity aspect A: 10%*A1+40%*A2+...+5%*AN=100% (<=> if As==1).
     */
    float areNotesAssociatedMetric() {
        return (
            features[IDX_HAVE_MUTUAL_REL]*30. +
            features[IDX_HAVE_MUTUAL_REL]*30.
          )/100.;
    }
};

}
#endif // M8R_ASSOCIATION_ASSESSMENT_NOTES_FEATURE_H

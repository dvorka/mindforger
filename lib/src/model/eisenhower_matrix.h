/*
 eisenhower_matrix.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_EISENHOWER_MATRIX_H
#define M8R_EISENHOWER_MATRIX_H

#include "organizer.h"

namespace m8r {

/**
 * @brief Eisenhower Matrix as Outlines and Notes sorter.
 */
class EisenhowerMatrix : public Organizer
{
public:
    enum SortBy {
        IMPORTANCE,
        URGENCY
    };

    static constexpr const auto CONFIG_VALUE_SORT_BY_I = "importance";
    static constexpr const auto CONFIG_VALUE_SORT_BY_U = "urgency";

    static constexpr const auto KEY_EISENHOWER_MATRIX =
        "/m1ndf0rg3r/organizers/eisenhower-matrix";

    static std::string createEisenhowerMatrixKey();

    static EisenhowerMatrix* createEisenhowerMatrixOrganizer();
    static bool isEisenhowMatrixOrganizer(Organizer* o);

public:
    // values: importance, urgency; default: importance
    int sortBy;

public:
    explicit EisenhowerMatrix(const std::string& name);
    explicit EisenhowerMatrix(const EisenhowerMatrix&);
    explicit EisenhowerMatrix(const Organizer&);
    EisenhowerMatrix(const EisenhowerMatrix&&) = delete;
    EisenhowerMatrix& operator=(const EisenhowerMatrix&) = delete;
    EisenhowerMatrix& operator=(const EisenhowerMatrix&&) = delete;
    ~EisenhowerMatrix();

    int getSortBy() const { return this->sortBy; }
    const std::string getSortByAsStr();
    void setSortBy(int sortBy) { this->sortBy = sortBy; }
};

}
#endif // M8R_EISENHOWER_MATRIX_H

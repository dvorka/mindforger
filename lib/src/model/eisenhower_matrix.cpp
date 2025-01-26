/*
 eisenhower_matrix.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "eisenhower_matrix.h"

using namespace std;

namespace m8r {

string EisenhowerMatrix::createEisenhowerMatrixKey() {
    return Organizer::createOrganizerKey(
        set<string>{}, "repository", Thing::getNextKey(), "/", "eisehnower-matrix"
    );
}

EisenhowerMatrix::EisenhowerMatrix(const std::string& name)
    : Organizer{name, Organizer::OrganizerType::EISENHOWER_MATRIX},
      sortBy{EisenhowerMatrix::SortBy::IMPORTANCE}
{
    if(!this->name.length()) {
        this->name = "Eisenhower Matrix";
    }
}

EisenhowerMatrix::EisenhowerMatrix(const EisenhowerMatrix& em):
    Organizer{em},
    sortBy{em.getSortBy()}
{
}

EisenhowerMatrix::EisenhowerMatrix(const Organizer& o):
    Organizer{o}
{
    this->organizerType=OrganizerType::EISENHOWER_MATRIX;
}

EisenhowerMatrix::~EisenhowerMatrix()
{
}

const string EisenhowerMatrix::getSortByAsStr() {
    switch(this->sortBy) {
    case EisenhowerMatrix::SortBy::URGENCY:
        return EisenhowerMatrix::CONFIG_VALUE_SORT_BY_U;
    case EisenhowerMatrix::SortBy::IMPORTANCE:
    default:
        return EisenhowerMatrix::CONFIG_VALUE_SORT_BY_I;
    }
}

EisenhowerMatrix* EisenhowerMatrix::createEisenhowerMatrixOrganizer() {
    EisenhowerMatrix* eisenhowerMatrixOrganizer = new EisenhowerMatrix(
        "Eisenhower Matrix"
    );
    eisenhowerMatrixOrganizer->setKey(EisenhowerMatrix::KEY_EISENHOWER_MATRIX);

    eisenhowerMatrixOrganizer->setUpperRightTag("important & urgent");
    eisenhowerMatrixOrganizer->setLowerRightTag("important");
    eisenhowerMatrixOrganizer->setUpperLeftTag("urgent");
    eisenhowerMatrixOrganizer->setLowerLeftTag(".");

    return eisenhowerMatrixOrganizer;
}

bool EisenhowerMatrix::isEisenhowMatrixOrganizer(Organizer* o)
{
    if(o && o->getKey() == KEY_EISENHOWER_MATRIX) {
        return true;
    }
    return false;
}

} // m8r namespace

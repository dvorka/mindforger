/*
 dikw_pyramid.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_DIKW_PYRAMID_H
#define M8R_DIKW_PYRAMID_H

namespace m8r {

/**
 * @brief DIKW pyramid.
 *
 * DIKW pyramid = Data > Information > Knowledge > Wishdom.
 *
 * Typically information is defined in terms of data,
 * knowledge in terms of information, and wisdom in
 * terms of knowledge.
 *
 * @see https://en.wikipedia.org/wiki/DIKW_pyramid
 */
class DikwPyramid
{
public:
    explicit DikwPyramid();
    DikwPyramid(const DikwPyramid&) = delete;
    DikwPyramid(const DikwPyramid&&) = delete;
    DikwPyramid &operator=(const DikwPyramid&) = delete;
    DikwPyramid &operator=(const DikwPyramid&&) = delete;
    ~DikwPyramid();
};

}
#endif // M8R_DIKW_PYRAMID_H

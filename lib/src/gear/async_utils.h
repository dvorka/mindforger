/*
 async_utils.h     MindForger thinking notebook

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
#ifndef M8R_ASYNC_UTILS_H
#define M8R_ASYNC_UTILS_H

#include <cmath>

namespace m8r {

/**
 * @brief Progress callback context.
 */
class ProgressCallbackCtx
{
public:
    explicit ProgressCallbackCtx();
    ProgressCallbackCtx(const ProgressCallbackCtx&) = delete;
    ProgressCallbackCtx(const ProgressCallbackCtx&&) = delete;
    ProgressCallbackCtx& operator=(const ProgressCallbackCtx&) = delete;
    ProgressCallbackCtx& operator=(const ProgressCallbackCtx&&) = delete;
    virtual ~ProgressCallbackCtx();

    /**
     * @brief Normalize progress to percents.
     * @param progress  progress in interval [0.0, 1.0].
     * @return progress as percent in the interval [0, 100]
     */
    int normalizeProgressToPercent(float progress);

    virtual void updateProgress(float progress) = 0;
};

}
#endif // M8R_ASYNC_UTILS_H

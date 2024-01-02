/*
 limbo.h     MindForger thinking notebook

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
#ifndef M8R_LIMBO_H
#define M8R_LIMBO_H

#include <vector>

#include "../model/outline.h"
#include "../config/configuration.h"

namespace m8r {

/**
 * @brief The Limbo.
 *
 * Limbo is where deleted Os, Ns and other MF things may last until they are purged:
 *
 * - deleted O is moved to Limbo
 *   > MF repository: ${REPOSITORY}/limbo/ *.md
 *   > MD repository: ${SYSTEM_TMP}/mindforger/limbo/ *.md
 * - deleted N is moved to Limbo O in Limbo:
 *   > MF repository: ${REPOSITORY}/limbo/LIMBO-for-notes.md
 *   > MD repository: ${SYSTEM_TMP}/mindforger/limbo-<UUID>/LIMBO-for-notes.md
 *   > MD file: ${SYSTEM_TMP}/mindforger/limbo-<UUID>/LIMBO-for-notes.md
 *
 * Limbo directory and/or limbo Ns O can be opened from MindForger UI, but only
 * as a normal file/directory i.e. using Qt desktop services (it's not managed
 * as regular O/Markdown file. Markdown Os/Ns limbo has UUID associated w/ single
 * MF session in order to avoid blending/clashes of different repositories/files.
 * MF session is reset on every MD/MD directory load.
 *
 * This class provides Limbo mgmt - directory/file/... names, helper methods, etc.
 *
 * Limbo is inspired by Limbo dream level in Inception movie by Christopher Nolan.
 */
class Limbo
{
private:
    std::vector<Outline*> limboOutlines;

    std::string osLimboDirPath;
    std::string nsLimboFilePath;

public:
    explicit Limbo();
    Limbo(const Limbo&) = delete;
    Limbo(const Limbo&&) = delete;
    Limbo &operator=(const Limbo&) = delete;
    Limbo &operator=(const Limbo&&) = delete;
    ~Limbo();

    /**
     * @brief Reinitilize paths on MD, MD dir or MF repository open.
     */
    //void reinit();

    //const std::string& getOutlinesLimboDirPath() const { return limboPath; }
    //const std::string& getNotesLimboFilePath() const { return limboPath; }

    //void forget(Outline* o);
    //void forget(Note* n);

    /**
     * @brief Purge all Os and Ns in limbo.
     */
    //void purgeLimbo();
};

}
#endif // M8R_LIMBO_H

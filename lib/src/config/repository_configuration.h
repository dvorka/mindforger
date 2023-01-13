/*
 repository_configuration.h     M8r configuration management

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_REPOSITORY_CONFIGURATION_H_
#define M8R_REPOSITORY_CONFIGURATION_H_

#include <string>
#include <vector>

#include "../model/organizer.h"

namespace m8r {

// IMPROVE :-Z C++
constexpr const auto FILENAME_M8R_REPOSITORY_CONFIGURATION = "repository-configuration.md";

/**
 * @brief MindForger configuration.
 *
 * Repository configuration file (Markdown-based DSL) maintained by
 * this class contains repository specific configuration (cross
 * repository / files MindForger configuration is maintained by
 * Configuration class.
 *
 * MindForger repository configuration file is stored in
 * $REPOSITORY/repository-configuration.md
 *
 * This class instance is referenced from global configuration
 * in case that MindForger is in Repository::RepositoryType==MINDFORGER
 * and Repository::RepositoryMODE==REPOSITORY mode. It is not available
 * if MindForger shows just file(s).
 */
class RepositoryConfiguration {

private:
    // organizers
    std::vector<Organizer*> organizers;

public:
    explicit RepositoryConfiguration();
    RepositoryConfiguration(const RepositoryConfiguration&) = delete;
    RepositoryConfiguration(const RepositoryConfiguration&&) = delete;
    RepositoryConfiguration &operator=(const RepositoryConfiguration&) = delete;
    RepositoryConfiguration &operator=(const RepositoryConfiguration&&) = delete;
    virtual ~RepositoryConfiguration();

    void clear();

    /*
     * organizers
     */
    void clearOrganizers() { this->organizers.clear(); }
    void addOrganizer(Organizer* organizer);
    void removeOrganizer(Organizer* organizer);
    std::vector<Organizer*> getOrganizers() const { return this->organizers; }
    void sortOrganizers();
};

} // namespace

#endif /* M8R_REPOSITORY_CONFIGURATION_H_ */

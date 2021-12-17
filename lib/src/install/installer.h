/*
 installer.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_INSTALLER_H
#define M8R_INSTALLER_H

#include <string>
#include <iostream>

#include "../definitions.h"
#include "../config/configuration.h"
#include "../gear/file_utils.h"
#include "../exceptions.h"

namespace m8r {

class Installer
{
public:
    static const std::string FILE_PATH_DEFAULT_LINUX_SYSTEM_REPOSITORY;

private:
    std::string filePathDefaultSystemRepository;

public:
    explicit Installer();
    Installer(const Installer&) = delete;
    Installer(const Installer&&) = delete;
    Installer &operator=(const Installer&) = delete;
    Installer &operator=(const Installer&&) = delete;
    ~Installer() = default;

    /**
     * @brief Create empty MindForger repository.
     *
     * Creates the directory and the skeleton of empty repository in it.
     */
    bool createEmptyMindForgerRepository(const std::string& directory);

    /**
     * @brief Init empty MindForger repository w/ default documentation and stencils.
     */
    bool initMindForgerRepository(bool copyDoc, bool copyStencils, const char* dstRepository);
};

}
#endif // M8R_INSTALLER_H

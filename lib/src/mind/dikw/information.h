/*
 information.h     MindForger thinking notebook

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
#ifndef M8R_INFORMATION_H
#define M8R_INFORMATION_H

#include <string>

namespace m8r {

class InformationSource
{
public:
    static const std::string DIR_MEMORY_M1ndF0rg3rL1br8ry;
    static const std::string FILE_META_M1ndF0rg3rL1br8ryM3t8;

    enum SourceType {
        FILESYSTEM,
        URL,
        WIKI,
        MAN,
        ZEAL_REPOSITORY,
        ZEAL_DOCSET
    };

protected:
    // information source type
    SourceType type;
    // information source locator e.g. filesystem path, URL, ...
    std::string locator;

public:
    explicit InformationSource(SourceType type, std::string locator);
    InformationSource(const InformationSource&) = delete;
    InformationSource(const InformationSource&&) = delete;
    InformationSource &operator=(const InformationSource&) = delete;
    InformationSource &operator=(const InformationSource&&) = delete;
    ~InformationSource();
};


class Information
{
public:
    explicit Information();
    Information(const Information&) = delete;
    Information(const Information&&) = delete;
    Information &operator=(const Information&) = delete;
    Information &operator=(const Information&&) = delete;
    ~Information();
};

}
#endif // M8R_INFORMATION_H

/*
 tag.h     MindForger application entry point

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
#ifndef M8R_TAG_H_
#define M8R_TAG_H_

#include <string>

#include "../mind/ontology/thing.h"

namespace m8r {

/**
 * Outline label - an extensible set of labels w/ a predefined base.
 * Labels are loaded from the configuration (default set populated on installation).
 * Outline may have associated zero or one label. Labels may have associated also
 * a color.
 */
class Tag : public Thing
{

public:
    // static initialization order fiasco prevention
    static const std::string& KeyImportant(void) {
        static const std::string KEY_IMPORTANT = std::string{"important"};
        return KEY_IMPORTANT;
    }
    static const std::string& KeyCool(void) {
        static const std::string KEY_COOL = std::string{"cool"};
        return KEY_COOL;
    }
    static const std::string& KeyTodo(void) {
        static const std::string KEY_TODO = std::string{"todo"};
        return KEY_TODO;
    }
    static const std::string& KeyPersonal(void) {
        static const std::string KEY_PERSONAL = std::string{"personal"};
        return KEY_PERSONAL;
    }
    static const std::string& KeyLater(void) {
        static const std::string KEY_LATER = std::string{"later"};
        return KEY_LATER;
    }
    static const std::string& KeyObsolete(void) {
        static const std::string KEY_OBSOLETE = std::string{"obsolete"};
        return KEY_OBSOLETE;
    }
    static const std::string& KeyProblem(void) {
        static const std::string KEY_PROBLEM = std::string{"problem"};
        return KEY_PROBLEM;
    }

    Tag(void) = delete;
    Tag(const std::string& name, const Color& color);
    Tag(const Tag&) = delete;
    Tag(const Tag&&) = delete;
    Tag &operator=(const Tag&) = delete;
    Tag &operator=(const Tag&&) = delete;
    virtual ~Tag();
};

} /* namespace m8r */

#endif /* M8R_TAG_H_ */

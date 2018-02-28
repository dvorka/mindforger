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

#include "../config/color.h"
#include "../mind/ontology/thing_class_rel_triple.h"

namespace m8r {

/**
 * Tag is a member of an extensible set of labels w/ a predefined base.
 * Labels are loaded from the configuration (default set populated on installation).
 * Outline may have associated zero or one label. Labels may have associated also
 * a color.
 *
 * All Outline/Note tags are organized to MindForger ontology that
 * defines semantic structure and hierarchy of maintained knowledge.
 *   Any newly created tag becomes child of the Class. User (or MindForger) may
 * move tag around the ontology.
 *   Although Thing ontology forms a single tree it is multi-layered - sub-trees
 * that beginning just below Thing. Such layer defines facet i.e. technical,
 * functional, emotional, etc. view of the tag.
 */
class Tag : public Clazz
{
private:
    const Color& color;

public:
    /**
     * @brief Tag type.
     */
    // IMPROVE this information to be captured rather in Ontology - Tag may have different parent classes
    enum TagType {
        CLASS,
        PROPERTY
    };

public:
    // static initialization order fiasco prevention
    static const std::string& KeyImportant() {
        static const std::string KEY_IMPORTANT = std::string{"important"};
        return KEY_IMPORTANT;
    }
    static const std::string& KeyCool() {
        static const std::string KEY_COOL = std::string{"cool"};
        return KEY_COOL;
    }
    static const std::string& KeyTodo() {
        static const std::string KEY_TODO = std::string{"todo"};
        return KEY_TODO;
    }
    static const std::string& KeyPersonal() {
        static const std::string KEY_PERSONAL = std::string{"personal"};
        return KEY_PERSONAL;
    }
    static const std::string& KeyLater() {
        static const std::string KEY_LATER = std::string{"later"};
        return KEY_LATER;
    }
    static const std::string& KeyObsolete() {
        static const std::string KEY_OBSOLETE = std::string{"obsolete"};
        return KEY_OBSOLETE;
    }
    static const std::string& KeyProblem() {
        static const std::string KEY_PROBLEM = std::string{"problem"};
        return KEY_PROBLEM;
    }

    static const std::string& KeyWhat() {
        static const std::string KEY_PROBLEM = std::string{"what"};
        return KEY_PROBLEM;
    }
    static const std::string& KeyHow() {
        static const std::string KEY_PROBLEM = std::string{"how"};
        return KEY_PROBLEM;
    }
    static const std::string& KeyWhy() {
        static const std::string KEY_PROBLEM = std::string{"why"};
        return KEY_PROBLEM;
    }
    static const std::string& KeyWhere() {
        static const std::string KEY_PROBLEM = std::string{"where"};
        return KEY_PROBLEM;
    }
    static const std::string& KeyWho() {
        static const std::string KEY_PROBLEM = std::string{"who"};
        return KEY_PROBLEM;
    }

    Tag() = delete;
    explicit Tag(const std::string& name, Clazz* isA, const Color& color);
    Tag(const Tag&) = delete;
    Tag(const Tag&&) = delete;
    Tag &operator=(const Tag&) = delete;
    Tag &operator=(const Tag&&) = delete;
    virtual ~Tag();

    const Color& getColor() const { return color; }
};

} // m8r namespace

#endif /* M8R_TAG_H_ */

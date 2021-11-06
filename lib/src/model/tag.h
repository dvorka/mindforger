/*
 tag.h     MindForger application entry point

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
    static const std::string& KeyDone() {
        static const std::string KEY_DONE = std::string{"done"};
        return KEY_DONE;
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
        static const std::string KEY_MINDFORGER_HOME = std::string{"who"};
        return KEY_MINDFORGER_HOME;
    }

    static const std::string& KeyMindForgerHome() {
        static const std::string KEY_PROBLEM = std::string{"mindforger-home"};
        return KEY_PROBLEM;
    }

    static bool hasTagStrings(
        const std::vector<const Tag*>& thingTags,
        std::vector<std::string>& filterTags
    ) {
        if(!filterTags.size()) {
            return false;
        }
        if(thingTags.size() > 1) {
            unsigned int matches{0};
            for(std::string ft: filterTags) {
                for(const Tag* t: thingTags) {
                    if(t->equals(ft)) {
                        ++matches;
                    }
                }
            }
            return matches == filterTags.size();
        } else if(thingTags.size()==1 && filterTags.size()==1) {
            return thingTags[0]->equals(filterTags[0]);
        }
        return false;
    }
    // IMPROVE: consolidate ^v methods (iterator parameter, vector version removal)
    static bool hasTagStrings(
        const std::vector<const Tag*>& thingTags,
        std::set<std::string>& filterTags
    ) {
        if(!filterTags.size()) {
            return false;
        }
        if(thingTags.size() > 1) {
            unsigned int matches{0};
            for(std::string ft: filterTags) {
                for(const Tag* t: thingTags) {
                    if(t->equals(ft)) {
                        ++matches;
                    }
                }
            }
            return matches == filterTags.size();
        } else if(thingTags.size()==1 && filterTags.size()==1) {
            return thingTags[0]->equals(*filterTags.begin());
        }
        return false;
    }

    /**
     * @brief escape tag represented as string prior persistence.
     *
     * All forbidden characters (like ,) are replaced with -.
     *
     * @param stringTag     tag represented as string.
     * @return escaped string.
     */
    static std::string escape(std::string& stringTag) {
        std::string::value_type forbidden{','}, safe{'-'};
        std::size_t found = stringTag.find(forbidden);
        if (found != std::string::npos) {
            std::replace_if(
                std::begin(stringTag),
                std::end(stringTag),
                [&forbidden](std::string::value_type v) { return v==forbidden; },
                safe);
        }
        return stringTag;
    }

    Tag() = delete;
    explicit Tag(const std::string& name, Clazz* isA, const Color& color);
    Tag(const Tag&) = delete;
    Tag(const Tag&&) = delete;
    Tag& operator=(const Tag&) = delete;
    Tag& operator=(const Tag&&) = delete;
    virtual ~Tag();

    bool equals(const std::string& s) const {
        return this->name == s;
    }

    const Color& getColor() const { return color; }
};

/**
 * @brief List of tags.
 */
class Tags {
public:
    static constexpr const auto ESC_TAG_DELIMITER = ",,";

    // save tags as strings (w/ escaped delimiter ,, ~ ,)
    static std::string tagsToString(std::vector<std::string>& tags, bool escape=true) {
        std::string s{};

        std::string escapeString{};
        int escapeLength{2};
        if(escape) {
            escapeString.assign(ESC_TAG_DELIMITER);
        } else {
            escapeString.assign(", ");
        }

        for(std::string t:tags) {
            s.append(t);
            s.append(ESC_TAG_DELIMITER);
        }
        if(s.length()) {
            s = s.substr(0, s.length()-escapeLength);
        }
        return s;
    }
    // IMPROVE: consolidate ^v methods (iterator parameter, vector version removal)
    static std::string tagsToString(std::set<std::string>& tags, bool escape=true) {
        std::string s{};

        std::string escapeString{};
        int escapeLength{2};
        if(escape) {
            escapeString.assign(ESC_TAG_DELIMITER);
        } else {
            escapeString.assign(", ");
        }

        for(std::string t:tags) {
            s.append(t);
            s.append(escapeString);
        }
        if(s.length()) {
            s = s.substr(0, s.length()-escapeLength);
        }
        return s;
    }

    // parse tag strings from escaped string
    static std::set<std::string> tagsFromString(std::string& s) {
        std::set<std::string> tags{};

        if(s.size()) {
            size_t last = 0;
            size_t next = 0;
            while((next = s.find(ESC_TAG_DELIMITER, last)) != std::string::npos) {
                tags.insert(s.substr(last, next-last));
                last = next + 2;
            }
            tags.insert(s.substr(last));
        }

        return tags;
    }

private:
    // TODO hashset
    std::vector<const Tag*> tags;

public:
    explicit Tags();
    explicit Tags(std::vector<const Tag*> ts);
    Tags(const Tags&) = delete;
    Tags(const Tags&&) = delete;
    Tags& operator=(const Tags&) = delete;
    Tags& operator=(const Tags&&) = delete;
    ~Tags();

    std::vector<const Tag*> getTags() const { return tags; }
    std::vector<const Tag*>* getTagsPtr() { return &tags; }

    bool hasTag(const Tag* tag) const {
        if(std::find(tags.begin(), tags.end(), tag) == tags.end()) {
            return false;
        } else {
            return true;
        }
    }

    /**
     * @brief Check whether every tag in the parameter exists in tags.
     * @param ts    Tags whose presence to check.
     * @return Return `true` if all tags in the parameter are among tags, else `false`.
     */
    bool match(std::vector<const Tag*>& ts) {
        if(ts.size()) {
            for(auto t:ts) {
                if(!this->hasTag(t)) {
                    return false;
                }
            }
        }

        return true;
    }

    void addTag(const Tag* tag)
    {
        if(tag && !this->hasTag(tag)) {
            tags.push_back(tag);
        }
    }

    void addTags(std::vector<const Tag*> ts) {
        for(auto t:ts) {
            this->addTag(t);
        }
    }

    void removeTag(const Tag* tag)
    {
        // erase-remove idiom
        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
    }

    void removeTags(std::vector<const Tag*> ts) {
        for(auto t:ts) {
            this->removeTag(t);
        }
    }

    std::string toString(std::vector<std::string>& tags, bool escape=true);
    std::string toString(std::set<std::string>& tags, bool escape=true);
    std::set<std::string> fromString(std::string& s);
};

/**
 * @brief AND, OR and NOT tags expression.
 */
class TagExpression {
};

} // m8r namespace

#endif /* M8R_TAG_H_ */

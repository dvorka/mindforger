/*
 note.h     MindForger thinking notebook

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
#ifndef M8R_NOTE_H_
#define M8R_NOTE_H_

#include <vector>
#include <algorithm>
#include <string>

#include "../definitions.h"
#include "outline.h"
#include "note_type.h"
#include "tag.h"
#include "link.h"
#include "../exceptions.h"

namespace m8r {

class Outline;

/**
 * @brief Note - a thought.
 *
 * "COOL" (extensible tags)
 * "IDEA" (extensible type)
 * note "Example"
 *   w/
 * 10% progress
 * 12/24 deadline
 */
class Note : public ThingInTime
{
private:
    static constexpr int FLAG_MASK_POST_DECLARED_SECTION = 1;
    static constexpr int FLAG_MASK_TRAILING_HASHES_SECTION = 1<<1;

private:
    // parent outline - might be changed on refactoring
    Outline* outline;

    // various format, structure, semantic, ... flags (bit)
    int flags;

    // [0,inf)
    u_int16_t depth;

    // IMPROVE hashset
    std::vector<const Tag*> tags;
    std::vector<Link*> links;
    const NoteType* type;
    std::vector<std::string*> description;

    std::string modifiedPretty;
    u_int32_t revision;
    std::string readPretty;
    u_int32_t reads;

    u_int8_t progress;
    time_t deadline;

    /*
     * Transient fields
     */

    int aiAaMatrixIndex;

public:
    Note() = delete;
    explicit Note(const NoteType* type, Outline* outline);
    explicit Note(const Note&);
    Note(const Note&&) = delete;
    Note& operator=(const Note&) = delete;
    Note& operator=(const Note&&) = delete;
    void clear();
    virtual ~Note();

    void completeProperties(const time_t outlineModificationTime);
    void checkAndFixProperties();

    virtual std::string& getKey() override;

    /**
     * @brief Return GitHub compatible mangled name to ensure compatiblity between GitHub and MindForger # links.
     *
     * See also https://github.com/dvorka/trainer/blob/master/markdow/section-links-mangling.md
     */
    std::string getMangledName() const;
    time_t getDeadline() const;
    void setDeadline(time_t deadline);
    u_int16_t getDepth() const;
    void setDepth(u_int16_t depth);
    virtual void setModified() override;
    virtual void setModified(time_t modified) override;
    void makeModified();
    const std::string& getModifiedPretty() const;
    void setModifiedPretty();
    void setModifiedPretty(const std::string& modifiedPretty);
    std::string& getOutlineKey() const;
    u_int8_t getProgress() const;
    void setProgress(u_int8_t progress);
    time_t getRead() const;
    void setRead(time_t read);
    void makeRead();
    const std::string& getReadPretty() const;
    void setReadPretty();
    void setReadPretty(const std::string& readPretty);
    u_int32_t getReads() const;
    void setReads(u_int32_t reads);
    u_int32_t getRevision() const;
    void setRevision(u_int32_t revision);
    void incRevision();
    void incReads() { reads++; }
    const Tag* getPrimaryTag() const;
    const std::vector<const Tag*>* getTags() const;
    void addTag(const Tag* tag);
    void setTag(const Tag* tag);
    void setTags(const std::vector<const Tag*>* tags);
    bool hasTag(const Tag* tag) const {
        if(std::find(tags.begin(), tags.end(), tag) == tags.end()) {
            return false;
        } else {
            return true;
        }
    }
    bool hasTagStrings(std::vector<std::string>& filterTags) {
        return Tag::hasTagStrings(this->tags, filterTags);
    }
    // IMPROVE: consolidate ^v methods (iterator parameter, vector version removal)
    bool hasTagStrings(std::set<std::string>& filterTags) {
        return Tag::hasTagStrings(this->tags, filterTags);
    }
    void addName(const std::string& s);
    const NoteType* getType() const;
    void setType(const NoteType* type);
    const std::vector<std::string*>& getDescription() const;
    std::string getDescriptionAsString(const std::string& separator="\n") const;
    void setDescription(const std::vector<std::string*>& description);
    void moveDescription(std::vector<std::string*>& target);
    void clearDescription();
    void addDescription(const std::vector<std::string*>& d);
    void addDescriptionLine(std::string *line);
    Outline* getOutline() const;
    void setOutline(Outline* outline);

    void addLink(Link* link);
    const std::vector<Link*>& getLinks() const { return links; }
    size_t getLinksCount() const { return links.size(); }

    void promote();
    void demote();

    void setPostDeclaredSection() { flags |= FLAG_MASK_POST_DECLARED_SECTION; }
    bool isPostDeclaredSection() const { return flags & FLAG_MASK_POST_DECLARED_SECTION; }
    void setTrailingHashesSection() { flags |= FLAG_MASK_TRAILING_HASHES_SECTION; }
    bool isTrailingHashesSection() const { return flags & FLAG_MASK_TRAILING_HASHES_SECTION; }

    void makeDirty();

    bool isReadOnly() const;

    int getAiAaMatrixIndex() const { return aiAaMatrixIndex; }
    void setAiAaMatrixIndex(int i) { aiAaMatrixIndex = i; }
};

} // m8r namespace

#endif /* M8R_NOTE_H_ */

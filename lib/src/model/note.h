/*
 note.h     MindForger thinking notebook

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
#ifndef M8R_NOTE_H_
#define M8R_NOTE_H_

#include <vector>

#include "outline.h"
#include "attachment.h"
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
class Note : public Thing
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

    time_t created;
    time_t modified;
    std::string modifiedPretty;
    u_int32_t revision;
    time_t read;
    u_int32_t reads;

    u_int8_t progress;
    time_t deadline;

    std::vector<Attachment*> attachments;

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

    const std::vector<Attachment*>& getAttachments() const;
    void setAttachments(const std::vector<Attachment*>& attachments);
    time_t getCreated() const;
    void setCreated(time_t created);
    time_t getDeadline() const;
    void setDeadline(time_t deadline);
    u_int16_t getDepth() const;
    void setDepth(u_int16_t depth);
    time_t getModified() const;
    void makeModified();
    void setModified();
    void setModified(time_t modified);
    const std::string& getModifiedPretty() const;
    void setModifiedPretty();
    void setModifiedPretty(const std::string& modifiedPretty);
    const std::string& getOutlineKey() const;
    u_int8_t getProgress() const;
    void setProgress(u_int8_t progress);
    time_t getRead() const;
    void setRead(time_t read);
    u_int32_t getReads() const;
    void setReads(u_int32_t reads);
    u_int32_t getRevision() const;
    void setRevision(u_int32_t revision);
    void incRevision();
    void incReads() { reads++; }
    const Tag* getPrimaryTag() const;
    const std::vector<const Tag*>& getTags() const;
    void addTag(const Tag* tag);
    void setTag(const Tag* tag);
    void setTags(const std::vector<const Tag*>* tags);
    void addName(const std::string& s);
    const NoteType* getType() const;
    void setType(const NoteType* type);
    const std::vector<std::string*>& getDescription() const;
    void setDescription(const std::vector<std::string*>& description);
    void moveDescription(std::vector<std::string*>& target);
    void clearDescription();
    void addDescription(const std::vector<std::string*>& d);
    void addDescriptionLine(std::string *line);
    Outline*& getOutline();
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
};

} // m8r namespace

#endif /* M8R_NOTE_H_ */

/*
 outline.h     MindForger thinking notebook

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
#ifndef M8R_OUTLINE_H_
#define M8R_OUTLINE_H_

#include <string>
#include <vector>

#include "mind_entity.h"
#include "note_type.h"
#include "note.h"
#include "outline_type.h"
#include "../gear/datetime_utils.h"
#include "tag.h"

namespace m8r {

class Note;

enum class OutlineMemoryLocation {
    NORMAL,
    TEMPLATE, // IMPROVE ... > STENCIL
    LIMBO
};

/**
 * @brief Outline - a set of thoughts.
 *
 * "IMPORTANT" (extensible labels)
 * "GROW" (extensible type)
 * outline "Example"
 * LIMBO (fixed memory location: MEMORY/WORKING/LIMBO)
 *   w/
 * 2/5 importance
 * 3/5 urgency
 * 10% progress
 */
class Outline : public MindEntity
{
private:
    /**
     * @brief Auxiliary Note type that is used to represent Outline's title and description, e.g. in FTS.
     */
    static const NoteType NOTE_4_OUTLINE_TYPE;

private:
    OutlineMemoryLocation memoryLocation;

    /**
     * Outline path on the filesystem within the scope
     * of associated repository, can be used as ID.
     */
    // IMPROVE make Key object w/ equals - std::string and sequence integer for fast equals
    std::string key;

    // IMPROVE hashset
    std::vector<const Tag*> tags;
    const OutlineType* type;
    std::string title;
    std::vector<std::string*> description;

    time_t created;
    time_t modified;
    std::string modifiedPretty;
    u_int32_t revision;
    time_t read;
    u_int32_t reads;

    int8_t importance;
    int8_t urgency;
    int8_t progress;

    std::vector<Note*> notes;

    Note* outlineDescriptorAsNote;

    /**
     * @brief Markdown file size.
     */
    unsigned int bytesize;

public:
    Outline() = delete;
    explicit Outline(const OutlineType* type);
    Outline(const Outline&) = delete;
    Outline(const Outline&&) = delete;
    Outline& operator=(const Outline&) = delete;
    Outline& operator=(const Outline&&) = delete;
    virtual ~Outline();

    /**
     * @brief Notify Note modification to recalculate metadata.
     */
    virtual void notifyChange(Note* note);

    /**
     * @brief Set undefined properties.
     *
     * Checks, calculates and sets properties of a incompletely
     * initialized instance e.g missing timestamps and RD/WR
     * counts.
     */
    void completeProperties(const time_t fileModificationTime);

    /**
     * @brief TRUE if notes were loaded from filesystem.
     */
    bool isNotesLoaded(void) const;

    time_t getCreated() const;
    void setCreated(time_t created);
    int8_t getImportance() const;
    void setImportance(int8_t importance);
    const std::string& getKey() const;
    void setKey(const std::string key);
    const Tag* getPrimaryTag() const;
    const std::vector<const Tag*>& getTags() const;
    void setTag(const Tag* tag);
    void setTags(const std::vector<const Tag*>& tags);
    void addTag(const Tag *tag);
    time_t getModified() const;
    void setModified(void);
    void setModified(time_t modified);
    const std::string& getModifiedPretty() const;
    void setModifiedPretty(void);
    void setModifiedPretty(const std::string& modifiedPretty);
    const std::vector<Note*>& getNotes() const;
    size_t getNotesCount() const;
    void setNotes(const std::vector<Note*>& notes);
    void addNote(Note*);

    /**
     * @brief Add note on given offset.
     */
    void addNote(Note*, int offset);

    /**
     * @brief Forget note and its children.
     */
    void forgetNote(Note*);

    int8_t getProgress() const;
    void setProgress(int8_t progress);
    u_int32_t getRevision() const;
    void setRevision(u_int32_t revision);
    const OutlineType* getType() const;
    void setType(const OutlineType* type);
    int8_t getUrgency() const;
    void setUrgency(int8_t urgency);
    u_int32_t getReads() const;
    void setReads(u_int32_t reads);
    time_t getRead() const;
    void setRead(time_t read);
    OutlineMemoryLocation getMemoryLocation() const;
    void setMemoryLocation(OutlineMemoryLocation memoryLocation);
    const std::vector<std::string*>& getDescription() const;
    std::string getDescriptionAsString() const;
    void addDescriptionLine(std::string *);
    void setDescription(const std::vector<std::string*>& description);
    virtual const std::string& getTitle() const;
    void setTitle(const std::string& title);
    unsigned int getBytesize() const;
    void setBytesize(unsigned int bytesize);

    Note* getOutlineDescriptorAsNote(void);
};

} /* namespace m8r */

#endif /* M8R_OUTLINE_H_ */

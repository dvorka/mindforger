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

#include "../debug.h"

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
public:
    static const int NO_OFFSET = -1;
    static const int NO_SIBLING = -1;
    static const u_int16_t MAX_NOTE_DEPTH = 100;

private:
    /**
     * @brief Auxiliary Note type that is used to represent Outline's title and description, e.g. in FTS.
     */
    static const NoteType NOTE_4_OUTLINE_TYPE;

public:
    class Patch;

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

    // IMPROVE introduce own data structure: tree of ordered notes w/ offset i.e. heap
    std::vector<Note*> notes;

    Note* outlineDescriptorAsNote;

    /**
     * @brief Markdown file size.
     */
    unsigned int bytesize;

public:
    Outline() = delete;
    explicit Outline(const OutlineType* type);
    explicit Outline(const Outline&);
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
     * @brief TRUE if Notes were loaded from filesystem.
     */
    bool isNotesLoaded() const;

    const std::string& getKey() const;
    void setKey(const std::string key);
    virtual const std::string& getTitle() const;
    void setTitle(const std::string& title);
    const std::vector<std::string*>& getDescription() const;
    std::string getDescriptionAsString() const;
    void addDescriptionLine(std::string *);
    void setDescription(const std::vector<std::string*>& description);
    void clearDescription();
    time_t getCreated() const;
    void setCreated(time_t created);
    int8_t getImportance() const;
    void setImportance(int8_t importance);
    const Tag* getPrimaryTag() const;
    const std::vector<const Tag*>& getTags() const;
    void setTag(const Tag* tag);
    void setTags(const std::vector<const Tag*>* tags);
    void addTag(const Tag *tag);
    time_t getModified() const;
    void makeModified();
    void setModified();
    void setModified(time_t modified);
    const std::string& getModifiedPretty() const;
    void setModifiedPretty();
    void setModifiedPretty(const std::string& modifiedPretty);
    int8_t getProgress() const;
    void setProgress(int8_t progress);
    u_int32_t getRevision() const;
    void setRevision(u_int32_t revision);
    void incRevision();
    void incReads() { reads++; }
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
    unsigned int getBytesize() const;
    void setBytesize(unsigned int bytesize);

    const std::vector<Note*>& getNotes() const;
    size_t getNotesCount() const;
    void setNotes(const std::vector<Note*>& notes);
    void addNote(Note*);
    /**
     * @brief Clone Note including its children.
     *
     * New Note is stored as cloned Note's sibling (below).
     */
    Note* cloneNote(const Note* clonedNote);
    void addNote(Note*, int offset);
    void addNotes(std::vector<Note*>, int offset);
    int getNoteOffset(const Note* note) const;
    void getNoteChildren(const Note* note, std::vector<Note*>* children=nullptr, Outline::Patch* patch=nullptr);
    /**
     * @brief Forget Note including its children.
     */
    void forgetNote(Note*);

    void promoteNote(Note* note, Outline::Patch* patch=nullptr);
    void demoteNote(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteToFirst(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteUp(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteDown(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteToLast(Note* note, Outline::Patch* patch=nullptr);

    Note* getOutlineDescriptorAsNote();

private:
    /**
     * @brief Returns offset of the first sibling above on the same level.
     *
     * This method also returns offset of the Note.
     */
    int getOffsetOfAboveNoteSibling(Note* note, int& offset);
    int getOffsetOfBelowNoteSibling(Note* note, int& offset);

    void resetClonedNote(Note* n);
    void resetClonedOutline(Outline* o);
};

/**
 * @brief Modification protocol for Outline's Notes.
 *
 * Patch describes changes that were made by an operation (like promote or demote)
 * to its Notes. It's used by higher levels (like frontend) to process/adapt to
 * these changes (e.g. widget rendering).
 */
struct Outline::Patch {
    enum Diff {
        // no difference i.e. no change made to Notes
        NO,
        // Notes given by boundaries were changed e.g. title or depth, but not moved
        CHANGE,
        // Notes given by boundaries were moved/shuffled i.e. Notes are on different offsets
        MOVE,
        // Notes given by boundaries were deleted
        DELETE
    };

    Diff diff;

    /**
     * @brief Top boundary.
     */
    unsigned int start;
    /**
     * @brief Number of children i.e. it's 0 if Note has no children.
     */
    unsigned int count;

#ifdef DO_MF_DEBUG
    void print() const {
        MF_DEBUG("Patch " << diff << " : start " << start << " count " << count << "\n");
    }
#endif
};

} /* namespace m8r */

#endif /* M8R_OUTLINE_H_ */

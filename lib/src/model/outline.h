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

#include "../mind/ontology/thing_class_rel_triple.h"
#include "note.h"
#include "outline_type.h"
#include "../representations/markdown/markdown_document.h"
#include "../gear/datetime_utils.h"

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
class Outline : public Thing
{
public:
    static const int NO_OFFSET = -1;
    static const int NO_SIBLING = -1;
    static const u_int16_t MAX_NOTE_DEPTH = 100;

private:
    static constexpr int FLAG_MASK_POST_DECLARED_SECTION = 1;
    static constexpr int FLAG_MASK_TRAILING_HASHES_SECTION = 1<<1;

    /**
     * @brief Auxiliary Note type that is used to represent Outline's name and description, e.g. in FTS.
     */
    static const NoteType NOTE_4_OUTLINE_TYPE;

public:
    struct Patch;

private:
    OutlineMemoryLocation memoryLocation;

    /**
     * Outline path on the filesystem within the scope
     * of associated repository, can be used as ID.
     */
    // IMPROVE make Key object w/ equals - std::string and sequence integer for fast equals
    std::string key;

    // various format, structure, semantic, ... flags (bit)
    int flags;

    MarkdownDocument::Format format;

    std::vector<std::string*> preamble;
    // IMPROVE hashset
    std::vector<const Tag*> tags;
    std::vector<Link*> links;
    const OutlineType* type;
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

    /*
     * Transient fields
     */

    /**
     * @brief Indicates that O has been changed (e.g. read timestamp), but it was not saved (yet).
     */
    bool dirty;

    /**
     * @brief Time scope to use for filtering (selective forgetting) of O's Ns.
     */
    TimeScope timeScope;

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
     * @brief Ensure O's properties integrity.
     */
    void checkAndFixProperties();

    /**
     * @brief Virgin Outline has default name, no Notes, clean metadata, ...
     */
    bool isVirgin() const;

    const std::string& getKey() const;
    void setKey(const std::string key);
    MarkdownDocument::Format getFormat() const { return format; }
    void setFormat(MarkdownDocument::Format format) { this->format = format; }
    const std::vector<std::string*>& getPreamble() const;
    std::string getPreambleAsString() const;
    void addPreambleLine(std::string *line);
    void setPreamble(const std::vector<std::string*>& preamble);
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
    const std::vector<const Tag*>* getTags() const;
    void setTag(const Tag* tag);
    void setTags(const std::vector<const Tag*>* tags);
    void addTag(const Tag* tag);
    bool removeTag(const Tag* tag);
    bool hasTag(const Tag* tag) const {
        if(std::find(tags.begin(), tags.end(), tag) == tags.end()) {
            return false;
        } else {
            return true;
        }
    }
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
    void addNotes(std::vector<Note*>&, int offset);
    Note* getNoteByName(const std::string& noteName) const;
    Note* getNoteByMangledName(const std::string& mangledName) const;
    int getNoteOffset(const Note* note) const;

    /**
     * @brief Get direct Os children.
     *
     * This method returns N children of the O REGARDLESS how big
     * the gap in depth is. Consider the following example:
     *
     * O . . . .
     * . . . . 1
     * . . . 2 .
     * . . . . 3
     * . . 4 . .
     * . . . 5 .
     * . . . . 6
     * . 7 . . .
     * . . 8 . .
     * . 9 . . .
     *
     * Ns: 1, 2, 4, 7 and 9 will be returned. Therefore the method
     * description could be reformulated as Ns whose PARENT is O
     * are returned regardless how big depth GAP is between O and N.
     */
    void getDirectNoteChildren(std::vector<Note*>& children);
    size_t getDirectNoteChildrenCount() { std::vector<Note*> c; getDirectNoteChildren(c); return c.size(); }
    /**
     * @brief Get direct Ns children.
     *
     * This method returns N children of the N REGARDLESS how big
     * the gap in depth is.
     */
    void getDirectNoteChildren(const Note* note, std::vector<Note*>& children);
    size_t getDirectNoteChildrenCount(const Note* note) { std::vector<Note*> c; getDirectNoteChildren(note, c); return c.size(); }

    void getNoteChildren(const Note* note, std::vector<Note*>* children=nullptr, Outline::Patch* patch=nullptr);
    /**
     * @brief Get skeleton-style (Note per level) path to root.
     */
    void getNotePathToRoot(const size_t offset, std::vector<int>& parents);
    /**
     * @brief Forget Note including its children.
     */
    void forgetNote(Note* n) { removeNote(n, true); }
    /**
     * @brief Remove Note including its children from Outline, but do NOT dealocate them.
     */
    void removeNote(Note* n) { removeNote(n, false); }

    void promoteNote(Note* note, Outline::Patch* patch=nullptr);
    void demoteNote(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteToFirst(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteUp(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteDown(Note* note, Outline::Patch* patch=nullptr);
    void moveNoteToLast(Note* note, Outline::Patch* patch=nullptr);

    Note* getOutlineDescriptorAsNote();

    bool isDirty() const { return dirty; }
    void makeDirty() { dirty = true; }
    void clearDirty() { dirty = false; }

    /*
     * Links
     */

    void addLink(Link* link);
    const std::vector<Link*>& getLinks() const { return links; }
    size_t getLinksCount() const { return links.size(); }

    /*
     * Scope
     */

    void setTimeScope(const TimeScope& timeScope) { this->timeScope = timeScope; }
    const TimeScope& getTimeScope() const { return timeScope; }

    /*
     * Dialect detection
     */

    void setPostDeclaredSection() { flags |= FLAG_MASK_POST_DECLARED_SECTION; }
    bool isPostDeclaredSection() const { return flags & FLAG_MASK_POST_DECLARED_SECTION; }
    void setTrailingHashesSection() { flags |= FLAG_MASK_TRAILING_HASHES_SECTION; }
    bool isTrailingHashesSection() const { return flags & FLAG_MASK_TRAILING_HASHES_SECTION; }

    bool isApiaryBlueprint();

private:
    void removeNote(Note* note, bool dealocate);

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
        // Notes given by boundaries were changed e.g. name or depth, but not moved
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
        MF_DEBUG("Patch " << diff << " : start " << start << " count " << count << std::endl);
    }
#endif
};

} // m8r namespace

#endif /* M8R_OUTLINE_H_ */

/*
 mind.h     MindForger thinking notebook

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
#ifndef M8R_MIND_H_
#define M8R_MIND_H_

// IMPROVE make it enum or constexpr
#define NO_PARENT 0xFFFF

#include <inttypes.h>
#include <memory>

#include "../config/configuration.h"
#include "memory.h"

namespace m8r {

class Mind
{
public:
    static const int ALL_ENTRIES = -1;

private:
    const Configuration &config;
    Memory memory;

    std::vector<Note*> memoryDwell;

    /**
     * @brief TODO time to remember e.g. 1y, 1m, 1h > change the type.
     */
    uint8_t forget;

    /**
     * @brief Cache of all Notes across all Outlines: built on FTS traversal, evicted on any save/modification/delete.
     */
    std::vector<Note*> allNotesCache;

public:
    explicit Mind(Configuration &config);
    Mind() = delete;
    Mind(const Mind&) = delete;
    Mind(const Mind&&) = delete;
    Mind& operator=(const Mind&) = delete;
    Mind& operator=(const Mind&&) = delete;
    virtual ~Mind();

    /**
     * @brief Learn from memory to start thinking.
     *
     * Learn, associate, ...
     */
    void think();

    /**
     * @brief Dream to detox, optimize, check and clean up mind/memory.
     *
     * Mental processes:
     *   > Cleanup/detox: move to limbo, remove orphans, unused memory structures, ...
     *   > Integrity:
     *     > update mind w/ memory + memory w/ persistence (FS/DB/...)
     *     > calculate/fix missing/wrong Outline/Note metadata
     *   > Optimize:
     *     > incorporate newly learned knowledge to mind
     *     > discover (transitive) associations
     *     > calculate Note in/out rels sets + in/out associations sets + ...
     *     > calculate Galaxies (Galaxy == interconnected Outlines)
     *     > calculate BlackHole (for each Galaxy find Outline with most in/out relationships)
     */
    void dream();

    /*
     * REMEMBERING
     */

    /**
     * @brief Get memory reference.
     */
    Memory& remind() { return memory; }

    /**
     * @brief Get ontology.
     */
    Ontology& ontology() { return memory.getOntology(); }

    /**
     * @brief Get memory dwell.
     *
     * Get all Notes ordered by their importance in memory and
     * trimmed by forgetting threshold.
     */
    const std::vector<Note*>& getMemoryDwell(int pageSize = ALL_ENTRIES) const;
    size_t getMemoryDwellDepth() const;

    /*
     * FIND
     */

    /**
     * @brief Find outline by title - exact match.
     */
    std::unique_ptr<std::vector<Outline*>> findOutlineByTitleFts(const std::string& regexp) const;
    std::vector<Note*>* findNoteByTitleFts(const std::string& regexp) const;
    std::vector<Note*>* findNoteFts(const std::string& regexp, const bool ignoreCase=false, Outline* scope=nullptr);
    // TODO findFts() - search also outline title and description
    //   >> temporary note of Outline type (never saved), cannot be created by user
    void getOutlineTitles(std::vector<std::string>& titles) const;

    /*
     * (CROSS) REFERENCES - explicit associations created by the user.
     */

    /**
     * @brief Get Notes references by note (outgoing).
     */
    std::vector<Note*>* getReferencedNotes(const Note& note) const;
    std::vector<Note*>* getReferencedNotes(const Note& note, const Outline& outline) const;

    /**
     * @brief Get Notes that reference the note (incoming).
     */
    std::vector<Note*>* getRefereeNotes(const Note& note) const;
    std::vector<Note*>* getRefereeNotes(const Note& note, const Outline& outline) const;

    /*
     * LABELS and TAGS
     */

    /**
     * @brief Get Outlines tagged by given tags(logical AND).
     */
    std::vector<Outline*>* getTaggedOutlines(const std::vector<Tag*> labels) const;

    /**
     * @brief Get Notes tagged by given tags (logical AND).
     */
    std::vector<Note*>* getTaggedNotes(const std::vector<Tag*> tags) const;

    /**
     * @brief Get all tags assingned to Outlines in the memory.
     */
    std::vector<Tag*>* getOutlinesTags() const;

    /**
     * @brief Get all tags used in the memory.
     */
    std::vector<Tag*>* getTags() const;

    /**
     * @brief Get all tags used in the Outline by its Notes.
     */
    std::vector<Tag*>* getNoteTags(const Outline& outline) const;

    /**
     * @brief Determine how many Outlines/Notes are labeled with this label.
     */
    unsigned getTagCardinality(const Tag& tag) const;

    /**
     * @brief Determine how many Outlines are tagged with this label.
     */
    unsigned getOutlineTagCardinality(const Tag& tag) const;

    /**
     * @brief Determine how many Notes are tagged with this tag.
     */
    unsigned getNoteTagCardinality(const Tag& tag) const;

    /*
     * TYPES
     */

    std::vector<Outline*>* getOutlinesOfType(const OutlineType& type) const;

    void getAllNotes(std::vector<Note*>& notes) const;
    std::vector<Note*>* getNotesOfType(const NoteType& type) const;
    std::vector<Note*>* getNotesOfType(const NoteType& type, const Outline& outline) const;

    /*
     * ASSOCIATIONS - associations inferred by MindForger
     */

    /**
     * @brief Get associated Notes.
     */
    std::vector<Note*>* getAssociatedNotes(const Note& note) const;

    /**
     * @brief Get associated Notes within the scope of given Outline.
     */
    std::vector<Note*>* getAssociatedNotes(const Note& note, const Outline& outline) const;

    /**
     * @brief Get associated Notes to a set of words.
     */
    std::vector<Note*>* getAssociatedNotes(const std::vector<std::string*> words) const;

    /**
     * @brief Get associated Notes within the scope of given Outline.
     */
    std::vector<Note*>* getAssociatedNotes(const std::vector<std::string*> words, const Outline& outline) const;

    /*
     * FORGETTING
     */

    /**
     * @brief Get forgetting threshold.
     *
     * Forget threshold is relative time from now.
     */
    uint8_t getForgetThreshold() const;

    /**
     * @brief Set forgetting threshold.
     *
     * Forget threshold is relative time from now.
     */
    void setForgetThreashold(uint8_t);

    /*
     * OUTLINE MANAGEMENT
     */

    /**
     * @brief Create new outline and return its key.
     */
    std::string outlineNew(
            const std::string* title = nullptr,
            const OutlineType* outlineType = nullptr,
            const int8_t importance = 0,
            const int8_t urgency = 0,
            const int8_t progress = 0,
            const std::vector<const Tag*>* tags = nullptr,
            Stencil *outlineStencil = nullptr);

    Outline* outlineClone(const std::string& outlineKey);

    /**
     * @brief Move Outline to limbo.
     *
     * Note that Outline is note deleted as object instance - it's kept in Limbo container.
     */
    bool outlineForget(std::string outlineKey);

    /*
     * NOTE MANAGEMENT
     */

    /**
     * @brief Create a new note.
     */
    Note* noteNew(
            const std::string& outlineKey,
            const uint16_t offset,
            // IMPROVE pass title by reference
            const std::string* title = nullptr,
            const NoteType* noteType = nullptr,
            u_int16_t depth = 0,
            const std::vector<const Tag*>* tags = nullptr,
            const int8_t progress = 0,
            Stencil* noteStencil = nullptr);

    /**
     * @brief Clone a note.
     *
     * Cloned note is stored down from original note on the same level of depth.
     * If note has children, then they are cloned as well.
     */
    Note* noteClone(const std::string& outlineKey, const Note* newNote);
    /*
     * @brief Forget note.
     *
     * Note is marked with FORGOTTEN attribute in metadata and moved
     * to the end of outline. It's NOT rendered in default outline view,
     * however, it is rendered in archive view. Forgotten notes are NOT
     * indexed. Forgotten note can be DELETED with delete operation in
     * "limbo" view.
     */
    // IMPROVE noteForget();
    /**
     * @brief Delete note from outline.
     *
     * If note has children, then they are deleted as well.
     * Outline is not persisted. There is no UNDO for this operation.
     */
    Outline* noteForget(Note* note);

    /**
     * @brief Move note to the beginning on the current level of depth.
     */
    void noteFirst(Note* note, Outline::Patch* patch=nullptr);
    /**
     * @brief Move note to the end on the current level of depth.
     */
    void noteLast(Note* note, Outline::Patch* patch=nullptr);
    /**
     * @brief Move note one position up on the current level of depth.
     */
    void noteUp(Note* note, Outline::Patch* patch);
    /**
     * @brief Move note on position down on the current level of depth.
     */
    void noteDown(Note* note, Outline::Patch* patch=nullptr);
    /**
     * @brief Move note one level up from the current depth.
     *
     * If note can be moved, then return true, else return false.
     * If it has children, then its children are promoted as well.
     */
    void notePromote(Note* note, Outline::Patch* patch=nullptr);
    /**
     * @brief Move note one level down from the current depth.
     */
    void noteDemote(Note* note, Outline::Patch* patch=nullptr);

    /**
     * @brief Refactor note from the outline to another outline.
     */
    bool noteRefactor(
            std::string fromOutlineKey,
            uint16_t fromNoteId,
            std::string toOutlineKey,
            uint16_t toParentNoteId=NO_PARENT);
    /**
     * @brief Convert an outline to note in another outline.
     * @return new note key
     */
    std::string makeOutlineNote(
            std::string fromOutlineKey,
            std::string toOutlineKey,
            uint16_t toParentNoteId=NO_PARENT);
    /**
     * @brief Convert a note to new outline.
     * @return new Outline key
     */
    std::string makeNoteOutline(
            std::string fromOutlineKey,
            uint16_t fromNoteId);

    /*
     * DIAGNOSTICS
     */

    // IMPROVE MemoryStatistics getStatistics();

private:
    void noteChildren(Note* note, std::vector<Note*>& children, Outline::Patch* patch);

    /**
     * @brief Invoked on remembering Outline/Note/... to flush all inferred knowledge, caches, ...
     */
    void onRemembering();

    void findNoteFts(
            std::vector<Note*>* result,
            const std::string& regexp,
            const bool ignoreCase,
            Outline* outline);
};

} /* namespace */

#endif /* M8R_MIND_H_ */

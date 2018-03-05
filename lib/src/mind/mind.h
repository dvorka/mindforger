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

#include <inttypes.h>
#include <memory>

#include "memory.h"
#include "ontology/thing_class_rel_triple.h"
#include "../config/configuration.h"
#include "aspect/forget_aspect.h"

namespace m8r {

constexpr auto NO_PARENT = 0xFFFF;

/**
 * @brief Mind.
 *
 * MindForger aims to be *thinking* notebook. By thinking is meant a mental
 * process of human mind. While mind is thinking, it instantly throws ideas,
 * suggestions, associations, inferences based on what it's host (human)
 * percieves i.e. based on the current though and what host sees/smells/feels.
 *
 * Mind can be in the following states:
 *
 * SLEEPING (do nothing)
 *   Mind is cleared and no mental processes are running.
 *
 * THINKING (think)
 *   Mind instantly provides a set of Things (Notes/Outlines/Tags/...) for
 * a given Thing. When:
 *   - SEARCHING/recalling/Menu:Associate it provides relevant Thing suggestions
 *     as user writes query.
 *     For example, for query string user gets relevant Outlines and Notes.
 *   - READING/browsing it provides Things relevant to the viewed Thing.
 *     For example, for viewed Note user gets relevant Notes.
 *   - WRITING it provides Things relevant to the Thing being changed and to
 *     the text being written.
 *     For example, for a word written to description user gets relevant Notes.
 *
 * DREAMING (heal and get ready)
 *   Mind is cleared, then Memory content is processed to Mind.
 * Things and Relationships are converted to Triples, implicit/transitive/opposite/...
 * relationships are inferred to Triples as well, etc. Once Memory is processed
 * to Mind, it's checked (for Triples integrity), optimized (redundant Triples
 * are removed). Dreaming is idle when Mind is ready to wake up.
 *
 * Mind's flow of thoughts cannot be stopped when awake, it can just be slowed
 * down for instance by meditation.
 *
 * See also m8r::Ontology.
 */
/* Semantic scoping is used by Mind to limit/restrict/choose relevant Things
 * (consider a concept like 'wheel' in various Outlines like Car, Wheel of Life, ...):
 *
 * - Relevant Outlines for an Outline are filtered by Tags union (Outline w/ biggest
 *   tag union is choosen from candidates).
 * - Relevant Notes for a Note are searched from the current Outline first and then
 *   from other Outlines (autolinking, link completion).
 * - text completion is performed from the current description first, then from Outline's
 *   Notes.
 * - ...
 */
class Mind
{
public:
    static const int ALL_ENTRIES = -1;

private:
    Configuration &config;
    Memory memory;

    /**
     * @brief Semantic view of Memory.
     *
     * This member contains all relationships - not just explicitly defined
     * in Things, but also implicit relationships like Notes parent/child hierarchy
     * relationships, etc.
     *
     * This member is empty until Mind::think() or Mind::dream() is invoked. Once Mind
     * is in thinking state triples are maintained.
     *
     * This member is used for triple filtering (per O/N/T/...), thinking, associations,
     * inferring, ...
     *
     * See also m8r::Ontology.
     */
    std::vector<Triple*> triples;

    /**
     * @brief Notes time machine.
     *
     * This member represents memory dwell - a structure of Notes that represents their
     * relevance to the present. Deeper Note is, less relevant it is.
     */
    std::vector<Note*> memoryDwell;

    /**
     * @brief Cache of all Notes across all Outlines: built on FTS traversal, evicted on
     * any save/modification/delete.
     */
    std::vector<Note*> allNotesCache;

    /**
     * @brief Forgetting.
     */
    ForgetAspect forgetAspect;

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

    /**
     * @brief Sleep just to learn memory and relax.
     *
     * Triples are cleared - no thinking or dreaming.
     */
    void sleep();

    /**
     * @brief Learn new MindForger/Markdown repository/directory/file defined by configuration while maintaining current mind state.
     *
     * Mind and Memory is reset i.e. this method does NOT add new knowledge, but it starts over.
     */
    void learn();

    /**
     * @brief Forget everything while maintaining current mind state.
     */
    void amnesia();

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
     * @brief Find outline by name - exact match.
     */
    std::unique_ptr<std::vector<Outline*>> findOutlineByNameFts(const std::string& regexp) const;
    std::vector<Note*>* findNoteByNameFts(const std::string& regexp) const;
    std::vector<Note*>* findNoteFts(const std::string& regexp, const bool ignoreCase=false, Outline* scope=nullptr);
    // TODO findFts() - search also outline name and description
    //   >> temporary note of Outline type (never saved), cannot be created by user
    void getOutlineNames(std::vector<std::string>& names) const;

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
    std::string getForgetThresholdAsString() const { return forgetAspect.getThresholdAsString(); }
    bool isForgetThreasholdEnabled() const { return forgetAspect.isEnabled(); }
    ForgetAspect& getForgetThreshold() { return forgetAspect; }

    /*
     * OUTLINE MANAGEMENT
     */

    /**
     * @brief Create new outline and return its key.
     */
    std::string outlineNew(
            const std::string* name = nullptr,
            const OutlineType* outlineType = nullptr,
            const int8_t importance = 0,
            const int8_t urgency = 0,
            const int8_t progress = 0,
            const std::vector<const Tag*>* tags = nullptr,
            const std::vector<std::string*>* preamble = nullptr,
            Stencil* outlineStencil = nullptr);

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
            // IMPROVE pass name by reference
            const std::string* name = nullptr,
            const NoteType* noteType = nullptr,
            u_int16_t depth = 0,
            const std::vector<const Tag*>* tags = nullptr,
            const int8_t progress = 0,
            Stencil* noteStencil = nullptr);

    /**
     * @brief Clone Note.
     *
     * Cloned Note is stored down from original note on the same level of depth.
     * If Note has children, then they are cloned as well.
     */
    Note* noteClone(const std::string& outlineKey, const Note* newNote);

    /**
     * @brief Refactor Note to an Outline.
     *
     * Refactored Note is moved from owning Outline to target Outline along with its
     * child Notes.
     */
    Outline* noteRefactor(Note* noteToRefactor, const std::string& targetOutlineKey, Note* targetParent=nullptr);

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
     * TRIPLES
     */

    unsigned getTriplesCount() const { return triples.size(); }

    /*
     * DIAGNOSTICS
     */

    // IMPROVE MemoryStatistics getStatistics();

private:
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

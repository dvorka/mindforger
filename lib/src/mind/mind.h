/*
 mind.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <mutex>
#include <regex>

#include "memory.h"
#include "knowledge_graph.h"
#include "ai/ai.h"
#include "associated_notes.h"
#include "ontology/thing_class_rel_triple.h"
#include "aspect/mind_scope_aspect.h"
#include "../config/configuration.h"
#include "../representations/representation_interceptor.h"
#include "../representations/markdown/markdown_configuration_representation.h"
#ifdef MF_NER
    #include "ai/nlp/named_entity_recognition.h"
#endif

namespace m8r {

class Ai;
class KnowledgeGraph;

constexpr auto NO_PARENT = 0xFFFF;

enum class FtsSearch {
    EXACT,
    IGNORE_CASE,
    REGEXP
};

/**
 * @brief Mind.
 *
 * MindForger aims to be *thinking* notebook. By thinking is meant a mental
 * process of human mind. While mind is thinking, it instantly throws ideas,
 * suggestions, associations, inferences based on what it's host (human)
 * percieves i.e. based on the current though and what host sees/smells/feels.
 *
 * Mind lifecycle:
 *
 *  SLEEPING     ... initial Mind state
 *     |
 *   amnesia(), sleep(), learn()
 *     |
 *     V
 *  SLEEPING
 *     |
 *   think()
 *     |
 *     V
 *  DREAMING
 *     |         ... automatically switches to THINKING once DREAMING is done,
 *     |             user/API caller cannot change DREAMING to different state
 *     V
 *  THINKING
 *     |
 *   amnesia(), sleep(), learn()
 *     |
 *     V
 *  SLEEPING
 *
 * Mind states description:
 *
 * SLEEPING (do nothing)
 *   Mind/AI is cleared and no mental processes are running.
 *
 * THINKING (think)
 *   Mind *instantly throwing* (associated) Things (Notes/Outlines/Tags/...) for
 * a given Thing (N/O/T). When:
 *   - SEARCHING/recalling/Menu:Associate it provides relevant Thing suggestions
 *     as user writes query.
 *     For example, for query string user gets relevant Outlines and Notes.
 *   - READING/browsing it provides Things relevant to the viewed Thing.
 *     For example, for viewed Note user gets relevant Notes.
 *   - WRITING it provides Things relevant to the Thing being changed and to
 *     the text being written.
 *     For example, for a word written to description user gets relevant Notes.
 *
 * DREAMING (heal and get ready for thinking)
 *   Mind is cleared, then Memory content is processed to Mind (AI, NLP, neural network,
 * triples). Things and Relationships are converted to Triples, implicit/transitive/opposite/...
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
    static constexpr int ALL_ENTRIES = -1;

private:
    Configuration &config;
    Ontology ontology;
    RepresentationInterceptor* autoInterceptor;
    HtmlOutlineRepresentation htmlRepresentation;
    MarkdownConfigurationRepresentation* mdConfigRepresentation;
    Memory memory;

    /**
     * Atomic mind state changes and asynchronous computations synchronization
     * through Mind components and processes.
     */
    std::mutex exclusiveMind;

    /**
     * @brief Delete watermark is incremented when an O or N is deleted.
     *
     * This is a dirty flag used by other components to evict caches, etc.
     */
    int deleteWatermark;

    /**
     * @brief Active mental processes.
     */
    int activeProcesses;

    /**
     * @brief Need for associations.
     */
    char associationsSemaphore;

    /**
     * Where the mind thinks.
     */
    Ai* ai;

    /**
     * @brief Knowledge graph mind representation.
     */
    KnowledgeGraph* knowledgeGraph;

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
     * @brief Time scope.
     */
    TimeScopeAspect timeScopeAspect;
    /**
     * @brief Tags scope.
     */
    TagsScopeAspect tagsScopeAspect;
    /**
     * @brief Composite Mind scope - time, tag, ...
     */
    MindScopeAspect scopeAspect;

public:
    explicit Mind(Configuration &config);
    Mind() = delete;
    Mind(const Mind&) = delete;
    Mind(const Mind&&) = delete;
    Mind& operator=(const Mind&) = delete;
    Mind& operator=(const Mind&&) = delete;
    virtual ~Mind();

    HtmlOutlineRepresentation* getHtmlRepresentation() { return &htmlRepresentation; }

    int getDeleteWatermark() const { return deleteWatermark; }

    /**
     * @brief Synchronize both desired and current state and persist it.
     */
    void persistMindState(Configuration::MindState mindState) {
        config.setMindState(mindState);
        config.setDesiredMindState(mindState);
        mdConfigRepresentation->save(config);
    }

    /*
     * THINKING
     */

    /**
     * @brief Learn new MindForger/Markdown repository/directory/file defined by configuration AND *preserve* desired mind state (it's NOT changed).
     *
     * Mind and Memory is RESET i.e. this method does NOT add new knowledge, but it starts over.
     */
    bool learn();

    /**
     * @brief Think to do useful things for user when searching, viewing or editing.
     *
     * Mind is kept. If Mind is NOT initialized, then think() first switches to dream()
     * to prepare AI. When ready, it starts to think to be useful.
     */
    std::shared_future<bool> think();

    /**
     * @brief Sleep to clear Mind, keep Memory and relax.
     *
     * Memory is kept, but Mind is cleared. No thinking or dreaming.
     */
    bool sleep();

    /**
     * @brief Forget everything e.g. when MF creates a new empty repository.
     */
    bool amnesia();

    /*
     *  AI
     */

    /**
     * Manage active mind processes.
     */
    bool isActiveProcesses() const { return activeProcesses==0; }
    void incActiveProcesses() { activeProcesses++; }
    void decActiveProcesses() { activeProcesses--; }

    /*
     * Knowledge graph
     */

    KnowledgeGraph* getKnowledgeGraph() const { return knowledgeGraph; }

    unsigned getTriplesCount() const { return triples.size(); }

#ifdef MF_NER

    /*
     * NRE
     */

    bool isNerInitilized() const;
    void recognizePersons(const Outline* outline, int entityFilter, std::vector<NerNamedEntity>& result);

#endif

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
    Ontology& getOntology() { return ontology; }

    /**
     * @brief Get memory dwell.
     *
     * Get all Notes ordered by their importance in memory and
     * trimmed by forgetting threshold.
     */
    const std::vector<Note*>& getMemoryDwell(int pageSize = ALL_ENTRIES) const;
    size_t getMemoryDwellDepth() const;

    /*
     * SEARCHING
     */

    /**
     * @brief Find outline by name - exact match.
     */
    std::unique_ptr<std::vector<Outline*>> findOutlineByNameFts(const std::string& pattern) const;
    std::vector<Note*>* findNoteByNameFts(const std::string& pattern) const;
    std::vector<Note*>* findNoteFts(
            const std::string& pattern,
            const FtsSearch mode = FtsSearch::EXACT,
            Outline* outlineScope=nullptr);
    // TODO findFts() - search also outline name and description
    //   >> temporary note of Outline type (never saved), cannot be created by user
    void getOutlineNames(std::vector<std::string>& names) const;

    /*
     * SCOPING
     */

    // time scope aspect
    std::string getTimeScopeAsString() { return timeScopeAspect.getTimeScopeAsString(); }
    TimeScopeAspect& getTimeScopeAspect() { return timeScopeAspect; }

    // tags scope aspect
    TagsScopeAspect& getTagsScopeAspect() { return tagsScopeAspect; }

    // composite mind scope aspect
    MindScopeAspect& getScopeAspect() { return scopeAspect; }

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
     * @brief Get Outlines tagged by given tags (logical AND).
     */
    void findOutlinesByTags(const std::vector<const Tag*>& tags, std::vector<Outline*>& result) const;

    /**
     * @brief Get Notes tagged by given tags (logical AND).
     */
    void findNotesByTags(const std::vector<const Tag*>& tags, std::vector<Note*>& result) const;

    /**
     * @brief Get all tags assigned to Outlines in the memory.
     */
    std::vector<Tag*>* getOutlinesTags() const;

    /**
     * @brief Get all tags used in the memory.
     */
    Taxonomy<Tag>& getTags();

    /**
     * @brief Get tags w/ cardinality.
     */
    // TODO int to unsigned int
    void getTagsCardinality(std::map<const Tag*,int>& tagsCardinality);

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

    /**
     * @brief Remove tag from all Outlines.
     */
    void removeTagFromOutlines(const Tag* tag, std::vector<Outline*>& modifiedOutlines);

    /**
     * @brief Tag Outline and ensure that no other Outline has the tag. Persist modifications.
     */
    bool setOutlineUniqueTag(const Tag* tag, const std::string& outlineKey);

    /*
     * TYPES
     */

    // IMPROVE rename to getAllOs()
    const std::vector<Outline*>& getOutlines() const;
    std::vector<Outline*>* getOutlinesOfType(const OutlineType& type) const;

    void getAllNotes(std::vector<Note*>& notes, bool sortByRead=false, bool addNoteForOutline=false) const;
    std::vector<Note*>* getNotesOfType(const NoteType& type) const;
    std::vector<Note*>* getNotesOfType(const NoteType& type, const Outline& outline) const;

    /*
     * ASSOCIATIONS
     */

    void associate() { ++associationsSemaphore; }
    char needForAssociations() const { return associationsSemaphore; }
    void meditateAssociations() { associationsSemaphore = 0; }

    /**
     * @brief Get Note's associations (N -> Ns).
     *
     * Return value explanation:
     *   - future !VALID ... associated Ns are on the way - wait for future to become valid
     *                       and then call this method AGAIN (i.e. ASYNC doesn't store associations
     *                       to provided vector to avoid race conditions in the future).
     *   - future VALID
     *     > true  ... associated Ns can be found in vector
     *     > false ... associated Ns will NOT be computed - Mind's not thinking, memory empty, ...
     */
    std::shared_future<bool> getAssociatedNotes(AssociatedNotes& associations);

    /*
     * OUTLINE MGMT
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

    /**
     * @brief Import O from a TWiki file.
     */
    Outline* learnOutlineTWiki(const std::string& twikiFile);

    /**
     * @brief Clone O.
     */
    Outline* outlineClone(const std::string& outlineKey);

    /**
     * @brief Move Outline to limbo.
     *
     * Note that Outline is note deleted as object instance - it's kept in Limbo container.
     */
    bool outlineForget(std::string outlineKey);

    /*
     * NOTE MGMT
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
     * DIAGNOSTICS
     */

    // IMPROVE MemoryStatistics getStatistics();

private:
    /**
     * @brief Dream to reset, detox, optimize, check/clean up mind/memory and prepare to think.
     *
     * Memory is kept. When all dreaming activities are DONE, Mind wakes up and switches to THINK mode.
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
     *   > AI
     *     > NLP lexicon, BoW
     *     > associations neural network
     */
    std::shared_future<bool> mindDream();

    bool mindSleep();
    bool mindAmnesia();

    /**
     * @brief Invoked on remembering Outline/Note/... to flush all inferred knowledge, caches, ...
     */
    void onRemembering();

    void findNoteFts(
            std::vector<Note*>* result,
            const std::string& pattern,
            const FtsSearch searchMode,
            Outline* outline);
};

} /* namespace */

#endif /* M8R_MIND_H_ */

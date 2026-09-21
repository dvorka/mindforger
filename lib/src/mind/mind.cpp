/*
 mind.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "mind.h"

#ifdef MF_MD_2_HTML_CMARK
  #include "ai/autolinking/autolinking_mind.h"
  #include "ai/autolinking/cmark_aho_corasick_block_autolinking_preprocessor.h"
#else
  #include "ai/autolinking/naive_autolinking_preprocessor.h"
#endif

using namespace std;

namespace m8r {

Mind::Mind(Configuration &configuration)
    : config{configuration},
      ontology{},
#if defined  MF_MD_2_HTML_CMARK
      autoInterceptor(new CmarkAhoCorasickBlockAutolinkingPreprocessor{*this}),
#else
      autoInterceptor(new NaiveAutolinkingPreprocessor{*this}),
#endif
      htmlRepresentation{ontology, autoInterceptor},
      mdConfigRepresentation(new MarkdownConfigurationRepresentation{}),
      memory{configuration, ontology, htmlRepresentation},
#ifdef MF_MD_2_HTML_CMARK
      autolinking{new AutolinkingMind{*this}},
#else
      autolinking{nullptr},
#endif
      notebookTreeCache{},
      exclusiveMind{},
      wingman{nullptr},
      timeScopeAspect{},
      tagsScopeAspect{ontology},
      scopeAspect{timeScopeAspect, tagsScopeAspect}
{
    ai = new Ai{memory, *this};

    initWingman();

    deleteWatermark = 0;
    activeProcesses = 0;
    associationsSemaphore = 0;

    knowledgeGraph = new KnowledgeGraph{this};

    timeScopeAspect.setTimeScope(config.getTimeScope());
    tagsScopeAspect.setTags(config.getTagsScope());
    memory.setMindScope(&scopeAspect);

    stats = new MindStatistics();
    stats->mostReadOutline = nullptr;
    stats->mostWrittenOutline = nullptr;
    stats->mostReadNote = nullptr;
    stats->mostWrittenNote = nullptr;
    stats->mostUsedTag = nullptr;
}

Mind::~Mind()
{
    delete ai;
    if(wingman) delete wingman;
    delete knowledgeGraph;
    delete mdConfigRepresentation;
    delete autoInterceptor;
    delete autolinking;
    delete stats;

    for(auto& entry:notebookTreeCache) {
        delete entry.second;
    }

    // - Memory destruct outlines
    // - allNotesCache Notes is just container referencing Memory's Outlines
}

/*
 * THINKING
 */

bool Mind::learn()
{
    MF_DEBUG("@Learn" << endl);
    lock_guard<mutex> criticalSection{exclusiveMind};

    if(config.getMindState()!=Configuration::MindState::DREAMING && !activeProcesses) {
        MF_DEBUG("Learning..." << endl);
        mindAmnesia();
        memory.learn();
#ifdef MF_MD_2_HTML_CMARK
        autolinking->reindex();
#endif
        MF_DEBUG("Mind LEARNED " << memory.getOutlinesCount() << " Os" << endl);
        return true;
    } else {
        MF_DEBUG("Learn: CANNOT learn because Mind is DREAMING and/or there are " << activeProcesses << " active Mind processes" << endl);
        return false;
    }
}

shared_future<bool> Mind::think()
{
    MF_DEBUG("@Think w/ threshold " << config.getAsyncMindThreshold() << endl);
    lock_guard<mutex> criticalSection{exclusiveMind};

    if(config.getMindState()==Configuration::MindState::SLEEPING) {
        if(config.getAsyncMindThreshold() > memory.getNotesCount()) {
            // get ready for thinking - dream() changes state to THINKING on its finish
            return mindDream();
        } else {
            // IMPROVE design ASYNC AI/AA to handle also huge repositories
            MF_DEBUG(
                "Think: CANNOT think because number of Notes in Mind is too big"
                <<
                memory.getNotesCount() << "/" << config.getAsyncMindThreshold()
                << endl
                );
            persistMindState(Configuration::MindState::SLEEPING);
            promise<bool> p;
            p.set_value(false);
            return p.get_future();
        }
    } else {
        MF_DEBUG("Think: CANNOT think because Mind is DREAMING or already THINKING (asleep first)" << endl);
        promise<bool> p;
        p.set_value(false);
        return p.get_future();
    }
}

/* It does NOT need mutex because it's private and can be called from Mind only.
 * This method may run long time. It ALWAYS switches mind state to THINKING when finishes.
 */
shared_future<bool> Mind::mindDream()
{
    MF_DEBUG("@Dream" << endl);

    // Mind is expected to be clean and SLEEPING
    if(config.getMindState()==Configuration::MindState::SLEEPING) {
        MF_DEBUG("Dreaming..." << endl);
        config.setMindState(Configuration::MindState::DREAMING);

        // sanity
        // o integrity check: ...
        // o memory structure check:
        //  - Os w/o description
        //  - Os w/o any N
        //  - Ns w/o description
        // o links
        //  - broken links (target doesn't exist)
        //  - orphan files/images not referenced from any O or N by a link

        // triples: infer all triples, check, fix, optimize and save

        // AI: AA, NN, ... may take long time to finish
        return ai->dream();
    } else {
        MF_DEBUG("Dream: CANNOT dream because Mind is not ready ~ SLEEPING (asleep first)" << endl);
        promise<bool> p;
        p.set_value(false);
        return p.get_future();
    }
}

bool Mind::sleep()
{
    MF_DEBUG("@Sleep" << endl);
    lock_guard<mutex> criticalSection{exclusiveMind};
    if(mindSleep()) {
        persistMindState(Configuration::MindState::SLEEPING);
        return true;
    } else {
        return false;
    }
}

/*
 *  This method does NOT need mutex because it's private and it's called from Mind only
 */
bool Mind::mindSleep()
{
    if(config.getMindState()!=Configuration::MindState::DREAMING && !activeProcesses) {
        // AI can asleep ONLY if there are no active mental processes
        if(ai->sleep()) {
            meditateAssociations();

            allNotesCache.clear();
            memoryDwell.clear();
            triples.clear();

            MF_DEBUG("Mind IS sleeping..." << endl);
            return true;
        } else {
            MF_DEBUG("Sleep: CANNOT asleep because there are " << activeProcesses << " active Mind processes" << endl);
            return false;
        }
    } else {
        MF_DEBUG("Sleep: CANNOT asleep because Mind is DREAMING (wait for " << activeProcesses << " dreaming processes to finish)" << endl);
        // DREAMING cannot be cancelled > wait for dream() to finish before calling sleep() again.
        return false;
    }
}

bool Mind::amnesia()
{
    MF_DEBUG("@Amnesia" << endl);
    lock_guard<mutex> criticalSection{exclusiveMind};
    if(mindAmnesia()) {
        persistMindState(Configuration::MindState::SLEEPING);
        return true;
    } else {
        return false;
    }
}

shared_future<bool> Mind::getAssociatedNotes(AssociatedNotes& associations)
{
    lock_guard<mutex> criticalSection{exclusiveMind};

    if(config.getMindState()==Configuration::MindState::THINKING) {
        switch(associations.getSourceType()) {
        case OUTLINE:
            MF_DEBUG("Outline associations..." << endl);
            return ai->getAssociatedNotes(associations.getOutline(), *associations.getAssociations());
            break;
        case NOTE:
            MF_DEBUG("Note associations..." << endl);
            return ai->getAssociatedNotes(associations.getNote(), *associations.getAssociations());
        case WORD:
            MF_DEBUG("Word associations..." << endl);
            return ai->getAssociatedNotes(associations.getWord(), *associations.getAssociations(), associations.getNote());
        default:
            ; // NOP
        }
    }

    associations.getAssociations()->clear();
    promise<bool> p{};
    p.set_value(false);
    return shared_future<bool>(p.get_future());
}

/*
 *  This method does NOT need mutex because it's private and it's called from Mind only
 */
bool Mind::mindAmnesia()
{
    if(config.getMindState()!=Configuration::MindState::DREAMING && !activeProcesses) {
        mindSleep();

        // forget EVERYTHING
        memory.amnesia();
#ifdef MF_MD_2_HTML_CMARK
        autolinking->clear();
#endif
        MF_DEBUG("Mind WITH amnesia" << endl);
        return true;
    } else {
        MF_DEBUG("Amnesia: CANNOT asleep because Mind is DREAMING (wait for " << activeProcesses << " dreaming processes to finish)" << endl);
        return false;
    }
}

/*
 * Autolinking
 */

void Mind::autolinkUpdate(const std::string& oldName, const std::string& newName) const
{
#ifdef MF_MD_2_HTML_CMARK
    autolinking->update(oldName, newName);
#endif
}

bool Mind::autolinkFindLongestPrefixWord(std::string& s, std::string& r) const
{
#ifdef MF_MD_2_HTML_CMARK
    return autolinking->findLongestPrefixWord(s, r);
#else
    return false;
#endif
}

/*
 * Remembering
 */


void Mind::remember(const std::string& outlineKey)
{
    memory.remember(outlineKey);

    // TODO onRemembering()

#ifdef MF_MD_2_HTML_CMARK
    if(config.isAutolinking()) {
        autolinking->reindex();
    }
#endif
}

void Mind::remember(Outline* outline)
{
    memory.remember(outline);

#ifdef MF_MD_2_HTML_CMARK
    if(config.isAutolinking()) {
        autolinking->reindex();
    }
#endif
}

void Mind::forget(Outline* outline)
{
    memory.forget(outline);

    // TODO onRemembering()

#ifdef MF_MD_2_HTML_CMARK
    if(config.isAutolinking()) {
        autolinking->reindex();
    }
#endif
}


const vector<Note*>& Mind::getMemoryDwell(int pageSize) const
{
    UNUSED_ARG(pageSize);

    return memoryDwell;
}

size_t Mind::getMemoryDwellDepth() const
{
    return memoryDwell.size();
}

/*
vector<Note*>* Mind::findNoteByNameFts(const string& pattern) const
{
    UNUSED_ARG(pattern);

    return nullptr;
}
*/

void Mind::getOutlineNames(vector<string>& names) const
{
    // IMPROVE PERF cache vector (stack member) until and evict on memory modification
    vector<Outline*> outlines = memory.getOutlines();
    for(Outline* outline:outlines) {
        names.push_back(outline->getName());
    }
}

void Mind::getOutlineKeys(vector<string>& keys) const
{
    // IMPROVE PERF cache vector (stack member) until and evict on memory modification
    vector<Outline*> outlines = memory.getOutlines();
    for(Outline* outline:outlines) {
        keys.push_back(outline->getKey());
    }
}

// One match in either title or body is enought to be added to the result
void Mind::findNoteFts(
        vector<Note*>* result,
        const string& pattern,
        const FtsSearch searchMode,
        Outline* outline)
{
    // IMPROVE make this faster - do NOT convert to lower case, but compare it in that method > will do less
    // IMPROVE avoid duplicate code - introduce an pre-processing iface (lower/nop) and used one code
    if(searchMode == FtsSearch::IGNORE_CASE) {
        string s{};
        stringToLower(outline->getName(), s);
        if(s.find(pattern)!=string::npos) {
            result->push_back(outline->getOutlineDescriptorAsNote());
        } else {
            for(string* d:outline->getDescription()) {
                if(d) {
                    s.clear();
                    stringToLower(*d, s);
                    if(s.find(pattern)!=string::npos) {
                        result->push_back(outline->getOutlineDescriptorAsNote());
                        break;
                    }
                }
            }
        }
        for(Note* note:outline->getNotes()) {
            if(scopeAspect.isOutOfScope(note)) {
                continue;
            }
            s.clear();
            stringToLower(note->getName(), s);
            if(s.find(pattern)!=string::npos) {
                result->push_back(note);
            } else {
                for(string* d:note->getDescription()) {
                    if(d) {
                        s.clear();
                        stringToLower(*d, s);
                        if(s.find(pattern)!=string::npos) {
                            result->push_back(note);
                            break;
                        }
                    }
                }
            }
        }
    } else if (searchMode == FtsSearch::EXACT) {
        if(outline->getName().find(pattern)!=string::npos) {
            result->push_back(outline->getOutlineDescriptorAsNote());
        } else {
            for(string* d:outline->getDescription()) {
                if(d && d->find(pattern)!=string::npos) {
                    result->push_back(outline->getOutlineDescriptorAsNote());
                    // avoid multiple matches in the result
                    break;
                }
            }
        }
        for(Note* note:outline->getNotes()) {
            if(scopeAspect.isOutOfScope(note)) {
                continue;
            }
            if(note->getName().find(pattern)!=string::npos) {
                result->push_back(note);
            } else {
                for(string* d:note->getDescription()) {
                    if(d && d->find(pattern)!=string::npos) {
                        result->push_back(note);
                        // avoid multiple matches in the result
                        break;
                    }
                }
            }
        }
    } else if (searchMode == FtsSearch::REGEXP) {
        std::smatch matchedString;
        std::regex regex{pattern};
        if(std::regex_search(outline->getName(), matchedString, regex)) {
            result->push_back(outline->getOutlineDescriptorAsNote());
        } else {
            for(string* d:outline->getDescription()) {
                if(d && std::regex_search(*d, matchedString, regex)) {
                    result->push_back(outline->getOutlineDescriptorAsNote());
                    // avoid multiple matches in the result
                    break;
                }
            }
        }
        for(Note* note:outline->getNotes()) {
            if(scopeAspect.isOutOfScope(note)) {
                continue;
            }
            if(std::regex_search(outline->getName(), matchedString, regex)) {
                result->push_back(note);
            } else {
                for(string* d:note->getDescription()) {
                    if(d && std::regex_search(*d, matchedString, regex)) {
                        result->push_back(note);
                        // avoid multiple matches in the result
                        break;
                    }
                }
            }
        }
    }
}

// IMPROVE consider result be parameter passed by caller (reuse, mem)
vector<Note*>* Mind::findNoteFts(const string& pattern, FtsSearch searchMode, Outline* outlineScope)
{
    if(allNotesCache.size()) {
        allNotesCache.clear();
    }

    vector<Note*>* result = new vector<Note*>();

    string r{};
    if(searchMode == FtsSearch::IGNORE_CASE) {
        stringToLower(pattern, r);
    } else {
        r.assign(pattern);
    }

    if(outlineScope) {
        findNoteFts(result, r, searchMode, outlineScope);
    } else {
        const vector<m8r::Outline*> outlines = memory.getOutlines();
        for(Outline* outline:outlines) {
            if(scopeAspect.isOutOfScope(outline)) {
                continue;
            }
            findNoteFts(result, r, searchMode, outline);
        }
    }
    return result;
}

vector<Note*>* Mind::getReferencedNotes(const Note& note) const
{
    UNUSED_ARG(note);

    return nullptr;
}

vector<Note*>* Mind::getReferencedNotes(const Note& note, const Outline& outline) const
{
    UNUSED_ARG(note);
    UNUSED_ARG(outline);

    return nullptr;
}

vector<Note*>* Mind::getRefereeNotes(const Note& note) const
{
    UNUSED_ARG(note);

    return nullptr;
}

vector<Note*>* Mind::getRefereeNotes(const Note& note, const Outline& outline) const
{
    UNUSED_ARG(note);
    UNUSED_ARG(outline);

    return nullptr;
}

void Mind::findNotesByTags(const vector<const Tag*>& tags, vector<Note*>& result) const
{
    vector<Note*> allNotes{};
    memory.getAllNotes(allNotes);
    for(Note* n:allNotes) {
        const vector<const Tag*>* thingTags = n->getTags();
        bool hasAllTags=true;
        for(size_t i=0; i<tags.size(); i++) {
            if(std::find(
                        thingTags->begin(),
                        thingTags->end(),
                        tags.at(i)) == thingTags->end())
            {
                hasAllTags=false;
                break;
            }
        }
        if(hasAllTags) {
            result.push_back(n);
        }
    }
}

void Mind::getAllThings(
    vector<Thing*>& things,
    vector<string>* thingsNames,
    string* pattern,
    ThingNameSerialization as,
    Outline* currentO)
{
    const vector<Outline*>& os = getOutlines();
    for(Outline* o:os) {
        if((pattern && stringStartsWith(o->getName(), *pattern))
              ||
            pattern==nullptr)
        {
            things.push_back(o);
            if(thingsNames) {
                string s{};
                switch(as) {
                case ThingNameSerialization::LINK:
                    // IMPROVE make this Note's method
                    {
                        s += "[";
                        s += o->getName();
                        s += "](";
                        string p = RepositoryIndexer::makePathRelative(
                             config.getActiveRepository(),
                             currentO?currentO->getKey():o->getKey(),
                             o->getKey());
                        pathToLinuxDelimiters(p, p);
                        s += p;
                        s += ")";
                        break;
                    }
                case ThingNameSerialization::NAME:
                case ThingNameSerialization::SCOPED_NAME:
                default:
                    s += o->getName();
                    break;
                }
                thingsNames->push_back(s);
            }
        }
    }
    vector<Note*> ns{};
    getAllNotes(ns);
    for(Note* n:ns) {
        if((pattern && stringStartsWith(n->getName(), *pattern))
              ||
            pattern==nullptr)
        {
            things.push_back(n);
            if(thingsNames) {
                string s{};
                switch(as) {
                case ThingNameSerialization::NAME:
                    s += n->getName();
                    break;
                case ThingNameSerialization::LINK:
                    // IMPROVE make this Note's method
                    {
                        s += "[";
                        s += n->getName();
                        s += " (";
                        s += n->getOutline()->getName();
                        s += ")](";
                        string p = RepositoryIndexer::makePathRelative(
                             config.getActiveRepository(),
                             currentO?currentO->getKey():n->getOutline()->getKey(),
                             n->getKey());
                        pathToLinuxDelimiters(p, p);
                        s += p;
                        s += ")";
                        break;
                    }
                case ThingNameSerialization::SCOPED_NAME:
                default:
                    {
                        // IMPROVE make this Note's method: getScopedName()
                        s += n->getName();
                        s += " (";
                        s += n->getOutline()->getName();
                        s += ")";
                        break;
                    }
                }
                thingsNames->push_back(s);
            }
        }
    }
}

const vector<Outline*>& Mind::getOutlines() const
{
    // IMPROVE PERF use dirty flag to avoid result-rebuilt
    static vector<Outline*> result{};

    if(scopeAspect.isEnabled()) {
        result.clear();
        for(Outline* o:memory.getOutlines()) {
            if(scopeAspect.isInScope(o)) {
                result.push_back(o);
            }
        }
        return result;
    } else {
        return memory.getOutlines();
    }
}

vector<Outline*>* Mind::getOutlinesOfType(const OutlineType& type) const
{
    UNUSED_ARG(type);

    return nullptr;
}

std::vector<Note*>& Mind::getAllNotes(vector<Note*>& notes, bool sortByRead, bool addNoteForOutline) const
{
    return memory.getAllNotes(notes, sortByRead, addNoteForOutline);
}

vector<Note*>* Mind::getNotesOfType(const NoteType& type) const
{
    UNUSED_ARG(type);

    return nullptr;
}

vector<Note*>* Mind::getNotesOfType(const NoteType& type, const Outline& outline) const
{
    UNUSED_ARG(type);
    UNUSED_ARG(outline);

    return nullptr;
}

void Mind::findOutlinesByTags(const vector<const Tag*>& tags, vector<Outline*>& result) const
{
    for(Outline* o:memory.getOutlines()) {
        bool allMatched = true;
        for(size_t i=0; i<tags.size(); i++) {
            if(std::find(
                        o->getTags()->begin(),
                        o->getTags()->end(),
                        tags.at(i)) == o->getTags()->end())
            {
                allMatched = false;
                break;
            }
        }
        if(allMatched) {
            result.push_back(o);
        }
    }
}

vector<Tag*>* Mind::getOutlinesTags() const
{
    return nullptr;
}

Taxonomy<Tag>& Mind::getTags()
{
    return ontology.getTags();
}

void Mind::getTagsCardinality(map<const Tag*,int>& tagsCardinality)
{
    if(ontology.getTags().size()) {
        for(const Tag* t:ontology.getTags().values()) {
            // IMPROVE make NONE exclusion faster (checks in three loops below)
            if(!stringistring(string("none"), t->getName())) {
                tagsCardinality[t] = 0;
            }
        }
        const vector<Outline*>& outlines = memory.getOutlines();
        bool doO, doN;
        for(Outline* o:outlines) {
            doO = false;
            if(scopeAspect.isEnabled()) {
                if(scopeAspect.isInScope(o)) {
                    doO = true;
                }
            } else {
                doO = true;
            }
            if(doO) {
                for(const Tag* ot:*o->getTags()) {
                    if(!stringistring(string("none"), ot->getName())) {
                        tagsCardinality[ot] = tagsCardinality[ot]+1;
                    }
                }

                doN = false;
                for(Note* n:o->getNotes()) {
                    if(scopeAspect.isEnabled()) {
                        if(scopeAspect.isInScope(n)) {
                            doN = true;
                        }
                    } else {
                        doN = true;
                    }
                    if(doN) {
                        for(const Tag* nt:*n->getTags()) {
                            if(!stringistring(string("none"), nt->getName())) {
                                tagsCardinality[nt] = tagsCardinality[nt]+1;
                            }
                        }
                    }
                }
            }
        }
    } else {
        tagsCardinality.clear();
    }
}

vector<Tag*>* Mind::getNoteTags(const Outline& outline) const
{
    UNUSED_ARG(outline);

    return nullptr;
}

unsigned Mind::getTagCardinality(const Tag& tag) const
{
    UNUSED_ARG(tag);

    return 0;
}

unsigned Mind::getOutlineTagCardinality(const Tag& tag) const
{
    UNUSED_ARG(tag);

    return 0;
}

unsigned Mind::getNoteTagCardinality(const Tag& tag) const
{
    UNUSED_ARG(tag);

    return 0;
}

void Mind::removeTagFromOutlines(const Tag* tag, vector<Outline*>& modifiedOutlines)
{
    vector<const Tag*> tags{};
    tags.push_back(tag);
    for(Outline* o:memory.getOutlines()) {
        if(o->removeTag(tag)) {
            modifiedOutlines.push_back(o);
        }
    }
}

bool Mind::setOutlineUniqueTag(const Tag* tag, const string& outlineKey)
{
    Outline* o=memory.getOutline(outlineKey);
    if(o) {
        // strip home tag from all other Outlines
        vector<Outline*> modifiedOutlines{};
        removeTagFromOutlines(tag, modifiedOutlines);
        for(Outline* mo:modifiedOutlines) {
            // persist Os w/ removed T (timestamp not changed)
            memory.remember(mo->getKey());
        }

        // mark O as modified
        o->addTag(tag);
        memory.remember(o->getKey());
        return true;
    } else {
        return false;
    }
}

string Mind::outlineNew(
    // IMPROVE pass name by reference
    const string* name,
    const OutlineType* outlineType,
    const int8_t importance,
    const int8_t urgency,
    const int8_t progress,
    const vector<const Tag*>* tags,
    const vector<string*>* preamble,
    Stencil* outlineStencil)
{
    string key = memory.createOutlineKey(name);
    Outline* outline{};
    if(outlineStencil) {
        outline = memory.createOutline(outlineStencil);
        outline->setModified();
    } else {
        outline = new Outline{ontology.getDefaultOutlineType()};
    }

    if(preamble && preamble->size()) {
        outline->setPreamble(*preamble);
    }

    if(outline) {
        outline->completeProperties(datetimeNow());
        outline->setKey(key);
        if(name && !name->empty()) {
            outline->setName(*name);
        }
        if(outlineType) {
            outline->setType(outlineType);
        }
        outline->setImportance(importance);
        outline->setUrgency(urgency);
        outline->setProgress(progress);
        if(tags) {
            for(const Tag* t:*tags) {
                outline->addTag(t);
            }
        }
        if(outline->getNotes().empty()) {
            Note* note = new Note{memory.getOntology().getNoteTypes().get(NoteType::KeyNote()), outline};
            note->completeProperties(datetimeNow());
            outline->addNote(note);
        }

        remember(outline);
        onRemembering();
    } else {
        throw MindForgerException("Unable to create new Outline!");
    }

    return outline?outline->getKey():nullptr;
}

string Mind::outlineNew(Outline* outline)
{
    if(outline) {
        remember(outline);
        onRemembering();
    }

    return outline?outline->getKey():nullptr;
}


Outline* Mind::learnOutlineTWiki(const string& twikiFile)
{
    string directory{}, file{};
    pathToDirectoryAndFile(twikiFile, directory, file);
    if(!file.size()) {
        file.assign("twiki-outline");
    }
    string outlineKey = memory.createOutlineKey(&file);
    if(memory.learnOutlineTWiki(twikiFile, outlineKey)) {
        // IMPROVE: this is heavy operation - load just O
        learn();
        Outline* o = memory.getOutline(outlineKey);
        if(o) {
            // add twiki and import tags
            o->addTag(memory.getOntology().findOrCreateTag("twiki"));
            o->addTag(memory.getOntology().findOrCreateTag("import"));

            remember(o);
            onRemembering();
            return o;
        } else {
            MF_DEBUG("Unable to load imported O: " << outlineKey << endl);
            return nullptr;
        }
    } else {
        return nullptr;
    }
}


Outline* Mind::outlineClone(const std::string& outlineKey)
{
    Outline* o = memory.getOutline(outlineKey);
    if(o) {
        Outline* clonedOutline = new Outline{*o};
        clonedOutline->setKey(memory.createOutlineKey(&o->getName()));
        memory.remember(clonedOutline);
        onRemembering();
        return clonedOutline;
    } else {
        return nullptr;
    }
}

bool Mind::outlineForget(string outlineKey)
{
    Outline* o = memory.getOutline(outlineKey);
    if(o) {
        deleteWatermark++;

        // remove the Outline from every Notebook tree BEFORE it's forgotten
        // so that no tree keeps a dangling reference to a deleted Notebook
        notebookTreeRemoveOutlineFromAll(outlineKey);

        forget(o);
        auto k = memory.createLimboKey(&o->getName());
        o->setKey(k);
        moveFile(outlineKey, k);
        return true;
    }
    return false;
}

string Mind::outlineMapKey2Relative(const string& outlineKey) const
{
    string relativeKey{
        outlineKey.substr(config.getMemoryPath().size() +1)
    };
    MF_DEBUG("  " << relativeKey << endl);

    return relativeKey;
}

string Mind::outlineMapKey2Absolute(const string& outlineKey) const
{
    string resolvedKey{
        config.getMemoryPath()
        + FILE_PATH_SEPARATOR
        + outlineKey
    };
    MF_DEBUG("  " << resolvedKey << endl);

    return resolvedKey;
}

Outline* Mind::notebookTreeNew(const string& treeKey, const string& name)
{
    MF_DEBUG("Creating new Notebook tree: " << name << endl);

    // NOTE: unlike the legacy, singleton Outlines map, a new Notebook
    // tree starts EMPTY - Notebooks are added to it by explicit user
    // action only (Mind::notebookTreeAddOutline)
    Outline* notebookTree = new Outline{
        memory.getOntology().getDefaultOutlineType()};

    notebookTree->setName(name);
    notebookTree->setKey(treeKey);
    notebookTree->completeProperties(datetimeNow());

    // cache it immediately so that a subsequent notebookTreeGet(treeKey)
    // returns this SAME instance rather than re-parsing it from disk
    notebookTreeCache[treeKey] = notebookTree;

    return notebookTree;
}

void Mind::notebookTreeRemoveStaleEntries(Outline* notebookTree)
{
    // ensure that the tree contains only Notes which still link to
    // an existing Outline - drop the rest (Outline was deleted/renamed)
    vector<Note*> osToRemove{};

    MF_DEBUG("Notebook tree links validity check:");
    for(Note* n:notebookTree->getNotes()) {
        Link* oLink = n->getLinkByName(LINK_NAME_OUTLINE_KEY);
        if(oLink) {
            string oKey{oLink->getUrl()};
            Outline* o = findOutlineByKey(oKey);
            if(o) {
                // valid O in MF & tree
                #ifdef MF_DEBUG_LIBRARY
                MF_DEBUG(
                    "  VALID  : " << n->getName() << endl <<
                    "           " << oKey << endl
                );
                #endif
                // refresh N representing O (name, timestamps, ... may be changed by other views)
                n->setName(o->getName());
                n->setModified(o->getModified());
                n->setModifiedPretty();
                n->setRead(o->getRead());
                n->setReadPretty();
            } else {
                MF_DEBUG("  INVALID (no O for link): " << n->getName() << endl);
                osToRemove.push_back(n);
            }
        } else {
            MF_DEBUG("  INVALID (missing link): " << n->getName() << endl);
            osToRemove.push_back(n);
        }
    }
    MF_DEBUG("DONE Notebook tree links validity check" << endl);

    if(osToRemove.size()) {
        MF_DEBUG("Removing Ns with INVALID O key:" << endl);
        for(auto oToRemove:osToRemove) {
            // forgetNote() removes AND deallocates a Note's WHOLE
            // subtree (every descendant of greater depth) - if
            // oToRemove was itself a descendant of an earlier,
            // also-stale entry in this list, it was already freed as
            // a side effect of that removal, so skip it here, else
            // this dereferences/double-frees already-freed memory
            const auto& currentNotes = notebookTree->getNotes();
            if(std::find(currentNotes.begin(), currentNotes.end(), oToRemove)
                == currentNotes.end()
            ) {
                continue;
            }

            MF_DEBUG("  " << oToRemove->getName() << endl);
            notebookTree->forgetNote(oToRemove);
        }
    }
}

Outline* Mind::notebookTreeLearn(const string& treeKey)
{
    #ifdef MF_DEBUG_LIBRARY
    MF_DEBUG("Learning Notebook tree from " << treeKey << endl);
    #endif
    Outline* notebookTree = memory.learnNotebookTree(treeKey);

    vector<Note*> osToRemove{};

    // normalization: set Ns types to O + resolve O links to absolute
    #ifdef MF_DEBUG_LIBRARY
    MF_DEBUG("Setting tree's Ns type O" << endl);
    #endif
    for(auto n:notebookTree->getNotes()) {
        #ifdef MF_DEBUG_LIBRARY
        MF_DEBUG(
            "  Setting '" << n->getName()
            << "' with " << n->getLinks().size() << " link(s)"
            << " to O" << endl
        );
        #endif
        n->setType(&Outline::NOTE_4_OUTLINE_TYPE);

        Link* oMemPathLink = n->getLinkByName(LINK_NAME_OUTLINE_PATH);
        if(oMemPathLink && oMemPathLink->getUrl().size() > 0) {
            string oMemPath{oMemPathLink->getUrl()};

            n->clearLinks();
            n->addLink(
                new Link{
                    LINK_NAME_OUTLINE_KEY,
                    Mind::outlineMapKey2Absolute(oMemPath)
                }
            );
            n->addLink(
                new Link{
                    LINK_NAME_OUTLINE_PATH,
                    oMemPath
                }
            );
        } else {
            MF_DEBUG("  SKIPPING N w/o link: " << n->getName() << endl);
            osToRemove.push_back(n);
        }
    }

    if(osToRemove.size()) {
        MF_DEBUG("Removing Ns with MISSING relative O key:" << endl);
        for(auto oToRemove:osToRemove) {
            // see the matching guard in notebookTreeRemoveStaleEntries()
            // for why this membership check is needed
            const auto& currentNotes = notebookTree->getNotes();
            if(std::find(currentNotes.begin(), currentNotes.end(), oToRemove)
                == currentNotes.end()
            ) {
                continue;
            }

            MF_DEBUG("  " << oToRemove->getName() << endl);
            notebookTree->forgetNote(oToRemove);
        }
        osToRemove.clear();
    }

    // drop entries whose Outline no longer exists in the Mind
    notebookTreeRemoveStaleEntries(notebookTree);

    return notebookTree;
}

Outline* Mind::notebookTreeGet(const string& treeKey)
{
    auto cached = notebookTreeCache.find(treeKey);
    if(cached != notebookTreeCache.end()) {
        // ensure consistency between mind's Os and tree's Os
        notebookTreeRemoveStaleEntries(cached->second);

        return cached->second;
    }

    Outline* notebookTree;
    if(isFile(treeKey.c_str())) {
        // load existing tree
        notebookTree = notebookTreeLearn(treeKey);
    } else {
        // tree registered, but its file is missing (e.g. deleted
        // externally) - self-heal by (re)creating an empty tree
        notebookTree = notebookTreeNew(treeKey, "Notebook Tree");
        notebookTreeRemember(notebookTree);
    }

    notebookTreeCache[treeKey] = notebookTree;

    return notebookTree;
}

Outline* Mind::notebookTreeRemember(Outline* notebookTree)
{
    if(notebookTree) {
        remind().getPersistence().save(notebookTree);
    }

    return notebookTree;
}

void Mind::notebookTreeAddOutline(Outline* notebookTree, Outline* outlineToAdd)
{
    if(!notebookTree || !outlineToAdd) {
        return;
    }

    // a Notebook may be organized to multiple trees, but not added
    // to the SAME tree twice
    for(Note* n:notebookTree->getNotes()) {
        Link* oLink = n->getLinkByName(LINK_NAME_OUTLINE_KEY);
        if(oLink && oLink->getUrl() == outlineToAdd->getKey()) {
            return;
        }
    }

    // clone O's descriptor to get N which might be deleted later
    Note* n = new Note(*outlineToAdd->getOutlineDescriptorAsNote());

    n->clearLinks();
    n->addLink(
        new Link{
            LINK_NAME_OUTLINE_KEY,
            outlineToAdd->getKey()
        }
    );
    n->addLink(
        new Link{
            LINK_NAME_OUTLINE_PATH,
            Mind::outlineMapKey2Relative(outlineToAdd->getKey())
        }
    );

    notebookTree->addNote(n, 0);
}

void Mind::notebookTreeRemoveOutlineFromAll(const string& outlineKey)
{
    for(NotebookTree* t:config.getRepositoryConfiguration().getNotebookTrees()) {
        Outline* notebookTree = notebookTreeGet(t->getKey());
        if(!notebookTree) {
            continue;
        }

        Note* toRemove = nullptr;
        for(Note* n:notebookTree->getNotes()) {
            Link* oLink = n->getLinkByName(LINK_NAME_OUTLINE_KEY);
            if(oLink && oLink->getUrl() == outlineKey) {
                toRemove = n;
                break;
            }
        }

        if(toRemove) {
            MF_DEBUG(
                "Removing forgotten Outline '" << outlineKey
                << "' from Notebook tree '" << t->getName() << "'" << endl);
            notebookTree->forgetNote(toRemove);
            notebookTreeRemember(notebookTree);
        }
    }
}

bool Mind::notebookTreeForget(const string& treeKey)
{
    // ensure it's loaded (self-heals if its file is already missing) so
    // its name is available to derive the Limbo file name from
    Outline* notebookTree = notebookTreeGet(treeKey);
    if(!notebookTree) {
        return false;
    }

    auto k = memory.createLimboKey(&notebookTree->getName());
    moveFile(treeKey, k);

    notebookTreeCache.erase(treeKey);
    delete notebookTree;

    return true;
}

Note* Mind::noteNew(
    const std::string& outlineKey,
    const uint16_t offset,
    // IMPROVE pass name by reference
    const std::string* name,
    const NoteType* noteType,
    u_int16_t depth,
    const std::vector<const Tag*>* tags,
    const int8_t progress,
    Stencil* noteStencil)
{
    Outline* o = memory.getOutline(outlineKey);
    if(o) {
        Note* n = memory.createNote(noteStencil);
        if(!n) {
            // IMPROVE make note type method parameter w/ a default
            n = new Note(ontology.findOrCreateNoteType(NoteType::KeyNote()),o);
        }
        n->setOutline(o);
        if(name) {
            n->setName(*name);
        }
        n->setModified();
        if(noteType) {
            n->setType(noteType);
        }
        n->setDepth(depth);
        if(tags) {
            n->setTags(tags);
        }
        n->setProgress(progress);
        n->completeProperties(n->getModified());

        n->setReadPretty();
        n->setModifiedPretty();

        o->addNote(n, NO_PARENT==offset?0:offset);
        return n;
    } else {
        throw MindForgerException("Outline for given key not found!");
    }
}

Note* Mind::noteClone(const string& outlineKey, const Note* newNote, const bool deep)
{
    Outline* o = memory.getOutline(outlineKey);
    if(o) {
        return o->cloneNote(newNote, deep);
    } else {
        throw MindForgerException("Outline for given key not found!");
    }
}

Outline* Mind::noteRefactor(Note* noteToRefactor, const string& targetOutlineKey, Note* targetParent)
{
    UNUSED_ARG(targetParent);

    if(noteToRefactor) {
        Outline* targetOutline = memory.getOutline(targetOutlineKey);
        if(targetOutline) {
            vector<Note*> children{};
            Outline* sourceOutline = noteToRefactor->getOutline();
            sourceOutline->getAllNoteChildren(noteToRefactor, &children);
            children.insert(children.begin(), noteToRefactor);
            // IMPROVE allow passing parent for the Note in the target Outline
            targetOutline->addNotes(children, 0);

            sourceOutline->removeNote(noteToRefactor);

            memory.remember(sourceOutline);
            memory.remember(targetOutline);

            return targetOutline;
        } else {
            throw MindForgerException("Outline for given key not found!");
        }
    } else {
        throw MindForgerException("Note to be refactored is nullptr!");
    }
}

Outline* Mind::noteForget(Note* note)
{
    Outline* o = note->getOutline();
    if(o) {
        deleteWatermark++;

        note->getOutline()->forgetNote(note);
        return o;
    } else {
        throw MindForgerException("Unable find Outline from which should be the Note deleted!");
    }
}

void Mind::noteUp(Note* note, Outline::Patch* patch)
{
    if(note) {
        note->getOutline()->moveNoteUp(note, patch);
    }
}

void Mind::noteDown(Note* note, Outline::Patch* patch)
{
    if(note) {
        note->getOutline()->moveNoteDown(note, patch);
    }
}

void Mind::noteFirst(Note* note, Outline::Patch* patch)
{
    if(note) {
        note->getOutline()->moveNoteToFirst(note, patch);
    }
}

void Mind::noteLast(Note* note, Outline::Patch* patch)
{
    if(note) {
        note->getOutline()->moveNoteToLast(note, patch);
    }
}

void Mind::notePromote(Note* note, Outline::Patch* patch)
{
    if(note) {
        note->getOutline()->promoteNote(note, patch);
    }
}

void Mind::noteDemote(Note* note, Outline::Patch* patch)
{
    if(note) {
        note->getOutline()->demoteNote(note, patch);
    }
}

void Mind::noteOnRename(const std::string& oldName, const std::string& newName)
{
#ifdef MF_MD_2_HTML_CMARK
    autolinking->update(oldName, newName);
#endif
}

void Mind::onRemembering()
{
    allNotesCache.clear();
}

MindStatistics* Mind::getStatistics()
{
    // IMPROVE cache it until memory is dirty
    const vector<Outline*>&os = memory.getOutlines();
    if(os.size()) {
        u_int32_t maxReads=0;
        u_int32_t maxWrites=0;
        for(Outline* o:os) {
            if(o->getReads() > maxReads) {
                maxReads = o->getReads();
                stats->mostReadOutline = o;
            }
            if(o->getRevision() > maxWrites) {
                maxWrites = o->getRevision();
                stats->mostWrittenOutline = o;
            }
        }
    } else {
        stats->mostReadOutline = nullptr;
        stats->mostWrittenOutline = nullptr;
    }

    vector<Note*> ns{};
    memory.getAllNotes(ns);
    if(ns.size()) {
        u_int32_t maxReads=0;
        u_int32_t maxWrites=0;
        for(Note* n:ns) {
            if(n->getReads() > maxReads) {
                maxReads = n->getReads();
                stats->mostReadNote = n;
            }
            if(n->getRevision() > maxWrites) {
                maxWrites = n->getRevision();
                stats->mostWrittenNote = n;
            }
        }
    } else {
        stats->mostReadNote = nullptr;
        stats->mostWrittenNote = nullptr;
    }

    map<const Tag*,int> ts{};
    getTagsCardinality(ts);
    if(ts.size()) {
        map<const Tag*,int>::iterator it{};
        int maxCardinality = 0;
        for(it = ts.begin(); it != ts.end(); it++) {
            if(it->second > maxCardinality) {
                stats->mostUsedTag = it->first;
                maxCardinality = it->second;
            }
        }
    } else {
        stats->mostUsedTag = nullptr;
    }

    return stats;
}

// unique_ptr template BREAKS Qt Developer indentation > stored at EOF
unique_ptr<vector<Outline*>> Mind::findOutlineByNameFts(const string& pattern) const
{
    // IMPROVE implement regexp and other search options by reusing HSTR code
    // IMPROVE PERF this method is extremely inefficient > use cached map (stack member) evicted on memory modification
    unique_ptr<vector<Outline*>> result{new vector<Outline*>()};
    if(pattern.size()) {
        vector<Outline*> outlines = memory.getOutlines();
        for(Outline* outline:outlines) {
            if(!pattern.compare(outline->getName())) {
                result->push_back(outline);
            }
        }
    }
    return result;
}

Outline* Mind::findOutlineByKey(const string& key) const
{
    if(key.size()) {
        vector<Outline*> outlines = memory.getOutlines();
        for(Outline* outline:outlines) {
            if(key.compare(outline->getKey()) == 0) {
                return outline;
            }
        }
    }

    return nullptr;
}

void Mind::initWingman()
{
    MF_DEBUG("MIND Wingman init: " << boolalpha << config.isWingman() << endl);

    if(!config.isWingman()) {
        MF_DEBUG("MIND Wingman init: DISABLED" << endl);
        if(wingman) {
            delete wingman;
            wingman = nullptr;
        }
        wingmanActiveLlmProviderId.clear();
        return;
    }

    LlmProviderConfig* provider = config.getActiveLlmProvider();
    if(!provider) {
        MF_DEBUG("MIND Wingman init: no active LLM provider configured" << endl);
        if(wingman) {
            delete wingman;
            wingman = nullptr;
        }
        wingmanActiveLlmProviderId.clear();
        return;
    }

    MF_DEBUG("MIND Wingman initialization for provider: " << provider->id << endl);

    if(wingman) {
        delete wingman;
        wingman = nullptr;
    }

    switch(provider->providerType) {
    case WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI: {
        MF_DEBUG("  MIND Wingman init: OpenAI" << endl);
        string effectiveKey = provider->apiKey;
        if(provider->useEnvVar) {
            const char* envKey = std::getenv(ENV_VAR_OPENAI_API_KEY);
            if(envKey) {
                effectiveKey = string(envKey);
                MF_DEBUG("  MIND Wingman OpenAI: using env var key" << endl);
            } else {
                MF_DEBUG("  MIND Wingman OpenAI: env var " << ENV_VAR_OPENAI_API_KEY << " not set > NO Wingman" << endl);
                wingmanActiveLlmProviderId.clear();
                return;
            }
        }
        wingman = (Wingman*)new OpenAiWingman{effectiveKey};
        wingman->setLlmModel(provider->llmModel);
        break;
    }
    case WingmanLlmProviders::WINGMAN_PROVIDER_OLLAMA:
        MF_DEBUG("  MIND Wingman init: ollama" << endl);
        wingman = (Wingman*)new OllamaWingman{provider->url};
        wingman->listModels();
        wingman->setLlmModel(provider->llmModel);
        break;
    case WingmanLlmProviders::WINGMAN_PROVIDER_OPENROUTER: {
        MF_DEBUG("  MIND Wingman init: OpenRouter" << endl);
        string effectiveKey = provider->apiKey;
        if(provider->useEnvVar) {
            const char* envKey = std::getenv(ENV_VAR_OPENROUTER_API_KEY);
            if(envKey) {
                effectiveKey = string(envKey);
                MF_DEBUG("  MIND Wingman OpenRouter: using env var key" << endl);
            } else {
                MF_DEBUG("  MIND Wingman OpenRouter: env var " << ENV_VAR_OPENROUTER_API_KEY << " not set > NO Wingman" << endl);
                wingmanActiveLlmProviderId.clear();
                return;
            }
        }
        wingman = (Wingman*)new OpenRouterWingman{effectiveKey};
        wingman->setLlmModel(provider->llmModel);
        break;
    }
    case WingmanLlmProviders::WINGMAN_PROVIDER_MOCK:
        MF_DEBUG("  MIND Wingman init: MOCK" << endl);
        wingman = (Wingman*)new MockWingman{MockWingman::LLM_MODEL_MOCK};
        break;
    default:
        MF_DEBUG("  MIND Wingman init: UNKNOWN > NO Wingman" << endl);
        wingmanActiveLlmProviderId.clear();
        return;
    }

    wingmanActiveLlmProviderId = provider->id;
}

int Mind::findLibraryOrphanOs()
{
    vector<Outline*> orphanOutlines{};
    const vector<Outline*>& outlines = memory.getOutlines();
    const Tag* t = memory.getOntology().findOrCreateTag(
        MarkdownDocumentRepresentation::TAG_LIB_DOC);
    MF_DEBUG("Searching ORPHAN library outlines..." << endl);
    for(Outline* outline:outlines) {
        if(!outline->hasTag(t)) {
            continue;
        }

        const vector<string*>& d = outline->getDescription();
        if(d.size()>0) {
            if(d[0]->size() > 0) {
                // MF_DEBUG("  Orphan: 1st line '" << *d[0] << "'" << endl);
                if(stringStartsWith(*d[0], MarkdownDocumentRepresentation::PREFIX_1ST_LINE)) {
                    // extract Markdown link
                    size_t i{strlen(MarkdownDocumentRepresentation::PREFIX_1ST_LINE)};
                    string s{
                        d[0]->substr(
                            i,
                            d[0]->size()-i)};
                    // MF_DEBUG("    '" << s << "'" << endl);

                    // parse Markdown link
                    string documentPath{};
                    if(s.size()>4 && s[0]=='[' && s[s.size()-1]==')') {
                        size_t i;
                        if((i=s.find("](")) != std::string::npos) {
                            documentPath = s.substr(i+2,s.size()-3-i);
                        }
                    }
                    if(documentPath.size()) {
                        MF_DEBUG("    '" << documentPath << "'" << endl);
                        if(!isFile(documentPath.c_str())) {
                            MF_DEBUG("      ORPHAN" << endl);
                            orphanOutlines.push_back(outline);

                            // tag O as orphan
                            const Tag* orphanTag = memory.getOntology().findOrCreateTag(
                                MarkdownDocumentRepresentation::TAG_LIB_DOC_ORPHAN);
                            if(!outline->hasTag(orphanTag)) {
                                outline->addTag(orphanTag);
                                memory.remember(outline);
                            }
                        }
                    }
                }
            }
        }
    }

#ifdef DO_MF_DEBUG
    if(orphanOutlines.size()) {
        MF_DEBUG("ORPHAN library outlines found:" << endl);
        for(Outline* o:orphanOutlines) {
            MF_DEBUG("  " << o->getName() << endl);
        }
    } else {
        MF_DEBUG("NO ORPHAN library outlines found." << endl);
    }
#endif

    return orphanOutlines.size();
}

Wingman* Mind::getWingman()
{
    string currentId = config.getActiveLlmProviderId();
    if(currentId != wingmanActiveLlmProviderId) {
        initWingman();
    } else if(wingman) {
        // sync model name in case it changed without changing the active provider
        LlmProviderConfig* provider = config.getActiveLlmProvider();
        if(provider) {
            wingman->setLlmModel(provider->llmModel);
        }
    }

    return this->wingman;
}

CommandWingmanChat Mind::wingmanChat(CommandWingmanChat& command)
{
    MF_DEBUG("MIND: Wingman chat..." << endl);

    if(getWingman()) {
        getWingman()->chat(command);
        MF_DEBUG("MIND: DONE Wingman chat" << endl);
    } else {
        MF_DEBUG("ERROR: MIND Wingman chat - Wingman NOT configured and/or initialized" << endl);
        command.errorMessage = "ERROR: Wingman NOT configured and/or initialized";
        command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
    }

    return command;
}

void Mind::refreshEmbeddings()
{
    MF_DEBUG("MIND: Refreshing embeddings..." << endl);
    if(getWingman()) {
        vector<Note*> allNotes{};
        memory.getAllNotes(allNotes);
#ifdef DO_MF_DEBUG
        int embeddingsSizeB=0;
        int counter=0;
        MF_DEBUG("  Embedding for " << allNotes.size() << " Notes:" << endl);
        auto beginTs = chrono::high_resolution_clock::now();
#endif
        for(Note* n:allNotes) {
            MF_DEBUG(
                "    " <<
                ++counter << "/" << allNotes.size() <<
                " getting embeddings for '" << n->getName() << "'" << endl);

            CommandWingmanEmbeddings command{};
            command.prompt = n->getDescriptionAsString();
            // TODO: truncate text

            // TODO: calculate embeddings ONLY if not in cache (timestamp not changed)

            getWingman()->embeddings(command);
#ifdef DO_MF_DEBUG
            if(command.answerEmbeddings.size()) {
                embeddingsSizeB += command.answerEmbeddings.size()*sizeof(command.answerEmbeddings[0]);
            }
#endif
            // TODO: store embeddings to embeddings cache: note key -> modified/embeddings vector
        }
#ifdef DO_MF_DEBUG
        auto endTs = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(endTs - beginTs);
        auto nDuration = to_string(float(duration.count()) / float(allNotes.size()));
        auto nps = to_string(1000.0 / (float(duration.count()) / float(allNotes.size())));
        MF_DEBUG(
            "Embeddings of " <<
            allNotes.size() <<
            " Notes via LLM in " <<
            to_string(duration.count()) << "ms" <<
            " (" << nDuration << "ms/N) " <<
            " (" << nps << "N/s)" <<
            endl);
        MF_DEBUG(
            "Embeddings size of " << allNotes.size() << " Notes: " <<
            stringIntFormat(std::to_string(embeddingsSizeB)) << " bytes " <<
            "(" << stringIntFormat(
                std::to_string(
                    int(
                        float(embeddingsSizeB) / float(allNotes.size())))) << " bytes/N)" <<
            endl
        );

        // BENCHMARK:
        // - production repository: 15.000 Notes
        // - ~7 Notes / second
        // - 38' for 15.000 Notes
        // - 250MB embedding vectors cache for 15.000 Notes > 16kB/Note
#endif
    }

    MF_DEBUG("MIND: DONE refreshing embeddings" << endl);
}

} /* namespace */

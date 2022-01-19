/*
 markdown_outline_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "markdown_outline_representation.h"

#include "../../mind/ontology/ontology.h"

namespace m8r {

using namespace std;
using namespace m8r::filesystem;

MarkdownOutlineRepresentation::MarkdownOutlineRepresentation(Ontology& ontology, RepresentationInterceptor* descriptionInterceptor)
    : ontology(ontology),
      descriptionInterceptor(descriptionInterceptor)
{
}

MarkdownOutlineRepresentation::~MarkdownOutlineRepresentation()
{
}

// IMPROVE return the last N doesn't seem to have much sense...
Note* MarkdownOutlineRepresentation::note(vector<MarkdownAstNodeSection*>* ast, const size_t astindex, Outline* outline)
{
    // IMPROVE move declarations to for scope
    Note* note = nullptr;
    const NoteType* noteType;
    vector<string*>* body;
    const string* s;
    for(size_t i = astindex; i < ast->size(); i++) {
        s = ast->at(i)->getMetadata().getType();
        if(s) {
            // IMPROVE consider string normalization to make parsing more robust
            // std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            // s[0] = toupper(s[0])
            if((noteType = ontology.getNoteTypes().get(*s)) == nullptr) {
                noteType = ontology.getDefaultNoteType();
            }
        } else {
            noteType = ontology.getDefaultNoteType();
        }
        note = new Note{noteType, outline};
        if(ast->at(i)->isPostDeclaredSection()) note->setPostDeclaredSection();
        if(ast->at(i)->isTrailingHashesSection()) note->setTrailingHashesSection();
        // TODO pull pointer > do NOT copy
        if (ast->at(i)->getText() != nullptr) {
            note->setName(*(ast->at(i)->getText()));
        }
        note->setDepth(ast->at(i)->getDepth());
        body = ast->at(i)->moveBody();
        if(body != nullptr) {
            for(string*& bodyItem : *body) {
                note->addDescriptionLine(bodyItem);
            }
        }
        delete body;
        note->setCreated(ast->at(i)->getMetadata().getCreated());
        note->setModified(ast->at(i)->getMetadata().getModified());
        note->setRevision(ast->at(i)->getMetadata().getRevision());
        note->setRead(ast->at(i)->getMetadata().getRead());
        note->setReads(ast->at(i)->getMetadata().getReads());
        note->setDeadline(ast->at(i)->getMetadata().getDeadline());
        note->setProgress(ast->at(i)->getMetadata().getProgress());

        if(ast->at(i)->getMetadata().getLinks().size()) {
            for(auto l:ast->at(i)->getMetadata().getLinks()) {
                note->addLink(l);
            }
            ast->at(i)->getMetadata().clearLinks();
        }

        if (ast->at(i)->getMetadata().getTags().size()) {
            const Tag* t;
            for(string* s : ast->at(i)->getMetadata().getTags()) {
                t = ontology.findOrCreateTag(*s);
                note->addTag(t);
            }
        }
        if(outline) {
            outline->addNote(note);
        }
    }
    return note;
}

Outline* MarkdownOutlineRepresentation::outline(const File& file)
{
    MarkdownDocument md{&file.name};
    md.from();
    vector<MarkdownAstNodeSection*>* ast = md.moveAst();

    Outline* o = outline(ast);
    o->setFormat(md.getFormat());
    if(o) {
        o->setKey(*md.getFilePath());
        o->setBytesize(md.getFileSize());
        o->completeProperties(md.getModified());
        o->setModifiedPretty(datetimeToPrettyHtml(o->getModified()));
    }
    return o;
}

Outline* MarkdownOutlineRepresentation::outline(vector<MarkdownAstNodeSection*>* ast)
{
    Outline* outline = new Outline{ontology.getDefaultOutlineType()};
    if(ast) {
        size_t off = 0;
        if(ast->size()) {
            MarkdownAstNodeSection* astNode = ast->at(off);

            // preamble
            if(astNode->isPreambleSection()) {
                vector<string*>* body = ast->at(off)->moveBody();
                if(body!=nullptr) {
                    // IMPROVE use body as is
                    for(string*& bodyItem:*body) {
                        outline->addPreambleLine(bodyItem);
                    }
                    delete body;
                }
                if(ast->size()>1) {
                    astNode = ast->at(++off);
                } else {
                    astNode = nullptr;
                }
            }

            if(astNode) {
                // O's section
                if(astNode->isPostDeclaredSection()) outline->setPostDeclaredSection();
                if(astNode->isTrailingHashesSection()) outline->setTrailingHashesSection();
                if(astNode->getText()!=nullptr) {
                    // IMPROVE pull pointer > do NOT copy
                    outline->setName(*(astNode->getText()));
                }
                const string* s = astNode->getMetadata().getType();
                if(s) {
                    const OutlineType* outlineType;
                    // IMPROVE consider string normalization to make parsing more robust
                    //std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                    //s[0] = toupper(s[0])
                    if((outlineType=ontology.getOutlineTypes().get(*s))==nullptr) {
                        outlineType = ontology.getDefaultOutlineType();
                    }
                    outline->setType(outlineType);
                }
                outline->setCreated(astNode->getMetadata().getCreated());
                outline->setModified(astNode->getMetadata().getModified());
                outline->setRevision(astNode->getMetadata().getRevision());
                outline->setRead(astNode->getMetadata().getRead());
                outline->setReads(astNode->getMetadata().getReads());
                outline->setImportance(astNode->getMetadata().getImportance());
                outline->setUrgency(astNode->getMetadata().getUrgency());
                outline->setProgress(astNode->getMetadata().getProgress());
                if(astNode->getMetadata().getTimeScope().relativeSecs) {
                    outline->setTimeScope(astNode->getMetadata().getTimeScope());
                }

                if(astNode->getMetadata().getLinks().size()) {
                    for(auto l:astNode->getMetadata().getLinks()) {
                        outline->addLink(l);
                    }
                    astNode->getMetadata().clearLinks();
                }

                if(astNode->getMetadata().getTags().size()) {
                    // IMPROVE move to for scope
                    const Tag* t;
                    for(string* s:astNode->getMetadata().getTags()) {
                        t = ontology.findOrCreateTag(*s);
                        outline->addTag(t);
                    }
                }

                vector<string*>* body = ast->at(off)->moveBody();
                if(body!=nullptr) {
                    // IMPROVE use body as is
                    for(string*& bodyItem:*body) {
                        outline->addDescriptionLine(bodyItem);
                    }
                    delete body;
                }
            }
        }

        // Ns sections
        if(ast->size() > off+1) {
            note(ast, off+1, outline);
        }

        // delete AST
        for(MarkdownAstNodeSection* node:*ast) {
            if(node!=nullptr) {
                delete node;
            }
        }
        delete ast;
    }

    return outline;
}

Outline* MarkdownOutlineRepresentation::header(const std::string *mdString)
{
    MarkdownDocument md{nullptr};
    md.from(mdString);
    vector<MarkdownAstNodeSection*>* ast = md.moveAst();

    return outline(ast);
}

Note* MarkdownOutlineRepresentation::note(const string *text)
{
    MarkdownDocument md{nullptr};
    md.from(text);
    vector<MarkdownAstNodeSection*>* ast = md.moveAst();

    Note* result{};
    if(ast) {
        if(ast->size()) {
            result = note(ast);
        }

        for(MarkdownAstNodeSection*& node:*ast) {
            delete node;
        }
        delete ast;
    }
    return result;
}

Note* MarkdownOutlineRepresentation::note(const File& file)
{
    string* md = fileToString(file.name);
    Note* n = note(md);
    delete md;
    return n;
}

string* MarkdownOutlineRepresentation::toHeader(Outline* outline)
{
    string* md = new string{};
    if(outline) {
        toHeader(outline, md);
    }
    return md;
}

string* MarkdownOutlineRepresentation::toPreamble(const Outline* outline, string* md)
{
    if(outline) {
        const vector<string*>& p=outline->getPreamble();
        if(p.size()) {
            for(string *s:p) {
                md->append(*s);
                // IMPROVE endl
                md->append("\n");
            }
        }
    }
    return md;
}

void MarkdownOutlineRepresentation::toHeader(Outline* outline, string* md)
{
    if(outline) {
        if(outline->getNotes().size()) {
            md->reserve(outline->getNotes().size()*AVG_NOTE_SIZE);
        }

        if(!outline->isPostDeclaredSection()) {
            md->append("# ");
        }
        if(outline->getName().size()) {
            md->append(outline->getName());
        } else {
            md->append(outline->getKey());
        }

        // IMPROVE c++11: std::to_string(int)
        if(outline->getFormat() == MarkdownDocument::Format::MINDFORGER) {
            char buffer[50];
            md->append(" <!-- Metadata:");
            md->append(" type: "); md->append(outline->getType()->getName()); md->append(";");
            if(outline->getTags()->size()) { md->append(" tags: "); md->append(to(outline->getTags())); md->append(";"); }
            if(outline->getLinksCount()) { md->append(" links: "); md->append(to(outline->getLinks())); md->append(";"); }
            md->append(" created: "); md->append(datetimeToString(outline->getCreated())); md->append(";");
            sprintf(buffer," reads: %d;",outline->getReads()); md->append(buffer);
            md->append(" read: "); md->append(datetimeToString(outline->getRead())); md->append(";");
            sprintf(buffer," revision: %d;",outline->getRevision()); md->append(buffer);
            md->append(" modified: "); md->append(datetimeToString(outline->getModified())); md->append(";");
            sprintf(buffer," importance: %d/5;",outline->getImportance()); md->append(buffer);
            sprintf(buffer," urgency: %d/5;",outline->getUrgency()); md->append(buffer);
            if(outline->getProgress()) {
                sprintf(buffer," progress: %d%%;",outline->getProgress()); md->append(buffer);
            }
            if(outline->getTimeScope().relativeSecs) {
                string ts{};
                outline->getTimeScope().toString(ts);
                md->append(" scope: "); md->append(ts); md->append(";");
            }
            md->append(" -->");
        }
        if(outline->isTrailingHashesSection()) {
            md->append(" #");
        }
        md->append("\n");

        if(outline->isPostDeclaredSection()) {
            int w=outline->getName().size()<2?2:outline->getName().size();
            for(int i=0; i<w; i++) md->append("=");
            // IMPROVE endl
            md->append("\n");
        }

        const vector<string*>& description=outline->getDescription();
        if(description.size()) {
            for(string *s:description) {
                md->append(*s);
                // IMPROVE endl
                md->append("\n");
            }
        }
    }
}

void MarkdownOutlineRepresentation::description(const std::string* md, std::vector<std::string*>& description)
{
    if(md) {
        istringstream is(*md);

        bool codeblock=false;
        int codeblockBackticksCount = 0;
        static const char SECTION = '#';
        static const char CB = '`';

        for(string line; std::getline(is, line); ) {
            // Escaping:
            // - TAB all lines starting with # to ensure correct sections separation
            //   (user creates such line when edits N description which is MD section
            //   and it would appear as section on O reload)
            if(line.size()>=3 && line[0]==CB && line[1]==CB && line[2]==CB) {
                codeblock = !codeblock;
                // detect opened ``` and automatically close it otherwise it will make
                // rest of the document code section - including the sections
                codeblockBackticksCount++;
            }
            if(line.size() && line.at(0)==SECTION && !codeblock) {
                // ESCAPE # using code fence
                line.insert(0, "    ");
                // ESCAPE # using HTML entity (configurable)
                //line.insert(0, "&#35;");
            }

            // TODO add quoting of multiline sections that use === and ---
            // TODO escape undesired --- and === section i.e. when user creates
            // them my mistake and there is NOT empty line before row with ---
            // as it would create new section which would apper on relad

            description.push_back(new string{line});
        }
        MF_DEBUG(
            "MD representation: unbounded code fence count=" << codeblockBackticksCount
            << " ~ " << (codeblockBackticksCount%2) << endl
        );
        if(codeblockBackticksCount > 0 && codeblockBackticksCount%2 == 1) {
            // close opened ``` to avoid unbounded code fence as described ^
            description.push_back(new string("```"));
        }
    } else {
        description.clear();
    }
}

string* MarkdownOutlineRepresentation::to(Outline* outline)
{
    string* md = new string{};
    md->reserve(AVG_OUTLINE_SIZE);
    return to(outline, md);
}

string* MarkdownOutlineRepresentation::to(Outline* outline, string* md)
{
    toPreamble(outline, md);
    toHeader(outline, md);
    // no longer needed: md->append("\n");
    if(outline) {
        const vector<Note*>& notes=outline->getNotes();
        if(notes.size()) {
            string noteMd{};
            for(Note* note:notes) {
                to(
                    note,
                    &noteMd,
                    outline->getFormat()==MarkdownDocument::Format::MINDFORGER,
                    // full O rendering w/o autolinking (performance)
                    false
                );
                md->append(noteMd);
                noteMd.clear();
            }
        }
    }
    return md;

}

string MarkdownOutlineRepresentation::to(const vector<const Tag*>* tags)
{
    string s;
    if(tags->size()) {
        for(const Tag* t:*tags) {
            s += t->getName();
            s += ",";
        }
        s.resize(s.size()-1);
    }
    return s;
}

string* MarkdownOutlineRepresentation::toLink(const string& label, const string& link, string* md)
{
    md->append("[");
    md->append(label);
    md->append("](");
    md->append(link);
    md->append(")");

    return md;
}

string MarkdownOutlineRepresentation::to(const vector<Link*>& links)
{
    string s;
    if(links.size()) {
        for(Link* l:links) {
            s += "[";
            s += l->getName();
            s += "](";
            s += l->getUrl();
            s += "),";
        }
        s.resize(s.size()-1);
    }
    return s;
}

string* MarkdownOutlineRepresentation::to(const Note* note)
{
    string* md = new string{};
    md->reserve(AVG_NOTE_SIZE);
    return to(note, md);
}

string* MarkdownOutlineRepresentation::to(const Note* note, string* md, bool includeMetadata, bool autolinking)
{
    md->clear();

    if(!note->isPostDeclaredSection()) {
        for(int i=0; i<=note->getDepth(); i++) {
            md->append("#");
        }
        md->append(" ");
    }

    if(note->getName().size()) {
        md->append(note->getName());
    } else {
        md->append("?");
    }

    if(includeMetadata) {
        char buffer[50];
        md->append(" <!-- Metadata:");
        md->append(" type: "); md->append(note->getType()->getName()); md->append(";");
        if(note->getTags()->size()) { md->append(" tags: "); md->append(to(note->getTags())); md->append(";"); }
        if(note->getLinksCount()) { md->append(" links: "); md->append(to(note->getLinks())); md->append(";"); }
        md->append(" created: "); md->append(datetimeToString(note->getCreated())); md->append(";");
        sprintf(buffer," reads: %d;",note->getReads()); md->append(buffer);
        md->append(" read: "); md->append(datetimeToString(note->getRead())); md->append(";");
        sprintf(buffer," revision: %d;",note->getRevision()); md->append(buffer);
        md->append(" modified: "); md->append(datetimeToString(note->getModified())); md->append(";");
        if(note->getProgress()) {
            sprintf(buffer," progress: %d%%;",note->getProgress()); md->append(buffer);
        }
        if(note->getDeadline()) {
            md->append(" deadline: "); md->append(datetimeToString(note->getDeadline())); md->append(";");
        }
        md->append(" -->");
    }
    if(note->isTrailingHashesSection()) {
        md->append(" ");
        for(int i=0; i<note->getDepth()+1; i++) {
            md->append("#");
        }
    }
    md->append("\n");

    if(note->isPostDeclaredSection()) {
        int w=note->getName().size()<2?2:note->getName().size();
        const char* c = note->getDepth()?"-":"=";
        for(int i=0; i<w; i++) {
            md->append(c);
        }
        // IMPROVE endl
        md->append("\n");
    }

    MF_DEBUG("= BEFORE autolinking =============================" << endl << md << endl);
    toDescription(note, md, autolinking);
    MF_DEBUG("= AFTER autolinking ==============================" << endl << md << endl);

    return md;
}

string* MarkdownOutlineRepresentation::toDescription(const Note* note, string* md, bool autolinking)
{
    if(descriptionInterceptor && autolinking) {
        string amd{};
        amd.reserve(1000);
        descriptionInterceptor->process(note->getDescription(), amd);
        if(md) {
            md->append(amd);
        }
    } else {
        toString(note->getDescription(), *md);
    }

    return md;
}

string* MarkdownOutlineRepresentation::toc(const Outline* outline, bool tags, bool links)
{
    UNUSED_ARG(links);

    string* md = new string{};
    if(outline) {
        for(Note* n:outline->getNotes()) {
            for(int i=0; i<n->getDepth(); i++) {
                md->append("    ");
            }
            if(links) md->append("* [");
            md->append(n->getName());
            if(links) md->append("](#");
            md->append(n->getMangledName());
            if(links) md->append(")");

            if(tags) {
                for(const Tag* t:*n->getTags()) {
                    md->append(" <kbd>");
                    md->append(t->getName());
                    md->append("</kbd>");
                }
            }

            md->append("\n");
        }
    }
    return md;
}

} // m8r namespace

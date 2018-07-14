/*
 knowledge_graph.h     MindForger thinking notebook

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
#ifndef M8R_KNOWLEDGE_GRAPH_H
#define M8R_KNOWLEDGE_GRAPH_H

#include "mind.h"
#include "../model/tag.h"

namespace m8r {

class Mind;

enum class KnowledgeGraphNodeType {
    MIND,
    OUTLINES,
    OUTLINE,
    NOTES,
    NOTE,
    TAGS,
    TAG,
    STENCILS,
    STENCIL,
    LIMBO
};

class KnowledgeGraphNode {

    enum KnowledgeGraphNodeType type;
    std::string name;
    long unsigned color;
    Thing* thing;

public:
    explicit KnowledgeGraphNode(
       KnowledgeGraphNodeType type,
       const std::string& name,
       long unsigned color=0x000000
    ) {
        this->type = type;
        this->name = name;
        this->color = color;
    }
    void setThing(Thing* thing) { this->thing = thing; }
    const std::string& getName() const { return name; }
    long getColor() const { return color; }
    KnowledgeGraphNodeType getType() const { return type; }
    Thing* getThing() const { return thing; }
    Outline* getOutline() const { return static_cast<Outline*>(thing); }
    Note* getNode() const { return static_cast<Note*>(thing); }
};

class KnowledgeSubGraph
{
    KnowledgeGraphNode* centralNode;
    std::vector<KnowledgeGraphNode*> parents;
    std::vector<KnowledgeGraphNode*> children;
    // IMPROVE map<> all;

    // ensure that knowledge graph size (number of nodes) is smaller than limit
    int limit;
    int count;

public:
    explicit KnowledgeSubGraph(KnowledgeGraphNode* centralNode, int limit=INT_MAX);
    KnowledgeSubGraph(const KnowledgeSubGraph&) = delete;
    KnowledgeSubGraph(const KnowledgeSubGraph&&) = delete;
    KnowledgeSubGraph &operator=(const KnowledgeSubGraph&) = delete;
    KnowledgeSubGraph &operator=(const KnowledgeSubGraph&&) = delete;
    ~KnowledgeSubGraph() {}

    void setCentralNode(KnowledgeGraphNode* centralNode) { this->centralNode = centralNode; }
    KnowledgeGraphNode* getCentralNode() const { return centralNode; }
    // IMPROVE bool contains(KnowledgeGraphNode node) { return false; }
    void addParent(KnowledgeGraphNode* p) { if(count-- > 0) parents.push_back(p); }
    void addChild(KnowledgeGraphNode* c) { if(count-- > 0) children.push_back(c); }
    std::vector<KnowledgeGraphNode*>& getParents() { return parents; }
    std::vector<KnowledgeGraphNode*>& getChildren() { return children; }
    int size() { return 1 + parents.size() + children.size(); }
    void clear() {
        centralNode = nullptr;
        parents.clear();
        children.clear();
        count = limit-1;
    }
};

class KnowledgeGraph
{
    Mind* mind;

    KnowledgeGraphNode* mindNode;
    KnowledgeGraphNode* tagsNode;
    KnowledgeGraphNode* outlinesNode;
    KnowledgeGraphNode* notesNode;
    KnowledgeGraphNode* limboNode;
    KnowledgeGraphNode* stencilsNode;

    long unsigned coreColor;
    long unsigned outlinesColor;
    long unsigned notesColor;

public:
    explicit KnowledgeGraph(
            Mind* mind,
            long unsigned coreColor=0x000000,
            long unsigned outlinesColor=0x220000,
            long unsigned notesColor=0x000022);
    KnowledgeGraph(const KnowledgeGraph&) = delete;
    KnowledgeGraph(const KnowledgeGraph&&) = delete;
    KnowledgeGraph &operator=(const KnowledgeGraph&) = delete;
    KnowledgeGraph &operator=(const KnowledgeGraph&&) = delete;
    ~KnowledgeGraph();

    KnowledgeGraphNode* getNode(KnowledgeGraphNodeType type);
    void getRelatedNodes(KnowledgeGraphNode* centralNode, KnowledgeSubGraph& subgraph);
};

}
#endif // M8R_KNOWLEDGE_GRAPH_H

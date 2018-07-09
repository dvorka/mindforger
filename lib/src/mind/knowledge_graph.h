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
    STENCILS,
    LIMBO
};

class KnowledgeGraphNode {

    enum KnowledgeGraphNodeType type;
    std::string name;
    long unsigned color;
    Thing* thing;

public:
    explicit KnowledgeGraphNode(KnowledgeGraphNodeType type, const std::string& name, long unsigned color=0x000000) {
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

public:
    //explicit KnowledgeSubGraph() {}
    explicit KnowledgeSubGraph(KnowledgeGraphNode* centralNode);
    KnowledgeSubGraph(const KnowledgeSubGraph&) = delete;
    KnowledgeSubGraph(const KnowledgeSubGraph&&) = delete;
    KnowledgeSubGraph &operator=(const KnowledgeSubGraph&) = delete;
    KnowledgeSubGraph &operator=(const KnowledgeSubGraph&&) = delete;
    ~KnowledgeSubGraph() {}

    void setCentralNode(KnowledgeGraphNode* centralNode) { this->centralNode = centralNode; }
    KnowledgeGraphNode* getCentralNode() const { return centralNode; }
    // IMPROVE bool contains(KnowledgeGraphNode node) { return false; }
    void addParent(KnowledgeGraphNode* p) { parents.push_back(p); }
    void addChild(KnowledgeGraphNode* c) { children.push_back(c); }
    std::vector<KnowledgeGraphNode*>& getParents() { return parents; }
    std::vector<KnowledgeGraphNode*>& getChildren() { return children; }
    void clear() {
        centralNode = nullptr;
        parents.clear();
        children.clear();
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

public:
    explicit KnowledgeGraph(Mind* mind);
    KnowledgeGraph(const KnowledgeGraph&) = delete;
    KnowledgeGraph(const KnowledgeGraph&&) = delete;
    KnowledgeGraph &operator=(const KnowledgeGraph&) = delete;
    KnowledgeGraph &operator=(const KnowledgeGraph&&) = delete;
    ~KnowledgeGraph();

    KnowledgeGraphNode* getNode(KnowledgeGraphNodeType type);
    void getRelatedNodes(KnowledgeGraphNode* centralNode, KnowledgeSubGraph& subgraph, int hops=1);
};

}
#endif // M8R_KNOWLEDGE_GRAPH_H

/*
 knowledge_graph.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "knowledge_graph.h"

namespace m8r {

/*
 * Knowledge SUB graph
 */

KnowledgeSubGraph::KnowledgeSubGraph(KnowledgeGraphNode* centralNode)
{
    this->centralNode = centralNode;
}

/*
 * Knowledge graph
 */

KnowledgeGraph::KnowledgeGraph(Mind* mind)
    : mind{mind}
{
    mindNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::MIND, "MIND"};
    tagsNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::TAGS, "tags"};
    outlinesNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::OUTLINES, "outlines"};
    notesNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::NOTES, "notes"};
    limboNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::LIMBO, "limbo"};
    stencilsNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::STENCILS, "stencils"};
}

KnowledgeGraph::~KnowledgeGraph()
{
    delete mindNode;
    delete tagsNode;
    delete outlinesNode;
    delete notesNode;
    delete limboNode;
    delete stencilsNode;
}

KnowledgeGraphNode* KnowledgeGraph::getNode(KnowledgeGraphNodeType type)
{
    switch(type) {
    case KnowledgeGraphNodeType::MIND:
        return mindNode;
    case KnowledgeGraphNodeType::OUTLINES:
        return outlinesNode;
    case KnowledgeGraphNodeType::OUTLINE:
        return nullptr;
    case KnowledgeGraphNodeType::NOTES:
        return notesNode;
    case KnowledgeGraphNodeType::NOTE:
        return nullptr;
    case KnowledgeGraphNodeType::TAGS:
        return tagsNode;
    case KnowledgeGraphNodeType::STENCILS:
        return stencilsNode;
    case KnowledgeGraphNodeType::LIMBO:
        return limboNode;
    }

    return nullptr;
}

void KnowledgeGraph::getRelatedNodes(KnowledgeGraphNode* centralNode, KnowledgeSubGraph& subgraph, int hops)
{
    UNUSED_ARG(hops);

    subgraph.clear();

    if(centralNode == mindNode) {
        subgraph.setCentralNode(mindNode);

        subgraph.addChild(tagsNode);
        subgraph.addChild(outlinesNode);
        subgraph.addChild(notesNode);
        subgraph.addChild(stencilsNode);
        subgraph.addChild(limboNode);

        return;
    } else if(centralNode == outlinesNode) {
        subgraph.setCentralNode(outlinesNode);

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == notesNode) {
        subgraph.setCentralNode(notesNode);

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == tagsNode) {
        subgraph.setCentralNode(tagsNode);

        std::vector<const Tag*>& tags = mind->getTags().values();
        KnowledgeGraphNode* n;
        for(const Tag* t:tags) {
            // TODO: reuse and delete - map<Thing*,Node*>
            n = new KnowledgeGraphNode{KnowledgeGraphNodeType::TAGS, t->getName(), t->getColor().asLong()};
            subgraph.addChild(n);
        }

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == stencilsNode) {
        subgraph.setCentralNode(stencilsNode);

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == limboNode) {
        subgraph.setCentralNode(limboNode);

        subgraph.addParent(mindNode);

        return;
    }

    if(centralNode->getType() == KnowledgeGraphNodeType::OUTLINE) {

        return;
    } else if(centralNode->getType() == KnowledgeGraphNodeType::OUTLINE) {

        return;
    }
}

} // m8r namespace

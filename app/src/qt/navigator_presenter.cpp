/*
 navigator_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "navigator_presenter.h"

namespace m8r {

NavigatorPresenter::NavigatorPresenter(NavigatorView* view, QObject* parent, KnowledgeGraph* knowledgeGraph)
    : QObject(parent),
      view{view},
      knowledgeGraph{knowledgeGraph},
      // TODO improve configuration
      subgraph{knowledgeGraph->getNode(KnowledgeGraphNodeType::MIND), Configuration::getInstance().getNavigatorMaxNodes()}
{
    // signals
    QObject::connect(view, SIGNAL(nodeSelectedSignal(NavigatorNode*)), this, SLOT(slotNodeSelected(NavigatorNode*)));
}

NavigatorPresenter::~NavigatorPresenter()
{
}

void NavigatorPresenter::showInitialView()
{
    subgraph.setMaxNodes(Configuration::getInstance().getNavigatorMaxNodes());
    knowledgeGraph->getRelatedNodes(knowledgeGraph->getNode(KnowledgeGraphNodeType::MIND), subgraph);
    view->refreshOnNextTimerTick(&subgraph);
}

void NavigatorPresenter::showInitialView(Outline* outline)
{
    subgraph.setMaxNodes(Configuration::getInstance().getNavigatorMaxNodes());
    knowledgeGraph->getRelatedNodes(knowledgeGraph->getNode(outline), subgraph);
    view->refreshOnNextTimerTick(&subgraph);
}

void NavigatorPresenter::showInitialView(Note* note)
{
    subgraph.setMaxNodes(Configuration::getInstance().getNavigatorMaxNodes());
    knowledgeGraph->getRelatedNodes(knowledgeGraph->getNode(note), subgraph);
    view->refreshOnNextTimerTick(&subgraph);
}

void NavigatorPresenter::showInitialViewTags()
{
    subgraph.setMaxNodes(Configuration::getInstance().getNavigatorMaxNodes());
    knowledgeGraph->getRelatedNodes(knowledgeGraph->getNode(KnowledgeGraphNodeType::TAGS), subgraph);
    view->refreshOnNextTimerTick(&subgraph);
}

void NavigatorPresenter::shuffle()
{
    view->shuffle();
}

void NavigatorPresenter::slotNodeSelected(NavigatorNode* node)
{
    knowledgeGraph->getRelatedNodes(node->getKnowledgeGraphNode(), subgraph);
    view->refreshOnNextTimerTick(&subgraph);
    switch(node->getKnowledgeGraphNode()->getType()) {
    case KnowledgeGraphNodeType::OUTLINE:
        emit signalOutlineSelected(static_cast<Outline*>(node->getKnowledgeGraphNode()->getThing()));
        break;
    case KnowledgeGraphNodeType::NOTE:
        emit signalNoteSelected(static_cast<Note*>(node->getKnowledgeGraphNode()->getThing()));
        break;
    default:
        emit signalThingSelected();
        break;
    }
}

} // m8r namespace

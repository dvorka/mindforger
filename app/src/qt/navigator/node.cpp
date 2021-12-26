/*
 node.cpp     MindForger thinking notebook

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

 ****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** BSD License Usage
 ** Alternatively, you may use this file under the terms of the BSD license
 ** as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of The Qt Company Ltd nor the names of its
 **     contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "edge.h"
#include "node.h"
#include "navigator_view.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

namespace m8r {

NavigatorNode::NavigatorNode(
        KnowledgeGraphNode* knowledgeGraphNode,
        NavigatorView* navigator,
        const QColor& color,
        bool bold
    )
    : navigator(navigator),
      knowledgeGraphNode(knowledgeGraphNode),
      nodeColor(color),
      showBold(bold)
{
    nodeName = QString::fromStdString(knowledgeGraphNode->getName());
    showType = true;
    switch(knowledgeGraphNode->getType()) {
    case KnowledgeGraphNodeType::TAG:
        nodeType.append("t");
        break;
    case KnowledgeGraphNodeType::OUTLINE:
        nodeType.append("o");
        break;
    case KnowledgeGraphNodeType::NOTE:
        nodeType.append("n");
        break;
    default:
        showType = false;
        break;
    }
    if(knowledgeGraphNode->getCardinality()) {
        nodeCardinality = QString::number(knowledgeGraphNode->getCardinality());
        nodeCardinalityPixelWidth = (log10(knowledgeGraphNode->getCardinality()) - 1) * 9;
    } else {
        nodeCardinalityPixelWidth = 0;
    }

    // Qt flags
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);

    // ensure nodes are rendered above edges: higher Z makes item rendered later i.e. above items w/ lower Z
    setZValue(2);
}

void NavigatorNode::addEdge(NavigatorEdge* edge)
{
	edgeList << edge;
	edge->adjust();
}

QList<NavigatorEdge *> NavigatorNode::edges() const
{
	return edgeList;
}

/**
 * @brief Force-drive graph: repulse magnets and rubber bands.
 *
 * IMPORTANT: method primarily calculates RELATIVE coordinates change for this node (vectors),
 *            finally it calculates new node position which is stored to newPos
 */
void NavigatorNode::calculateForces()
{
	if (!scene() || scene()->mouseGrabberItem() == this) {
		newPos = pos();
		return;
	}

    // NODES ~ REPULSE MAGNETS: sum up all forces pushing this node AWAY
    qreal xVelocity = 0;
    qreal yVelocity = 0;
    foreach(QGraphicsItem* otherItem, scene()->items()) {
        NavigatorNode* otherNode = qgraphicsitem_cast<NavigatorNode*>(otherItem);
        if(!otherNode) {
			continue;
        }

        // this node's coordinate system:
        // - this node is in [0, 0]
        // - other node coordinate is in fact vector:
        //   > this node is at the beginning of the vector
        //   > other node is at the end of other vector
        QPointF vector = mapToItem(otherNode, 0, 0);
        qreal dx = vector.x();
        qreal dy = vector.y();
        // formula that calculates and ADDs forces driving this node away in X and Y direction
        double l = 2.0 * (dx*dx + dy*dy);
        // if nodes have DIFFERENT coordinates, then add AWAY forces
        if(l > 0) {
            xVelocity += (dx * navigator->getInitialEdgeLenght()) / l;
            yVelocity += (dy * navigator->getInitialEdgeLenght()) / l;
        }
	}

    // EDGES ~ RUBBER BANDS: sum up all forces pulling this node TOGETHER & SUBSTRACT it from forces pulling items together
	double weight = (edgeList.size() + 1) * 10;
    foreach(NavigatorEdge* edge, edgeList) {
        QPointF vector;
        if(edge->getSrcNode() == this) {
            vector = mapToItem(edge->getDstNode(), 0, 0);
        } else {
            vector = mapToItem(edge->getSrcNode(), 0, 0);
        }
        // substract TOGETHER forces from away forces
        xVelocity -= vector.x() / weight;
        yVelocity -= vector.y() / weight;
	}

    // round velocity to avoid moving FOREVER
    // - stops redraw if change is small
    // - higher threshold avoids "floating" graph moving a tiny fraction of pixels
    // - original value 0.1
    if(qAbs(xVelocity) < 0.3 && qAbs(yVelocity) < 0.3) {
        xVelocity = yVelocity = 0;
    }

    // ensure node fits in scene
	QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xVelocity, yVelocity);
	newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
	newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));

    // newPos contains new node position which will be applied on advance() invocation
}

/**
 * @brief Apply new position calculated by calculateForces()
 */
bool NavigatorNode::advance()
{
    if (newPos == pos()) {
		return false;
    }

	setPos(newPos);

	return true;
}

// IMPORTANT boundingRect MUST be sect correctly, otherwise this node rendering is CLIPPED (text or shape)
QRectF NavigatorNode::boundingRect() const
{
    qreal adjust = 2; // spacing + type rect
    qreal typeAdjust = 20;
    return QRectF(
                -defaultNodeWidth/2 - adjust,
                -defaultNodeHeight/2 - adjust - typeAdjust,
                defaultNodeWidth + adjust + typeAdjust + 10+nodeCardinalityPixelWidth,
                defaultNodeHeight + adjust + 2*typeAdjust);
}

/**
 * @brief Shape size is used to get vieport for accepting click events related to this graphics object.
 */
QPainterPath NavigatorNode::shape() const
{
	QPainterPath path;
    path.addRect(-defaultNodeWidth/2, -defaultNodeHeight/2, defaultNodeWidth, defaultNodeHeight);
	return path;
}

// IMPORTANT this method must be FAST - every CPU tick matters
void NavigatorNode::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    // IMPROVE color and shape driven by node type

    // IMPROVE get font from navigator
    //navigator->getNodeFont();
    // IMPROVE calculate this only ONCE - font to be passed from parent and used through all the code
    QFont font = painter->font();
    font.setPointSize(10);
    font.setBold(showBold);
    painter->setFont(font);

    QFontMetrics fm = painter->fontMetrics();
    qreal PADDING_WIDTH = 10;
    qreal PADDING_HEIGHT = 6;
    int textWidth = fm.width(nodeName);
    int textHeight = fm.height();
    defaultNodeWidth = textWidth + PADDING_WIDTH;
    defaultNodeHeight = textHeight + PADDING_HEIGHT;

    // node name
    // rectangle
    QRectF rectF{-defaultNodeWidth/2,-defaultNodeHeight/2, defaultNodeWidth, defaultNodeHeight};
    painter->setPen(QPen(Qt::darkGray, 0));
    painter->setBrush(nodeColor);
    if(showType) {
        painter->drawRect(rectF);
    } else {
        painter->drawRoundRect(rectF);
    }
    // text
    //   IMPORTANT: check boundingRect() to ensure text is NOT clipped
    //   IMPROVE detect white to avoid white on white
    painter->setPen(Qt::white);
    painter->drawText((-defaultNodeWidth+PADDING_WIDTH)/2, 0+PADDING_HEIGHT/2+2, nodeName);

    // node type
    if(showType) {
        // rectangle
        QRectF rectT{defaultNodeWidth/2 - 5, defaultNodeHeight/2 - 5, 15, 15};
        painter->setPen(QPen(Qt::black, 0));
        painter->setBrush(Qt::black);
        painter->drawRect(rectT);
        // text
        painter->setPen(Qt::white);
        int textPadding = 7;
        painter->drawText(defaultNodeWidth/2 - 5 + textPadding - 3, defaultNodeHeight/2 + textPadding, nodeType);
    }

    // node cardinality
    if(nodeCardinalityPixelWidth) {
        // rectangle
        QRectF rectT{defaultNodeWidth/2 - 5, -20, 15+9 + nodeCardinalityPixelWidth, 15};
        painter->setPen(QPen(Qt::white, 0));
        painter->setBrush(Qt::white);
        painter->drawRect(rectT);
        // text
        painter->setPen(Qt::black);
        painter->drawText(defaultNodeWidth/2, -8, nodeCardinality);
    }
}

QVariant NavigatorNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch(change) {
	case ItemPositionHasChanged:
        foreach (NavigatorEdge *edge, edgeList) {
			edge->adjust();
        }
        navigator->itemMoved();
		break;
	default:
		break;
	};

	return QGraphicsItem::itemChange(change, value);
}

void NavigatorNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	update();
	QGraphicsItem::mousePressEvent(event);
}

void NavigatorNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton)) {
        dragging=true;
    }

    update();
    QGraphicsItem::mouseMoveEvent(event);
}

void NavigatorNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);

    if(event->button() == Qt::LeftButton) {
        if(dragging) {
            // drag & drop
            dragging = false;
        }
        else {
            // click ~ node selected ~ let navigator know about node selection
            navigator->iWasSelected(this);
        }
    }
}

} // m8r namespace

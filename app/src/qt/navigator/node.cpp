/*
 node.cpp     MindForger thinking notebook

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

NavigatorNode::NavigatorNode(const QString& name, NavigatorView* navigator, const QColor& color)
    : nodeName(name), navigator(navigator), nodeColor(color)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);

    // ensure nodes are rendered above edges: higher Z makes item rendered later i.e. above items w/ lower Z
    setZValue(2);
}

void NavigatorNode::addEdge(NavigatorEdge *edge)
{
	edgeList << edge;
	edge->adjust();
}

QList<NavigatorEdge *> NavigatorNode::edges() const
{
	return edgeList;
}

/**
 * Force-drive graph: magnets and rubber bands
 */
void NavigatorNode::calculateForces()
{
	if (!scene() || scene()->mouseGrabberItem() == this) {
		newPos = pos();
		return;
	}

    // NODES ~ MAGNETS: sum up all forces pushing this item away
	qreal xvel = 0;
	qreal yvel = 0;
    foreach(QGraphicsItem* item, scene()->items()) {
        NavigatorNode* node = qgraphicsitem_cast<NavigatorNode*>(item);
        if(!node) {
			continue;
        }

		QPointF vec = mapToItem(node, 0, 0);
		qreal dx = vec.x();
		qreal dy = vec.y();
        double l = 2.0 * (dx*dx + dy*dy);
        if(l > 0) {
			xvel += (dx * 150.0) / l;
			yvel += (dy * 150.0) / l;
		}
	}

    // EDGES ~ RUBBERS: now subtract all forces pulling items together
	double weight = (edgeList.size() + 1) * 10;
    foreach(NavigatorEdge *edge, edgeList) {
		QPointF vec;
        if(edge->getSrcNode() == this)
            vec = mapToItem(edge->getDstNode(), 0, 0);
		else
            vec = mapToItem(edge->getSrcNode(), 0, 0);
		xvel -= vec.x() / weight;
		yvel -= vec.y() / weight;
	}

    if(qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1) {
		xvel = yvel = 0;
    }

    // recalculate position
	QRectF sceneRect = scene()->sceneRect();
	newPos = pos() + QPointF(xvel, yvel);
	newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
	newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
}

bool NavigatorNode::advance()
{
	if (newPos == pos())
		return false;

	setPos(newPos);
	return true;
}

// IMPORTANT bondingRect MUST be sect correctly, otherwise drawing is CLIPPED (text or shape)
QRectF NavigatorNode::boundingRect() const
{
    qreal adjust = 2;

    //return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
    return QRectF( -nodeWidth/2 - adjust, -nodeHeight/2 - adjust, nodeWidth + adjust, nodeHeight + adjust);
}

QPainterPath NavigatorNode::shape() const
{
	QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
	return path;
}

// IMPORTANT this method must be FAST - every CPU tick matters
void NavigatorNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    // IMPROVE color and shape driven by node type

    // IMPROVE get font from navigator
    //navigator->getNodeFont();
    // IMPROVE calculate this only ONCE - font to be passed from parent and used through all the code
    QFont font = painter->font();
    font.setPointSize(10);
    painter->setFont(font);

    QFontMetrics fm = painter->fontMetrics();
    qreal PADDING_WIDTH = 10;
    qreal PADDING_HEIGHT = 6;
    int textWidth = fm.width(nodeName);
    int textHeight = fm.height();
    nodeWidth = textWidth + PADDING_WIDTH;
    nodeHeight = textHeight + PADDING_HEIGHT;

    // TODO measure text (constructor) width and rectangle width driven by width w/ a limit and trailing ...

    // rectangle
    QRectF rectF{-nodeWidth/2,-nodeHeight/2, nodeWidth, nodeHeight};
    painter->setPen(QPen(Qt::darkGray, 0));
    painter->setBrush(nodeColor);
    painter->drawRect(rectF);

    // label ... IMPORTANT: check boundingRect() to ensure text is NOT clipped
    // IMPROVE detect white to avoid white on white
    painter->setPen(Qt::white);
    painter->drawText((-nodeWidth+PADDING_WIDTH)/2, 0+PADDING_HEIGHT/2+2, nodeName);
}

void NavigatorNode::paintCircleWithShade(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::yellow).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
    } else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
}

QVariant NavigatorNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) {
	case ItemPositionHasChanged:
        foreach (NavigatorEdge *edge, edgeList)
			edge->adjust();
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

void NavigatorNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}

} // m8r namespace

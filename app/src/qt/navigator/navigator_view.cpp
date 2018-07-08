/*
 mind-navigator.cpp     MindForger thinking notebook

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

#include "navigator_view.h"
#include "edge.h"
#include "node.h"

#include <math.h>

#include <QKeyEvent>

namespace m8r {

NavigatorView::NavigatorView(QWidget* parent)
    : QGraphicsView(parent), timerId{0}, renderLegend{true}
{    
    // scene is peephole rectangle to the whole view (QGraphicsView)
    navigatorScene = new QGraphicsScene(this);
    navigatorScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(navigatorScene);

	setCacheMode(CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);

    // signals
    // on Node selection signals MUST be used (instead of a method call) as naviagor may destroy nodes on refresh
    QObject::connect(this, SIGNAL(nodeSelectedSignal(NavigatorNode*)), this, SLOT(refreshOnNodeSelection(NavigatorNode*)));
}

void NavigatorView::refresh(std::vector<const m8r::Tag*>& tags)
{
    // IMPROVE: resize scene also on window resize event
    w = static_cast<QWidget*>(parent())->width();
    h = static_cast<QWidget*>(parent())->height();
    // the first two args specify location of the [0,0]
    navigatorScene->setSceneRect(-w/4, -h/4, w, h);
    // scale scene to avoid scroll bars
    scale(qreal(0.99), qreal(0.99));
    //setMinimumSize(WIDTH, HEIGHT);

    navigatorScene->clear();

    // central node
    mindNode = new NavigatorNode(QString{"MIND"}, this, Qt::black, true);
    navigatorScene->addItem(mindNode);
    mindNode->setPos(0, 0);

    tagsNode = addMindNode(QString{"tags"});
    outlinesNode = addMindNode(QString{"outlines"});
    notesNode = addMindNode(QString{"notes"});
    limboNode = addMindNode(QString{"limbo"});
    stencilsNode = addMindNode(QString{"stencils"});

    NavigatorNode* n;
    NavigatorEdge* e;
    for(const m8r::Tag* t:tags) {
        // TODO set color
        // TODO set name
        // TODO set type
        n = new NavigatorNode(QString::fromStdString(t->getName()), this, QColor(t->getColor().asLong()));
        navigatorScene->addItem(n);
        e = new NavigatorEdge(tagsNode, n);
        navigatorScene->addItem(e);
        n->setPos(qrand()%w, qrand()%h);
    }
}

NavigatorNode* NavigatorView::addMindNode(const QString& label)
{
    NavigatorNode* n = new NavigatorNode(label, this, Qt::black, true);
    navigatorScene->addItem(n);
    n->setPos(qrand()%w, qrand()%h);
    navigatorScene->addItem(new NavigatorEdge(mindNode, n));
    return n;
}

void NavigatorView::refresh(m8r::Outline* o)
{
    UNUSED_ARG(o);
}

void NavigatorView::refreshOnNodeSelection(NavigatorNode* selectedNode)
{
    // TODO synchronize timer event and delete

//    navigatorScene->clear();

    for(QGraphicsItem* gi:items()) {
        if(gi != selectedNode) {
            scene()->removeItem(gi);
            delete gi;
        }
    }
}

void NavigatorView::itemMoved()
{
    if(!timerId) {
        // IMPROVE dynamic refresh rate
        timerId = startTimer(1000/25);
    }
}

void NavigatorView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {

    // IMPROVE choose last selected node and move that selected node
//	case Qt::Key_Up:
//        mindNode->moveBy(0, -20);
//		break;
//	case Qt::Key_Down:
//        mindNode->moveBy(0, 20);
//		break;
//	case Qt::Key_Left:
//        mindNode->moveBy(-20, 0);
//		break;
//	case Qt::Key_Right:
//        mindNode->moveBy(20, 0);
//		break;

	case Qt::Key_Plus:
		zoomIn();
		break;
	case Qt::Key_Minus:
		zoomOut();
		break;
	case Qt::Key_Space:
	case Qt::Key_Enter:
		shuffle();
		break;
	default:
		QGraphicsView::keyPressEvent(event);
	}
}

void NavigatorView::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);

    QList<NavigatorNode*> nodes;
    foreach(QGraphicsItem *item, scene()->items()) {
        if(NavigatorNode *node = qgraphicsitem_cast<NavigatorNode *>(item))
			nodes << node;
	}

    foreach(NavigatorNode *node, nodes)
		node->calculateForces();

	bool itemsMoved = false;
    foreach(NavigatorNode *node, nodes) {
        if(node->advance())
			itemsMoved = true;
	}

    if(!itemsMoved) {
		killTimer(timerId);
		timerId = 0;
	}
}

#ifndef QT_NO_WHEELEVENT
void NavigatorView::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, -event->delta() / 240.0));
}
#endif

void NavigatorView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);

/*
    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
            sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
               "wheel or the '+' and '-' keys"));

	QFont font = painter->font();
	font.setBold(true);
	font.setPointSize(14);
	painter->setFont(font);
	painter->setPen(Qt::lightGray);
	painter->drawText(textRect.translated(2, 2), message);
	painter->setPen(Qt::black);
	painter->drawText(textRect, message);
*/
}

void NavigatorView::scaleView(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

	scale(scaleFactor, scaleFactor);
}

void NavigatorView::shuffle()
{
	foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<NavigatorNode *>(item))
			item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
	}
}

void NavigatorView::zoomIn()
{
	scaleView(qreal(1.2));
}

void NavigatorView::zoomOut()
{
	scaleView(1 / qreal(1.2));
}

}  // m8r namespace

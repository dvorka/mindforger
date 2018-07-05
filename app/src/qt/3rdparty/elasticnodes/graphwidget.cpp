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

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <math.h>

#include <QKeyEvent>

//using namespace std;
//using namespace m8r;

static int WIDTH = 1920;
static int HEIGHT = 1080;

NavigatorView::NavigatorView(QWidget *parent)
	: QGraphicsView(parent), timerId(0)
{
    // scene is peephole rectangle to the whole view (QGraphicsView)
    navigatorScene = new QGraphicsScene(this);
    navigatorScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    // the first two args specify location of the [0,0]
    navigatorScene->setSceneRect(-WIDTH/4, -HEIGHT/4, WIDTH, HEIGHT);
    setScene(navigatorScene);

	setCacheMode(CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
    //scale(qreal(0.8), qreal(0.8));
    setMinimumSize(WIDTH, HEIGHT);
}

void NavigatorView::refresh(std::vector<const m8r::Tag*> tags)
{
    navigatorScene->clear();

    // central node
    mfNode = new Node(QString{"MF"}, this);
    navigatorScene->addItem(mfNode);
    mfNode->setPos(0, 0);

    int avoidIdenticalPosition{};
    Node* n;
    for(const m8r::Tag* t:tags) {
        // TODO set color
        // TODO set name
        // TODO set type
        n = new Node(QString::fromStdString(t->getName()), this);
        navigatorScene->addItem(n);
        // TODO make edges directed
        navigatorScene->addItem(new Edge(mfNode, n));
        n->setPos(-avoidIdenticalPosition, -avoidIdenticalPosition);

        avoidIdenticalPosition += 10;
    }

//    Node *node1 = new Node(this);
//	Node *node2 = new Node(this);
//	Node *node3 = new Node(this);
//	Node *node4 = new Node(this);
//	centerNode = new Node(this);
//	Node *node6 = new Node(this);
//	Node *node7 = new Node(this);
//	Node *node8 = new Node(this);
//	Node *node9 = new Node(this);
//	scene->addItem(node1);
//	scene->addItem(node2);
//	scene->addItem(node3);
//	scene->addItem(node4);
//	scene->addItem(centerNode);
//	scene->addItem(node6);
//	scene->addItem(node7);
//	scene->addItem(node8);
//	scene->addItem(node9);
//	scene->addItem(new Edge(node1, node2));
//	scene->addItem(new Edge(node2, node3));
//	scene->addItem(new Edge(node2, centerNode));
//	scene->addItem(new Edge(node3, node6));
//	scene->addItem(new Edge(node4, node1));
//	scene->addItem(new Edge(node4, centerNode));
//	scene->addItem(new Edge(centerNode, node6));
//	scene->addItem(new Edge(centerNode, node8));
//	scene->addItem(new Edge(node6, node9));
//	scene->addItem(new Edge(node7, node4));
//	scene->addItem(new Edge(node8, node7));
//	scene->addItem(new Edge(node9, node8));

//	node1->setPos(-50, -50);
//	node2->setPos(0, -50);
//	node3->setPos(50, -50);
//	node4->setPos(-50, 0);
//	centerNode->setPos(0, 0);
//	node6->setPos(50, 0);
//	node7->setPos(-50, 50);
//	node8->setPos(0, 50);
//	node9->setPos(50, 50);
}

void NavigatorView::refresh(m8r::Outline* o)
{

}

void NavigatorView::itemMoved()
{
	if (!timerId)
		timerId = startTimer(1000 / 25);
}

void NavigatorView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up:
		centerNode->moveBy(0, -20);
		break;
	case Qt::Key_Down:
		centerNode->moveBy(0, 20);
		break;
	case Qt::Key_Left:
		centerNode->moveBy(-20, 0);
		break;
	case Qt::Key_Right:
		centerNode->moveBy(20, 0);
		break;
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

	QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
		if (Node *node = qgraphicsitem_cast<Node *>(item))
			nodes << node;
	}

	foreach (Node *node, nodes)
		node->calculateForces();

	bool itemsMoved = false;
	foreach (Node *node, nodes) {
		if (node->advance())
			itemsMoved = true;
	}

	if (!itemsMoved) {
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
	Q_UNUSED(rect);

//	// Shadow
    QRectF sceneRect = this->sceneRect();
//	QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
//	QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
//	if (rightShadow.intersects(rect) || rightShadow.contains(rect))
//		painter->fillRect(rightShadow, Qt::darkGray);
//	if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
//		painter->fillRect(bottomShadow, Qt::darkGray);

//	// Fill
//	QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
//	gradient.setColorAt(0, Qt::white);
//	gradient.setColorAt(1, Qt::lightGray);
//	painter->fillRect(rect.intersected(sceneRect), gradient);
//	painter->setBrush(Qt::NoBrush);
//	painter->drawRect(sceneRect);

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
		if (qgraphicsitem_cast<Node *>(item))
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

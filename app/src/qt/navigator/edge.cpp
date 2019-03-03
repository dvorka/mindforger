/*
 edge.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <math.h>

#include <QPainter>

namespace m8r {

static const double PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0*PI;

NavigatorEdge::NavigatorEdge(NavigatorNode* source, NavigatorNode* destination)
	: arrowSize(10)
{
	setAcceptedMouseButtons(0);
    srcNode = source;
    dstNode = destination;
    srcNode->addEdge(this);
    dstNode->addEdge(this);
	adjust();

    // ensure nodes are rendered above edges
    setZValue(1);
}

NavigatorNode *NavigatorEdge::getSrcNode() const
{
    return srcNode;
}

NavigatorNode *NavigatorEdge::getDstNode() const
{
    return dstNode;
}

void NavigatorEdge::adjust()
{
    if (!srcNode || !dstNode)
		return;

    QLineF line(mapFromItem(srcNode, 0, 0), mapFromItem(dstNode, 0, 0));
	qreal length = line.length();

	prepareGeometryChange();

	if (length > qreal(20.)) {
		QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        srcPoint = line.p1() + edgeOffset;
        dstPoint = line.p2() - edgeOffset;
	} else {
        srcPoint = dstPoint = line.p1();
	}
}

QRectF NavigatorEdge::boundingRect() const
{
    if (!srcNode || !dstNode)
		return QRectF();

	qreal penWidth = 1;
	qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(
                srcPoint,
                QSizeF(dstPoint.x() - srcPoint.x(), dstPoint.y() - srcPoint.y()))
		.normalized()
		.adjusted(-extra, -extra, extra, extra);
}

// MindRaider style edge: draw triangle that represents oriented arrow
void NavigatorEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!srcNode || !dstNode)
        return;

    // draw arrow: line + 1/3 of edge length tringle

    // line
    painter->setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(srcPoint, dstPoint);

    // 1/3 triangle
    qreal dx{dstPoint.rx()-srcPoint.rx()};
    qreal dy{dstPoint.ry()-srcPoint.ry()};
    qreal ix{}, iy{};
    dx /= -3.0;
    dy /= -3.0;
    QPointF srcTriPoint{dstPoint.rx()+dx, dstPoint.ry()+dy};
    if(qAbs(dx) < qAbs(dy)) {
        ix = 5;
    } else {
        iy = 5;
    }
    QPointF s1{srcTriPoint.rx()-ix, srcTriPoint.ry()-iy};
    QPointF s2{srcTriPoint.rx()+ix, srcTriPoint.ry()+iy};

    painter->setBrush(Qt::darkGray);
    painter->drawPolygon(QPolygonF() << s1 << s2 << dstPoint);
}


// IMPROVE: NavigateEdge::paintLine ... is simpler than triangle ~ just w/o deltas

void NavigatorEdge::paintTriangle(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!srcNode || !dstNode)
        return;

    QPointF s1{srcPoint.rx()-5, srcPoint.ry()};
    QPointF s2{srcPoint.rx()+5, srcPoint.ry()};
    painter->setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(Qt::darkGray);
    painter->drawPolygon(QPolygonF() << s1 << s2 << dstPoint);
}

// Line w/ two arrows edge
void NavigatorEdge::paintLineWithTwoArrows(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!srcNode || !dstNode)
		return;

    QLineF line(srcPoint, dstPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // draw the line itself
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

    // draw the arrows
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TWO_PI - angle;

    QPointF sourceArrowP1 = srcPoint + QPointF(sin(angle + PI / 3) * arrowSize,
                              cos(angle + PI / 3) * arrowSize);
    QPointF sourceArrowP2 = srcPoint + QPointF(sin(angle + PI - PI / 3) * arrowSize,
                              cos(angle + PI - PI / 3) * arrowSize);
    QPointF destArrowP1 = dstPoint + QPointF(sin(angle - PI / 3) * arrowSize,
                          cos(angle - PI / 3) * arrowSize);
    QPointF destArrowP2 = dstPoint + QPointF(sin(angle - PI + PI / 3) * arrowSize,
                          cos(angle - PI + PI / 3) * arrowSize);

    painter->setBrush(Qt::black);
    painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}

} // m8r namespace

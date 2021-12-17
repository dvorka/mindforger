/*
 mind_navigator.h     MindForger thinking notebook

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

#ifndef M8R_NAVIGATOR_VIEW_H
#define M8R_NAVIGATOR_VIEW_H

#include <mutex>

#include <QGraphicsView>

#include "../../../../lib/src/mind/knowledge_graph.h"
#include "../../../../lib/src/model/outline.h"
#include "../look_n_feel.h"

namespace m8r {

class NavigatorNode;

// IMPROVE enjoy improving this class to achieve maximum rendering performance, there
//         is a lot of space for improvement

/**
 * @brief Knowledge graph navigator view.
 *
 * Knowledge graph is based on force-directed graph based (FDB) - magnets and rubber bands.
 *
 * Synchronization & UI threads: selected node sets subgraph, timerEvent()
 * then refreshes view which avoids the need for extra synchronization.
 *
 * @see http://doc.qt.io/qt-5/qtwidgets-graphicsview-elasticnodes-example.html
 */
class NavigatorView : public QGraphicsView
{
    Q_OBJECT

    static constexpr qreal EDGE_LENGTH_DEFAULT = 300.0;
    static constexpr qreal EDGE_LENGTH_DELTA = 30.0;
    static constexpr qreal EDGE_LENGTH_MAX = 5000.0;
    static constexpr qreal EDGE_LENGTH_MIN = EDGE_LENGTH_DELTA*2.0;

private:
    QGraphicsScene* navigatorScene;

    // IMPROVE is mutex still needed?
    std::mutex refreshMutex;

    int timerId, w, h;

    // stupid and ugly: multi-threading & weak Qt API
    std::vector<QGraphicsItem*> garbageItems;

    // subgraph to be rendered
    KnowledgeSubGraph* subgraph;

    qreal initialEdgeLenght;

    bool isDashboardlet;

public:
    NavigatorView(QWidget* parent, bool isDashboardlet=false);
    ~NavigatorView();

    qreal getInitialEdgeLenght() const { return initialEdgeLenght; }
    void checkAndFixInitialEdgeLength(qreal& l);

    void itemMoved();

    void iWasSelected(NavigatorNode* selectedNode);    
    void refreshOnNextTimerTick(KnowledgeSubGraph* subgraph) {
        std::lock_guard<std::mutex> criticalSection{refreshMutex};

        updateNavigatorView();
        this->subgraph = subgraph;
        itemMoved(); // kick timer if not running
    }
    void refreshOnNextTimerTick() {
        refreshOnNextTimerTick(subgraph);
    }

    void cleanupBeforeHide();

protected:
    void scaleView(qreal scaleFactor);

    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event) override;
#endif
    void mousePressEvent(QMouseEvent* mouseEvent) override;

private:
    void updateNavigatorView();
    void clearGarbageItems();

signals:
    void nodeSelectedSignal(NavigatorNode* selectedNode);
    void clickToSwitchFacet();

public slots:
    void shuffle();
    void zoomIn();
    void zoomOut();
};

}
#endif // M8R_NAVIGATOR_VIEW_H

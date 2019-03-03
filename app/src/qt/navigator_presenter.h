/*
 navigator_presenter.h     MindForger thinking notebook

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
*/
#ifndef M8RUI_NAVIGATOR_PRESENTER_H
#define M8RUI_NAVIGATOR_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/model/tag.h"
#include "../../lib/src/mind/knowledge_graph.h"
#include "navigator/navigator_view.h"
#include "navigator/node.h"

namespace m8r {

class NavigatorPresenter : public QObject
{
    Q_OBJECT

    NavigatorView* view;
    KnowledgeGraph* knowledgeGraph;

    KnowledgeSubGraph subgraph;

public:
    explicit NavigatorPresenter(NavigatorView* view, QObject* parent, KnowledgeGraph* knowledgeGraph);
    NavigatorPresenter(const NavigatorPresenter&) = delete;
    NavigatorPresenter(const NavigatorPresenter&&) = delete;
    NavigatorPresenter &operator=(const NavigatorPresenter&) = delete;
    NavigatorPresenter &operator=(const NavigatorPresenter&&) = delete;
    ~NavigatorPresenter();

    NavigatorView* getView() const { return view; }

    void showInitialView();
    void showInitialView(Outline* outline);
    void showInitialView(Note* note);
    void showInitialViewTags();

    void shuffle();

    void cleanupBeforeHide() { view->cleanupBeforeHide(); }

private slots:
    void nodeSelectedSlot(NavigatorNode* node);

signals:
    void thingSelectedSignal();
    void outlineSelectedSignal(Outline* note);
    void noteSelectedSignal(Note* note);
};

}
#endif // M8RUI_NAVIGATOR_PRESENTER_H

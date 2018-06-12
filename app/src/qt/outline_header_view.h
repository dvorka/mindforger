/*
 outline_header_view.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_HEADER_VIEW_H
#define M8RUI_OUTLINE_HEADER_VIEW_H

#include "../../lib/src/debug.h"

#include <QtWidgets>
#ifdef MF_QT_WEB_ENGINE
  #include <QWebEngineView>
  #include "web_engine_page_link_navigation_policy.h"
#else
  #include <QWebView>
#endif

namespace m8r {

#ifdef MF_QT_WEB_ENGINE
  class OutlineHeaderView : public QWebEngineView
#else
  class OutlineHeaderView : public QWebView
#endif
{
    Q_OBJECT

public:
    explicit OutlineHeaderView(QWidget* parent);
    OutlineHeaderView(const OutlineHeaderView&) = delete;
    OutlineHeaderView(const OutlineHeaderView&&) = delete;
    OutlineHeaderView &operator=(const OutlineHeaderView&) = delete;
    OutlineHeaderView &operator=(const OutlineHeaderView&&) = delete;
    ~OutlineHeaderView();

#ifdef MF_QT_WEB_ENGINE
    QWebEnginePage* getPage() const { return page(); }

// this is ugly and stupid workaround for handling double-click event in QWebEngineView
private:
    QObject *childObj = NULL;
protected:
    bool event(QEvent* evt) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;
#else
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
#endif

signals:
    void signalMouseDoubleClickEvent();
};

}
#endif // M8RUI_OUTLINE_HEADER_VIEW_H

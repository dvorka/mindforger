/*
 outline_header_view.cpp     MindForger thinking notebook

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
#include "outline_header_view.h"

namespace m8r {

using namespace std;

OutlineHeaderView::OutlineHeaderView(QWidget *parent)
#ifdef MF_QT_WEB_ENGINE
    : QWebEngineView(parent)
#else
    : QWebView(parent)
#endif
{
#ifdef MF_QT_WEB_ENGINE
    // TODO QWebEngine click handler to be implemented
#else
    // ensure that link clicks are not handled, but delegated to MF using linkClicked signal
    page()->setLinkDelegationPolicy(QWebPage::LinkDelegationPolicy::DelegateAllLinks);
#endif
}

OutlineHeaderView::~OutlineHeaderView()
{
}

#ifdef MF_QT_WEB_ENGINE

bool OutlineHeaderView::event(QEvent* event)
{
    MF_DEBUG(event->type() << endl);
    if (event->type() == QEvent::ChildPolished) {
        QChildEvent *childEvent = static_cast<QChildEvent*>(event);
        childObj = childEvent->child();
        if (childObj) {
            childObj->installEventFilter(this);
        }
    }

    return QWebEngineView::event(event);
}

bool OutlineHeaderView::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == childObj) {
        if(event->type() == QEvent::MouseButtonDblClick) {
            // double click to Note view opens Note editor
            emit signalMouseDoubleClickEvent();
            return true;
        }
    }

    return QWebEngineView::eventFilter(obj, event);
}

#else

void OutlineHeaderView::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    emit signalMouseDoubleClickEvent();
}

#endif

} // m8r namespace

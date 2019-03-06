/*
 outline_header_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
    // ensure that link clicks are not handled, but delegated to MF using linkClicked signal
    WebEnginePageLinkNavigationPolicy* p = new WebEnginePageLinkNavigationPolicy{this};
    setPage(p);
#else
    // ensure that link clicks are not handled, but delegated to MF using linkClicked signal
    page()->setLinkDelegationPolicy(QWebPage::LinkDelegationPolicy::DelegateAllLinks);
#endif

    // zoom
    setZoomFactor(Configuration::getInstance().getUiHtmlZoomFactor());
}

#ifdef MF_QT_WEB_ENGINE

bool OutlineHeaderView::event(QEvent* event)
{
    // INSTALL event filter to every child - child polished event is received 1+x for every child
    if (event->type() == QEvent::ChildPolished) {
        QChildEvent *childEvent = static_cast<QChildEvent*>(event);
        QObject *childObj = childEvent->child();
        if (childObj) {
            childObj->installEventFilter(this);
        }
    }

    return QWebEngineView::event(event);
}

bool OutlineHeaderView::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonDblClick) {
        // double click to Note view opens Note editor
        emit signalMouseDoubleClickEvent();
        event->accept();
        return true;
    }

    return QWebEngineView::eventFilter(obj, event);
}

void OutlineHeaderView::wheelEvent(QWheelEvent* event)
{
    if(QApplication::keyboardModifiers() & Qt::ControlModifier) {
        if(!event->angleDelta().isNull()) {
            if(event->angleDelta().ry()>0) {
                Configuration::getInstance().incUiHtmlZoom();
            } else {
                Configuration::getInstance().decUiHtmlZoom();
            }
            setZoomFactor(Configuration::getInstance().getUiHtmlZoomFactor());
            event->accept();
            return;
        }
    }

    //OutlineHeaderView::wheelEvent(event);
}

#else

void OutlineHeaderView::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    emit signalMouseDoubleClickEvent();
}

void OutlineHeaderView::wheelEvent(QWheelEvent* event)
{
    if(QApplication::keyboardModifiers() & Qt::ControlModifier) {
        if(!event->angleDelta().isNull()) {
            if(event->angleDelta().ry()>0) {
                Configuration::getInstance().incUiHtmlZoom();
            } else {
                Configuration::getInstance().decUiHtmlZoom();
            }
            setZoomFactor(Configuration::getInstance().getUiHtmlZoomFactor());
            return;
        }
    }

    QWebView::wheelEvent(event);
}

#endif

} // m8r namespace

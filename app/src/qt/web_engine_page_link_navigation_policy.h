/*
 web_engine_page_link_navigation_policy.h     MindForger thinking notebook

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
#ifndef M8RUI_WEB_ENGINE_PAGE_LINK_NAVIGATION_POLICY_H
#define M8RUI_WEB_ENGINE_PAGE_LINK_NAVIGATION_POLICY_H

#include "../lib/src/gear/lang_utils.h"
#include "../lib/src/debug.h"

#include <QtWidgets>
#ifdef MF_QT_WEB_ENGINE
  #include <QWebEngineView>
#endif

namespace m8r {

#ifdef MF_QT_WEB_ENGINE

class WebEnginePageLinkNavigationPolicy : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit WebEnginePageLinkNavigationPolicy(QObject* parent = 0);
    WebEnginePageLinkNavigationPolicy(const WebEnginePageLinkNavigationPolicy&) = delete;
    WebEnginePageLinkNavigationPolicy(const WebEnginePageLinkNavigationPolicy&&) = delete;
    WebEnginePageLinkNavigationPolicy &operator=(const WebEnginePageLinkNavigationPolicy&) = delete;
    WebEnginePageLinkNavigationPolicy &operator=(const WebEnginePageLinkNavigationPolicy&&) = delete;
    ~WebEnginePageLinkNavigationPolicy();

    bool acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool isMainFrame)
    {
#ifdef DO_M8R_DEBUG
        MF_DEBUG("acceptNavigationRequest(" << url << "," << type << "," << isMainFrame << ")" << std::endl);
#else
        UNUSED_ARG(type);
        UNUSED_ARG(isMainFrame);
#endif

        if(type == QWebEnginePage::NavigationTypeLinkClicked) {
            emit signalLinkClicked(url);
            return false;
        } else {
            return true;
        }
    }

signals:
    void signalLinkClicked(const QUrl& url);
};

#endif

}
#endif // M8RUI_WEB_ENGINE_PAGE_LINK_NAVIGATION_POLICY_H

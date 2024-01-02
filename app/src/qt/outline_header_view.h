/*
 outline_header_view.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "../../lib/src/config/configuration.h"

#include "outline_header_view_model.h"
#include "widgets/mf_widgets.h"
#include "widgets/view_to_edit_buttons_panel.h"

#include <QtWidgets>
#ifdef MF_QT_WEB_ENGINE
  #include <QWebEngineView>
  #include "web_engine_page_link_navigation_policy.h"
#else
  #include <QWebView>
#endif

namespace m8r {

#ifdef MF_QT_WEB_ENGINE
class OutlineHeaderViewerView : public QWebEngineView
#else
class OutlineHeaderViewerView : public QWebView
#endif
{
    Q_OBJECT

public:
    explicit OutlineHeaderViewerView(QWidget* parent);
    OutlineHeaderViewerView(const OutlineHeaderViewerView&) = delete;
    OutlineHeaderViewerView(const OutlineHeaderViewerView&&) = delete;
    OutlineHeaderViewerView &operator=(const OutlineHeaderViewerView&) = delete;
    OutlineHeaderViewerView &operator=(const OutlineHeaderViewerView&&) = delete;
    virtual ~OutlineHeaderViewerView() override {}

#ifdef MF_QT_WEB_ENGINE
    QWebEnginePage* getPage() const { return page(); }

protected:
    bool event(QEvent* evt) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;
#else
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
#endif
    virtual void keyPressEvent(QKeyEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;

signals:
    void signalMouseDoubleClickEvent();
    void signalFromViewOutlineHeaderToOutlines();
};

class OutlineHeaderView : public QWidget
{
    Q_OBJECT

private:
    OutlineHeaderViewModel* outlineHeaderModel;

    OutlineHeaderViewerView* headerViewer;
    ViewToEditEditButtonsPanel* view2EditPanel;

public:
    OutlineHeaderView(QWidget* parent);
    OutlineHeaderView(const OutlineHeaderView&) = delete;
    OutlineHeaderView(const OutlineHeaderView&&) = delete;
    OutlineHeaderView&operator=(const OutlineHeaderView&) = delete;
    OutlineHeaderView&operator=(const OutlineHeaderView&&) = delete;
    ~OutlineHeaderView();

    OutlineHeaderViewerView* getViever() const { return headerViewer; }
    ViewToEditEditButtonsPanel* getEditPanel() const { return view2EditPanel; }

    void setModel(OutlineHeaderViewModel* outlineHeaderModel) {
        this->outlineHeaderModel = outlineHeaderModel;
    }
    void setZoomFactor(qreal factor) {
        headerViewer->setZoomFactor(factor);
    }
    void setHtml(const QString& html, const QUrl& baseUrl = QUrl()) {
        headerViewer->setHtml(html, baseUrl);
    }
    void giveViewerFocus() {
        QMetaObject::invokeMethod(
            headerViewer, "setFocus",
            Qt::QueuedConnection
            /* Q_ARG(char*, text) */);
    }

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void slotOpenEditor();

signals:
    void signalOpenEditor();
};

}
#endif // M8RUI_OUTLINE_HEADER_VIEW_H

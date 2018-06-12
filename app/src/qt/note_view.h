/*
 note_view.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTE_VIEW_H
#define M8RUI_NOTE_VIEW_H

#include "../../lib/src/debug.h"
#include "../../lib/src/model/note.h"

#include "note_view_model.h"

#include <QtWidgets>
#ifdef MF_QT_WEB_ENGINE
  #include <QWebEngineView>
#else
  #include <QWebView>
#endif
#include <QUrl>

namespace m8r {

#ifdef MF_QT_WEB_ENGINE

class WebEnginePageLinkNavigationPolicy : public QWebEnginePage
{
    Q_OBJECT

public:
    WebEnginePageLinkNavigationPolicy(QObject* parent = 0) : QWebEnginePage(parent) {}

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

#ifdef MF_QT_WEB_ENGINE
class NoteView: public QWebEngineView
#else
class NoteView: public QWebView
#endif
{
    Q_OBJECT

private:
    NoteViewModel* noteModel;

public:
    NoteView(QWidget* parent);
    NoteView(const NoteView&) = delete;
    NoteView(const NoteView&&) = delete;
    NoteView &operator=(const NoteView&) = delete;
    NoteView &operator=(const NoteView&&) = delete;

    void setModel(NoteViewModel* noteModel) { this->noteModel = noteModel; }

#ifdef MF_QT_WEB_ENGINE
    QWebEnginePage* getPage() const { return page(); }

// this is ugly and stupid workaround for handling double-click events in QWebEngineView
private:
    QObject *childObj = NULL;
protected:
    bool event(QEvent* evt) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;
#else
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent*) override;
#endif

signals:
    void signalMouseDoubleClickEvent();
    void signalFromViewNoteToOutlines();
};

}
#endif // M8RUI_NOTE_VIEW_H

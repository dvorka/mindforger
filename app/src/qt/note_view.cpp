/*
 note_view.cpp     MindForger thinking notebook

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
#include "note_view.h"

namespace m8r {

NoteView::NoteView(QWidget *parent)
    : QWebView(parent)
{

    // ensure that link clicks are not handled, but delegated to MF using linkClicked signal
    page()->setLinkDelegationPolicy(QWebPage::LinkDelegationPolicy::DelegateAllLinks);
}

void NoteView::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    // double click to Note view opens Note editor
    emit signalMouseDoubleClickEvent();
}

void NoteView::keyPressEvent(QKeyEvent* event)
{
    if(event->modifiers() & Qt::AltModifier){
        if(event->key()==Qt::Key_Left) {
            signalFromViewNoteToOutlines();
        }
    }

    QWebView::keyPressEvent(event);
}

} // m8r namespace

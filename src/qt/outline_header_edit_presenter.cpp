/*
 outline_header_edit_presenter.cpp     MindForger thinking notebook

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
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "outline_header_edit_presenter.h"

using namespace std;

namespace m8r {

OutlineHeaderEditPresenter::OutlineHeaderEditPresenter(
        OutlineHeaderEditView *view,
        MainWindowPresenter* mainPresenter,
        QObject *parent) : QObject(parent)
{
    this->view = view;
    this->mainPresenter = mainPresenter;

    Ontology& ontology=mainPresenter->getMind()->ontology();
    this->mdRepresentation
        = new MarkdownOutlineRepresentation{ontology};

    QObject::connect(
        view, SIGNAL(signalSaveOutlineHeader()),
        this, SLOT(slotSave()));
    QObject::connect(
        view, SIGNAL(signalSaveAndCloseEditor()),
        this, SLOT(slotSaveAndCloseEditor()));
}

void OutlineHeaderEditPresenter::setCurrentOutline(Outline* outline, string* html)
{
    this->currentOutline = outline;
    view->setPlainText(QString::fromStdString(*html));
}

void OutlineHeaderEditPresenter::slotSaveAndCloseEditor(void)
{
    slotSave();
    if(!view->toPlainText().isEmpty()) {
        mainPresenter->getOrloj()->fromOutlineHeaderEditBackToView(currentOutline);
    }
}

void OutlineHeaderEditPresenter::slotSave(void)
{
    QString text = view->toPlainText();
    if(!text.isEmpty()) {
        // IMPROVE try to find a more efficient conversion
        string s = text.toStdString();
        Note* note = mdRepresentation->note(&s);

        // IMPROVE move this code to note - it updates existing note OR mind.learn()
        currentOutline->setTitle(note->getTitle());
        currentOutline->setDescription(note->getDescription());
        //currentNote->setDeadline();
        //currentNote->setProgress();
        //currentNote->setTags();
        //currentNote->setType();
        // set on remembering
        currentOutline->setModified();
        currentOutline->setModifiedPretty();
        currentOutline->setRevision(currentOutline->getRevision()+1);
        // TODO delete note (vectors to be kept)

        mainPresenter->getMind()->remind().remember(currentOutline->getKey());
        mainPresenter->getStatusBar()->showInfo("Outline saved!");
    } else {
        mainPresenter->getStatusBar()->showError("Outline header text is empty - it was NOT saved!");
    }
}

OutlineHeaderEditPresenter::~OutlineHeaderEditPresenter()
{
    if(mdRepresentation) delete mdRepresentation;
}

}

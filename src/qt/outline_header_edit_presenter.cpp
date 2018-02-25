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
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "outline_header_edit_presenter.h"

using namespace std;

namespace m8r {

OutlineHeaderEditPresenter::OutlineHeaderEditPresenter(
        OutlineHeaderEditView *view,
        MainWindowPresenter* mwp,
        QObject *parent) : QObject(parent)
{
    this->view = view;
    this->mainPresenter = mwp;

    outlineHeaderEditDialog
        = new OutlineHeaderEditDialog{mwp->getMind()->remind().getOntology(), view};
    this->view->setOutlineHeaderEditDialog(outlineHeaderEditDialog);

    QObject::connect(
        view, SIGNAL(signalSaveOutlineHeader()),
        this, SLOT(slotSaveOutlineHeader()));
    QObject::connect(
        view, SIGNAL(signalCloseEditor()),
        this, SLOT(slotCloseEditor()));
    QObject::connect(
        view, SIGNAL(signalSaveAndCloseEditor()),
        this, SLOT(slotSaveAndCloseEditor()));
}

OutlineHeaderEditPresenter::~OutlineHeaderEditPresenter()
{
}

void OutlineHeaderEditPresenter::setOutline(Outline* outline)
{
    this->currentOutline = outline;
    string mdDescription{};
    outlineHeader = outline->getOutlineDescriptorAsNote();
    mainPresenter->getMarkdownRepresentation()->toDescription(outlineHeader, &mdDescription);

    view->setOutline(outline, mdDescription);
}

void OutlineHeaderEditPresenter::slotCloseEditor()
{
    mainPresenter->getOrloj()->fromOutlineHeaderEditBackToView(currentOutline);
}

void OutlineHeaderEditPresenter::slotSaveAndCloseEditor()
{
    slotSaveOutlineHeader();

    if(!view->isDescriptionEmpty()) {
        mainPresenter->getOrloj()->fromOutlineHeaderEditBackToView(currentOutline);
    }
}

void OutlineHeaderEditPresenter::slotSaveOutlineHeader()
{
    // set UI data to current note
    if(currentOutline) {
        string name{"Outline"};
        if(!view->getName().isEmpty()) {
            name.assign(view->getName().toStdString());
        }
        currentOutline->setName(name);

        if(!view->isDescriptionEmpty()) {
            string s{view->getDescription().toStdString()};
            vector<string*> d{};
            mainPresenter->getMarkdownRepresentation()->description(&s, d);
            currentOutline->setDescription(d);
        } else {
            currentOutline->clearDescription();
        }

        // Outline metada (type, tags, progress, deadline) are set by Outline header edit dialog on it's close
        // (if user doesn't open dialog, nothing is blindly saved there & here)

        // IMPROVE if fields below are set on remembering (save) of Outline, then delete code below
        currentOutline->makeModified();
        if(currentOutline->getReads()<currentOutline->getRevision()) {
            currentOutline->setReads(currentOutline->getRevision());
        }

        // remember
        mainPresenter->getMind()->remind().remember(currentOutline->getKey());
        mainPresenter->getStatusBar()->showInfo(tr("Outline saved!"));
    } else {
        mainPresenter->getStatusBar()->showError(tr("Attempt to save data from UI to Outline, but no Outline is set."));
    }
}

}

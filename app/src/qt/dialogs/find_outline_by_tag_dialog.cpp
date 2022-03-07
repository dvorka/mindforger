/*
 find_outline_by_tag.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "find_outline_by_tag_dialog.h"

namespace m8r {

using namespace std;

FindOutlineByTagDialog::FindOutlineByTagDialog(Ontology& ontology, QWidget *parent)
    : QDialog{parent}, ontology(ontology)
{
    mode = ThingsMode::OUTLINES;

    // widgets
    editTagsGroup = new EditTagsPanel{MfWidgetMode::FIND_MODE, ontology, this};
    editTagsGroup->refreshOntologyTags();
    editTagsGroup->setTitle(tr("Notebook tags:"));

    outlinesGroup = new QGroupBox{tr("Notebooks:"),this};
    QVBoxLayout* outlinesGroupLayout = new QVBoxLayout{this};
    outlinesGroup->setLayout(outlinesGroupLayout);
    listView = new QListView(this);
    // list view model must be set - use of this type of mode enable the use of string lists controlling its content
    listView->setModel(&listViewModel);
    // disable editation of the list item on doble click
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    outlinesGroupLayout->addWidget(listView);

    findButton = new QPushButton{tr("&Open Notebook")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    switchOutlineNoteDialogsButton = new QPushButton{tr("&Find Note")};

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(switchOutlineNoteDialogsButton, SIGNAL(clicked()), this, SLOT(switchDialogsSlot()));
    QObject::connect(findButton, SIGNAL(clicked()), this, SLOT(handleChoice()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(editTagsGroup);
    mainLayout->addWidget(outlinesGroup);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(switchOutlineNoteDialogsButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals
    QObject::connect(editTagsGroup, SIGNAL(signalTagSelectionChanged()), this, SLOT(handleTagsChanged()));

    // dialog    
    setWindowTitle(tr("Find Notebook by Tags"));
    // height is set to make sure listview gets enough lines
    resize(fontMetrics().averageCharWidth()*55, fontMetrics().height()*30);
    setModal(true);
}

FindOutlineByTagDialog::~FindOutlineByTagDialog()
{
    delete listView;
    delete findButton;
    delete closeButton;
}

void FindOutlineByTagDialog::show(vector<Thing*>& outlines, vector<const Tag*>* tags, vector<string>* customizedNames)
{
    choice = nullptr;
    // tags are changed > need to be refreshed
    // IMPROVE dirty flag to avoid refresh that is not needed
    editTagsGroup->refreshOntologyTags();

    things.clear();
    listViewStrings.clear();
    bool useCustomNames = customizedNames!=nullptr && customizedNames->size()>0;
    if(outlines.size()) {
        for(size_t i=0; i<outlines.size(); i++) {
            things.push_back(outlines[i]);
            if(useCustomNames) {
                listViewStrings << QString::fromStdString(customizedNames->at(i));
            } else {
                if(outlines.at(i)->getName().size()) {
                    listViewStrings << QString::fromStdString(outlines[i]->getName());
                } else {
                    listViewStrings << "";
                }
            }
        }
        (static_cast<QStringListModel*>(listView->model()))->setStringList(listViewStrings);
    }

    findButton->setEnabled(things.size());
    editTagsGroup->clearTagList();
    if(tags) {
        for(const Tag* tag:*tags) {
            editTagsGroup->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            editTagsGroup->slotAddTag();
        }
    }
    editTagsGroup->getLineEdit()->clear();
    editTagsGroup->getLineEdit()->setFocus();
    QDialog::show();
}

void FindOutlineByTagDialog::getChosenTags(std::vector<const Tag*>* tags)
{
    const vector<const Tag*>& ts = editTagsGroup->getTags();
    *tags = ts;
}

void FindOutlineByTagDialog::handleTagsChanged()
{
    auto choosenTags = editTagsGroup->getTags();

    int row = 0;
    if(choosenTags.size()) {
        int visible = 0;
        for(Thing* e:things) {
            const std::vector<const Tag*>* thingTags;
            if(mode==ThingsMode::OUTLINES) {
                Outline* o = static_cast<Outline*>(e);
                thingTags = o->getTags();
            } else {
                Note* n = static_cast<Note*>(e);
                thingTags = n->getTags();
            }

            bool hasAllTags=true;
            for(size_t i=0; i<editTagsGroup->getTags().size(); i++) {
                if(std::find(
                    thingTags->begin(),
                    thingTags->end(),
                    editTagsGroup->getTags().at(i)) == thingTags->end())
                {
                    hasAllTags=false;
                    break;
                }
            }

            if(hasAllTags) {
                listView->setRowHidden(row, false);
                visible++;
            } else {
                listView->setRowHidden(row, true);
            }
            row++;
        }
        findButton->setEnabled(visible);
    } else {
        // show everything
        for(size_t i=0; i<things.size(); i++) {
            listView->setRowHidden(row++, true);
        }
    }
}

void FindOutlineByTagDialog::handleReturn()
{
    if(findButton->isEnabled()) {
        for(size_t row=0; row<things.size(); row++) {
            if(!listView->isRowHidden(row)) {
                choice = things[row];
                break;
            }
        }

        QDialog::close();
        emit searchFinished();
    }
}

void FindOutlineByTagDialog::handleChoice()
{
    if(listView->currentIndex().isValid()) {
        choice = things[listView->currentIndex().row()];

        QDialog::close();
        emit searchFinished();
    }
}

} // m8r namespace

/*
 organizer_cfg_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "organizer_cfg_dialog.h"

namespace m8r {

using namespace std;

OrganizerConfigDialog::OrganizerConfigDialog(Ontology& ontology, QWidget* parent)
    : QDialog{parent}, ontology(ontology)
{
    mode = ThingsMode::OUTLINES;

    // widgets
    editTagsGroup = new EditTagsPanel{ontology, this};
    editTagsGroup->refreshOntologyTags();
    editTagsGroup->setTitle(tr("Filtering options:"));

    findButton = new QPushButton{tr("&Open Notebook")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    // TODO QObject::connect(findButton, SIGNAL(clicked()), this, SLOT(handleChoice()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(editTagsGroup);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals
    QObject::connect(editTagsGroup, SIGNAL(signalTagSelectionChanged()), this, SLOT(handleTagsChanged()));

    // dialog    
    setWindowTitle(tr("Configure Quadrant Filtering"));
    // height is set to make sure listview gets enough lines
    resize(fontMetrics().averageCharWidth()*55, fontMetrics().height()*30);
    setModal(true);
}

OrganizerConfigDialog::~OrganizerConfigDialog()
{
    delete findButton;
    delete closeButton;
}

void OrganizerConfigDialog::show(vector<const Tag*>* tags)
{
    choice = nullptr;
    // tags are changed > need to be refreshed
    // IMPROVE dirty flag to avoid refresh that is not needed
    editTagsGroup->refreshOntologyTags();

    things.clear();

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

void OrganizerConfigDialog::getChosenTags(std::vector<const Tag*>* tags)
{
    const vector<const Tag*>& ts = editTagsGroup->getTags();
    *tags = ts;
}

} // m8r namespace

/*
 new_organizer_dialog.cpp     MindForger thinking notebook

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

#include "organizer_new_dialog.h"

namespace m8r {

using namespace std;

OrganizerNewDialog::OrganizerNewDialog(Ontology& ontology, QWidget* parent)
    : QDialog{parent}, ontology(ontology)
{
    mode = ThingsMode::OUTLINES;

    // widgets
    upperRighTags = new EditTagsPanel{ontology, this};
    upperRighTags->refreshOntologyTags();
    upperRighTags->setTitle(tr("Upper right quadrant tags:"));

    lowerRighTags = new EditTagsPanel{ontology, this};
    lowerRighTags->refreshOntologyTags();
    lowerRighTags->setTitle(tr("Lower right quadrant tags:"));

    lowerLeftTags = new EditTagsPanel{ontology, this};
    lowerLeftTags->refreshOntologyTags();
    lowerLeftTags->setTitle(tr("Lower left quadrant tags:"));

    upperLeftTags = new EditTagsPanel{ontology, this};
    upperLeftTags->refreshOntologyTags();
    upperLeftTags->setTitle(tr("Upper left quadrant tags:"));

    findButton = new QPushButton{tr("&Create")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    // TODO QObject::connect(findButton, SIGNAL(clicked()), this, SLOT(handleChoice()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(upperRighTags);
    mainLayout->addWidget(lowerRighTags);
    mainLayout->addWidget(lowerLeftTags);
    mainLayout->addWidget(upperLeftTags);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals

    // TODO redefine signals
    // TODO QObject::connect(upperRighTags, SIGNAL(signalTagSelectionChanged()), this, SLOT(handleTagsChanged()));

    // dialog    
    setWindowTitle(tr("New Organizer"));
    // make dialog big enough (FTS dialog based size)
    resize(fontMetrics().averageCharWidth()*90, fontMetrics().height()*35);
    // TODO height is set to make sure listview gets enough lines
    // TODO remove resize(fontMetrics().averageCharWidth()*55, fontMetrics().height()*30);
    setModal(true);
}

OrganizerNewDialog::~OrganizerNewDialog()
{
    delete findButton;
    delete closeButton;
}

void OrganizerNewDialog::show(vector<const Tag*>* tags)
{
    choice = nullptr;
    // tags are changed > need to be refreshed
    // IMPROVE dirty flag to avoid refresh that is not needed
    upperRighTags->refreshOntologyTags();
    lowerRighTags->refreshOntologyTags();
    lowerLeftTags->refreshOntologyTags();
    upperLeftTags->refreshOntologyTags();

    things.clear();

    findButton->setEnabled(things.size());
    upperRighTags->clearTagList();
    lowerRighTags->clearTagList();
    lowerLeftTags->clearTagList();
    upperLeftTags->clearTagList();
    if(tags) {
        for(const Tag* tag:*tags) {
            upperRighTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            upperRighTags->slotAddTag();

            lowerRighTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            lowerRighTags->slotAddTag();

            lowerLeftTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            lowerLeftTags->slotAddTag();

            upperLeftTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            upperLeftTags->slotAddTag();
        }
    }
    upperRighTags->getLineEdit()->clear();
    lowerRighTags->getLineEdit()->clear();
    lowerLeftTags->getLineEdit()->clear();
    upperLeftTags->getLineEdit()->clear();

    upperRighTags->getLineEdit()->setFocus();

    QDialog::show();
}

void OrganizerNewDialog::getChosenTags(std::vector<const Tag*>* tags)
{
    // TODO return all quadrant tags
    //const vector<const Tag*>& ts = editTagsGroup->getTags();
    //*tags = ts;
}

} // m8r namespace

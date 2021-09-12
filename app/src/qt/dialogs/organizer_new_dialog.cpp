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
    nameLabel = new QLabel(tr("Name")+":", this);
    nameEdit = new QLineEdit(tr("Organizer"), this);

    upperLeftTags = new EditTagsPanel{ontology, this};
    upperLeftTags->refreshOntologyTags();
    upperLeftTags->setTitle(tr("Upper left quadrant tags:"));

    upperRightTags = new EditTagsPanel{ontology, this};
    upperRightTags->refreshOntologyTags();
    upperRightTags->setTitle(tr("Upper right quadrant tags:"));

    lowerLeftTags = new EditTagsPanel{ontology, this};
    lowerLeftTags->refreshOntologyTags();
    lowerLeftTags->setTitle(tr("Lower left quadrant tags:"));

    lowerRightTags = new EditTagsPanel{ontology, this};
    lowerRightTags->refreshOntologyTags();
    lowerRightTags->setTitle(tr("Lower right quadrant tags:"));

    createButton = new QPushButton{tr("&Create")};
    createButton->setDefault(true);
    createButton->setEnabled(true);

    sortByLabel = new QLabel(tr("Sort Notebooks by")+":", this);
    sortByCombo = new QComboBox{this};
    sortByCombo->addItem(tr("importance"));
    sortByCombo->addItem(tr("urgency"));

    filterByLabel = new QLabel(tr("Filter by")+":", this);
    filterByCombo = new QComboBox{this};
    filterByCombo->addItem(tr("notes"), Organizer::CONFIG_VALUE_FILTER_BY_O);
    filterByCombo->addItem(tr("notebooks"), Organizer::CONFIG_VALUE_FILTER_BY_O);
    filterByCombo->addItem(tr("notebooks and notes"), Organizer::CONFIG_VALUE_FILTER_BY_O_N);

    // TODO notebook scope: choose notebook + show notebook ID in non-editable field

    closeButton = new QPushButton{tr("&Cancel")};

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{this};

    QHBoxLayout* h = new QHBoxLayout{this};
    QVBoxLayout* l = new QVBoxLayout{this};
    l->addWidget(upperRightTags);
    l->addWidget(lowerRightTags);
    QVBoxLayout* r = new QVBoxLayout{this};
    r->addWidget(upperLeftTags);
    r->addWidget(lowerLeftTags);
    h->addLayout(r);
    h->addLayout(l);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(createButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addLayout(h);
    mainLayout->addWidget(sortByLabel);
    mainLayout->addWidget(sortByCombo);
    mainLayout->addWidget(filterByLabel);
    mainLayout->addWidget(filterByCombo);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(createButton, SIGNAL(clicked()), this, SLOT(handleCreate()));
    // TODO QObject::connect(upperRighTags, SIGNAL(signalTagSelectionChanged()), this, SLOT(handleTagsChanged()));
    // TODO on n/on/o selection disable drop down w/ sorting by importance

    // dialog    
    setWindowTitle(tr("New Organizer"));
    // make dialog big enough (FTS dialog based size)
    resize(fontMetrics().averageCharWidth()*90, fontMetrics().height()*35);
    setModal(true);
}

OrganizerNewDialog::~OrganizerNewDialog()
{
    delete createButton;
    delete closeButton;
}

void OrganizerNewDialog::show(vector<const Tag*>* tags)
{
    nameEdit->clear();

    // tags are changed > need to be refreshed
    // IMPROVE dirty flag to avoid refresh that is not needed
    upperRightTags->refreshOntologyTags();
    lowerRightTags->refreshOntologyTags();
    lowerLeftTags->refreshOntologyTags();
    upperLeftTags->refreshOntologyTags();

    things.clear();

    // disabling button w/o explaining why would not be UX bug
    createButton->setEnabled(true);

    upperRightTags->clearTagList();
    lowerRightTags->clearTagList();
    lowerLeftTags->clearTagList();
    upperLeftTags->clearTagList();
    if(tags) {
        for(const Tag* tag:*tags) {
            upperRightTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            upperRightTags->slotAddTag();

            lowerRightTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            lowerRightTags->slotAddTag();

            lowerLeftTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            lowerLeftTags->slotAddTag();

            upperLeftTags->getLineEdit()->setText(QString::fromStdString(tag->getName()));
            upperLeftTags->slotAddTag();
        }
    }
    upperRightTags->getLineEdit()->clear();
    lowerRightTags->getLineEdit()->clear();
    lowerLeftTags->getLineEdit()->clear();
    upperLeftTags->getLineEdit()->clear();

    nameEdit->setFocus();
    nameEdit->setText(tr("Organizer"));
    nameEdit->selectAll();

    QDialog::show();
}

void OrganizerNewDialog::handleCreate()
{
    // VALIDATION of dialog data
    if(!nameEdit->text().size()) {
        QMessageBox::critical(
            this,
            tr("New Organizer Error"),
            tr("Organizer must have non-empty name.")
        );
    } else if(
        !upperRightTags->getTagCount()
        || !upperLeftTags->getTagCount()
        || !lowerRightTags->getTagCount()
        || !lowerLeftTags->getTagCount()
    ) {
        QMessageBox::critical(
            this,
            tr("New Organizer Error"),
            tr("At least one tag must be selected for every quadrant.")
        );
    } else {
        emit createFinished();
    }
}

} // m8r namespace

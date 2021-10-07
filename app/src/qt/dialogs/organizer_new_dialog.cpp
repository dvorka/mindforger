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
    : QDialog{parent},
      ontology{ontology},
      oScopeOutline{nullptr},
      organizerToEdit{nullptr}
{
    mode = ThingsMode::OUTLINES;

    // widgets
    nameLabel = new QLabel(tr("Name")+":", this);
    nameEdit = new QLineEdit(tr("Organizer"), this);

    upperLeftTags = new EditTagsPanel{ontology, this};
    upperLeftTags->refreshOntologyTags();
    upperLeftTags->setTitle(tr("Upper left quadrant tags")+":");

    upperRightTags = new EditTagsPanel{ontology, this};
    upperRightTags->refreshOntologyTags();
    upperRightTags->setTitle(tr("Upper right quadrant tags")+":");

    lowerLeftTags = new EditTagsPanel{ontology, this};
    lowerLeftTags->refreshOntologyTags();
    lowerLeftTags->setTitle(tr("Lower left quadrant tags")+":");

    lowerRightTags = new EditTagsPanel{ontology, this};
    lowerRightTags->refreshOntologyTags();
    lowerRightTags->setTitle(tr("Lower right quadrant tags")+":");

    oScopeLabel = new QLabel(tr("Notebook scope")+":", this);
    oScopeEdit = new QLineEdit("", this);
    oScopeEdit->setDisabled(true);
    findOutlineButton = new QPushButton{tr("Notebook")};
    clearOutlineButton = new QPushButton{tr("Clear")};

    createButton = new QPushButton{tr("&Create")};
    createButton->setDefault(true);
    createButton->setEnabled(true);

    sortByLabel = new QLabel(tr("Sort Notebooks by")+":", this);
    sortByCombo = new QComboBox{this};
    sortByCombo->addItem(tr("importance"), Organizer::SortBy::IMPORTANCE);
    sortByCombo->addItem(tr("urgency"), Organizer::SortBy::URGENCY);

    filterByLabel = new QLabel(tr("Filter by")+":", this);
    filterByCombo = new QComboBox{this};
    filterByCombo->addItem(tr("notebooks"), Organizer::FilterBy::OUTLINES);
    filterByCombo->addItem(tr("notes"), Organizer::FilterBy::NOTES);
    filterByCombo->addItem(tr("notebooks and notes"), Organizer::FilterBy::OUTLINES_NOTES);

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

    QHBoxLayout* buttonLayout = new QHBoxLayout{this};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(createButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addLayout(h);
    mainLayout->addWidget(oScopeLabel);

    QHBoxLayout* oScopeLayout = new QHBoxLayout{this};
    oScopeLayout->addWidget(oScopeEdit);
    oScopeLayout->addWidget(findOutlineButton);
    oScopeLayout->addWidget(clearOutlineButton);
    mainLayout->addLayout(oScopeLayout);

    mainLayout->addWidget(sortByLabel);
    mainLayout->addWidget(sortByCombo);
    mainLayout->addWidget(filterByLabel);
    mainLayout->addWidget(filterByCombo);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialogs
    findOutlineByNameDialog = new FindOutlineByNameDialog{this};
    findOutlineByNameDialog->setWindowTitle(tr("Find Notebook as Scope"));

    // signals
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(createButton, SIGNAL(clicked()), this, SLOT(handleCreate()));
    QObject::connect(findOutlineButton, SIGNAL(clicked()), this, SLOT(handleFindOutline()));
    QObject::connect(clearOutlineButton, SIGNAL(clicked()), this, SLOT(handleClearOutline()));
    QObject::connect(findOutlineByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineChoice()));

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

void OrganizerNewDialog::show(
    const std::vector<Outline*>& outlines,
    const std::vector<const Tag*>* tags,
    Organizer* organizerToEdit,
    Outline* oScopeOutline
) {
    this->outlines = outlines;
    this->organizerToEdit = organizerToEdit;

    if(!organizerToEdit) {
        createButton->setText(tr("Create"));
    } else {
        createButton->setText(tr("Update"));
    }

    nameEdit->clear();

    // tags are changed > need to be refreshed
    // IMPROVE dirty flag to avoid refresh that is not needed
    upperRightTags->refreshOntologyTags();
    lowerRightTags->refreshOntologyTags();
    lowerLeftTags->refreshOntologyTags();
    upperLeftTags->refreshOntologyTags();

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
    if(organizerToEdit) {
        setWindowTitle(tr("Edit Organizer"));

        nameEdit->setText(QString::fromStdString(organizerToEdit->getName()));

        upperRightTags->setTagsAsStrings(organizerToEdit->getUpperRightTags());
        upperLeftTags->setTagsAsStrings(organizerToEdit->getUpperLeftTags());
        lowerRightTags->setTagsAsStrings(organizerToEdit->getLowerRightTags());
        lowerLeftTags->setTagsAsStrings(organizerToEdit->getLowerLeftTags());

        if(organizerToEdit->getOutlineScope().size()) {
            if(oScopeOutline) {
                this->oScopeOutline = oScopeOutline;
                oScopeEdit->setText(QString::fromStdString(oScopeOutline->getName()));
            } else {
                organizerToEdit->clearOutlineScope();
            }
        }

        sortByCombo->setCurrentText(QString::fromStdString(organizerToEdit->getSortByAsStr()));

        if(oScopeOutline) {
            sortByCombo->setEnabled(false);
            filterByCombo->setCurrentText(Organizer::CONFIG_VALUE_FILTER_BY_N);
            filterByCombo->setEnabled(false);
        } else {
            sortByCombo->setEnabled(true);
            filterByCombo->setEnabled(true);
            filterByCombo->setCurrentText(QString::fromStdString(organizerToEdit->getFilterByAsStr()));
        }
    } else {
        setWindowTitle(tr("New Organizer"));

        nameEdit->setText(tr("Organizer"));
        nameEdit->selectAll();

        oScopeEdit->clear();
        oScopeOutline = nullptr;

        filterByCombo->setEnabled(true);
        sortByCombo->setEnabled(true);
    }

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

/*
 * O lookup
 */

void OrganizerNewDialog::handleFindOutline()
{
    vector<Thing*> ts{};
    for(Outline* o:this->outlines) {
        ts.push_back(o);
    }
    findOutlineByNameDialog->show(ts);
    findOutlineByNameDialog->exec();
}

void OrganizerNewDialog::handleFindOutlineChoice()
{
    if(findOutlineByNameDialog->getChoice()) {
        this->oScopeOutline = static_cast<Outline*>(findOutlineByNameDialog->getChoice());
        oScopeEdit->setText(QString::fromStdString(this->oScopeOutline->getName()));

        // reconfigure dialog controls
        sortByCombo->setEnabled(false);
        filterByCombo->setCurrentText(Organizer::CONFIG_VALUE_FILTER_BY_N);
        filterByCombo->setEnabled(false);
    }
}

void OrganizerNewDialog::handleClearOutline()
{
    this->oScopeOutline = nullptr;
    oScopeEdit->clear();

    // reconfigure dialog controls
    sortByCombo->setEnabled(true);
    filterByCombo->setEnabled(true);
}

} // m8r namespace

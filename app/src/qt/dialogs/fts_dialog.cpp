/*
 fts_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "fts_dialog.h"

namespace m8r {

FtsDialog::FtsDialog(QWidget *parent)
    : QDialog(parent)
{
    // facet
    scopeType = ResourceType::REPOSITORY;

    // widgets
    label = new QLabel{tr("Text to &find:")};
    lineEdit = new QLineEdit{this};
    label->setBuddy(lineEdit);

    completer = new QCompleter{completerStrings, this};
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);

    editorSearchModeChecks = new QGroupBox{tr("Match:"), this};
    editorIgnoreCheck = new QCheckBox{tr("&Ignore case"), editorSearchModeChecks};
    editorWordCheck = new QCheckBox{tr("&Whole words"), editorSearchModeChecks};
    editorReverseCheck = new QCheckBox{tr("&Reverse"), editorSearchModeChecks};
    editorIgnoreCheck->setChecked(true);
    QVBoxLayout* vbox = new QVBoxLayout{this};
    vbox->addWidget(editorIgnoreCheck);
    vbox->addWidget(editorWordCheck);
    vbox->addWidget(editorReverseCheck);
    vbox->addStretch(1);
    editorSearchModeChecks->setLayout(vbox);

    searchModeRadios = new QGroupBox{tr("Match:"), this};
    exactRadio = new QRadioButton{tr("&Exact"), searchModeRadios};
    ignoreRadio = new QRadioButton{tr("&Ignore case"), searchModeRadios};
    regexRadio = new QRadioButton{tr("&Regular expression"), searchModeRadios};
    ignoreRadio->setChecked(true);
    vbox = new QVBoxLayout{this};
    vbox->addWidget(exactRadio);
    vbox->addWidget(ignoreRadio);
    vbox->addWidget(regexRadio);
    vbox->addStretch(1);
    searchModeRadios->setLayout(vbox);

    findButton = new QPushButton{tr("&Search")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString&)));
    connect(findButton, SIGNAL(clicked()), this, SLOT(addExpressionToHistory()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout *mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    mainLayout->addWidget(editorSearchModeChecks);
    mainLayout->addWidget(searchModeRadios);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Full-text Search"));
    resize(fontMetrics().averageCharWidth()*35, height());
    updateFacet();
    setModal(true);
}

void FtsDialog::updateFacet()
{
    if(scopeType == ResourceType::NOTE) {
        editorSearchModeChecks->setVisible(true);
        searchModeRadios->setVisible(false);
    } else {
        editorSearchModeChecks->setVisible(false);
        searchModeRadios->setVisible(true);
    }
}

/**
 * @brief Set the scope for search.
 *
 * If scope type is repository, then whole MF repository is FTS,
 * else if scope type is O, then active O is FTS, else if
 * scope is NOTE, active N/O header editor is searched.
 */
void FtsDialog::setScope(ResourceType t, Outline* s)
{
    scopeType = t;
    scope = s;

    updateFacet();
}

void FtsDialog::enableFindButton(const QString& text)
{
    findButton->setEnabled(!text.isEmpty());
}

void FtsDialog::addExpressionToHistory()
{
    if(lineEdit->text().size()) {
        completerStrings.insert(0, lineEdit->text());
        ((QStringListModel*)completer->model())->setStringList(completerStrings);
    }
}

FtsDialog::~FtsDialog()
{
    delete label;
    delete lineEdit;
    delete completer;
    delete searchModeRadios;
    delete findButton;
    delete closeButton;
}

} // m8r namespace

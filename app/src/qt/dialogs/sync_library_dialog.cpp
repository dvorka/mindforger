/*
 sync_library_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "sync_library_dialog.h"

using namespace std;

namespace m8r {

SyncLibraryDialog::SyncLibraryDialog(QWidget* parent)
    : QDialog(parent)
{
    // widgets
    findLibrarySourceLabel = new QLabel{
        tr(
            "Choose a library directory to be synchronized - new notebook\n"
            "will be created for every new document found in the library.\n"
            "Existing MindForger notebooks for library documents which were\n"
            "deleted, renamed or moved (i.e. their link to library document\n"
            "is broken) will not be deprecated to protect your document-related\n"
            "notes. Feel free to deprecate such notebook(s) yourself.\n"
        ),
        parent};

    librariesLabel = new QLabel{tr("Choose library to update: ")};
    libraryPathsCombo = new QComboBox{this};

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    syncButton = new QPushButton{tr("&Update")};
    syncButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(findLibrarySourceLabel);
    mainLayout->addWidget(librariesLabel);
    mainLayout->addWidget(libraryPathsCombo);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(syncButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Update Document Library"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

SyncLibraryDialog::~SyncLibraryDialog()
{
    delete findLibrarySourceLabel;
    delete librariesLabel;
    delete libraryPathsCombo;
    delete syncButton;
    delete closeButton;
}

void SyncLibraryDialog::show()
{
    QDialog::show();
}

} // m8r namespace

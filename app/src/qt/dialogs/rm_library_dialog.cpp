/*
 sync_library_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "rm_library_dialog.h"

using namespace std;

namespace m8r {

RemoveLibraryDialog::RemoveLibraryDialog(QWidget* parent)
    : QDialog(parent),
      libsToRemove()
{
    // widgets
    findLibrarySourceLabel = new QLabel{
        tr(
            "Choose a library to be removed - this action will delete\n"
            "Notebooks which represent documents from the library\n"
            "directory.\n"
            "Referenced documents will NOT be deleted.\n"
        ),
        parent};

    librariesLabel = new QLabel{tr("Choose library to delete: ")};
    libraryPathsCombo = new QComboBox{this};

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    rmButton = new QPushButton{tr("&Delete")};
    rmButton->setDefault(true);
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
    buttonLayout->addWidget(rmButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Delete Document Library"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

void RemoveLibraryDialog::addLibraryToRemove(FilesystemInformationSource* libSrc)
{
    libsToRemove.push_back(libSrc);
    libraryPathsCombo->addItem(QString::fromStdString(libSrc->getPath()));
}

RemoveLibraryDialog::~RemoveLibraryDialog()
{
    delete findLibrarySourceLabel;
    delete librariesLabel;
    delete libraryPathsCombo;
    delete rmButton;
    delete closeButton;
}

void RemoveLibraryDialog::reset() {
    libraryPathsCombo->clear();
    for(FilesystemInformationSource* libToRemove:libsToRemove) {
        delete libToRemove;
    }
    libsToRemove.clear();
}

string RemoveLibraryDialog::getLibraryMfPathForLocator(string& locator) {
    for(FilesystemInformationSource* libToRemove:libsToRemove) {
        if(locator == libToRemove->getPath()) {
            return libToRemove->getMfPath();
        }
    }

    return string{};
}

void RemoveLibraryDialog::show()
{
    QDialog::show();
}

} // m8r namespace

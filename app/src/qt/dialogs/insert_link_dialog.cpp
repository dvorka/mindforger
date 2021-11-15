/*
 insert_link_dialog.cpp     MindForger thinking notebook

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
#include "insert_link_dialog.h"

namespace m8r {

using namespace std;

InsertLinkDialog::InsertLinkDialog(QWidget* parent)
    : QDialog(parent)
{
    // widgets
    linkTextLabel = new QLabel{tr("Link text:")};
    linkTextEdit = new QLineEdit{};
    pathLabel = new QLabel{tr("Notebook, Note, file path or web address:")};
    pathEdit = new QLineEdit{};

    findOutlineButton = new QPushButton{tr("Notebook")};
    findNoteButton = new QPushButton{tr("Note")};
    findFileButton = new QPushButton{tr("File")};
    findDirectoryButton = new QPushButton{tr("Directory")};

    copyToRepoCheckBox = new QCheckBox{tr("copy link target to repository")};
    copyToRepoCheckBox->setChecked(true);
    copyToRepoCheckBox->setEnabled(true);

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    insertButton = new QPushButton{tr("&Insert")};
    insertButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // dialogs
    findOutlineByNameDialog = new FindOutlineByNameDialog{this};
    findOutlineByNameDialog->setWindowTitle(tr("Find Notebook as Link Target"));
    findNoteByNameDialog = new FindNoteByNameDialog{this};
    findNoteByNameDialog->setWindowTitle(tr("Find Note as Link Target"));

    // signals
    QObject::connect(findOutlineButton, SIGNAL(clicked()), this, SLOT(handleFindOutline()));
    QObject::connect(findNoteButton, SIGNAL(clicked()), this, SLOT(handleFindNote()));
    QObject::connect(findFileButton, SIGNAL(clicked()), this, SLOT(handleFindFile()));
    QObject::connect(findDirectoryButton, SIGNAL(clicked()), this, SLOT(handleFindDirectory()));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(findOutlineByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineChoice()));
    QObject::connect(findNoteByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteChoice()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    QHBoxLayout* srcButtonLayout = new QHBoxLayout{};
    srcButtonLayout->addWidget(findOutlineButton);
    srcButtonLayout->addWidget(findNoteButton);
    srcButtonLayout->addWidget(findFileButton);
    srcButtonLayout->addWidget(findDirectoryButton);
    srcButtonLayout->addStretch();
    mainLayout->addLayout(srcButtonLayout);
    mainLayout->addWidget(linkTextLabel);
    mainLayout->addWidget(linkTextEdit);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathEdit);
    mainLayout->addWidget(copyToRepoCheckBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(insertButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Insert Link"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

InsertLinkDialog::~InsertLinkDialog()
{
}

void InsertLinkDialog::show(
        const Repository* repository,
        Outline* outline,
        vector<Thing*>& os,
        vector<Note*>& ns,
        const QString& selectedText,
        const QString& link)
{
    this->activeRepository = repository;
    this->currentOutline = outline;

    linkTextEdit->setText(selectedText.size()?selectedText:tr("link"));
    linkTextEdit->selectAll();
    findOutlineButton->setFocus();
    pathEdit->clear();
    if(link.size()) {
        pathEdit->setText(link);
    }

    outlines = os;
    notes = ns;

    QDialog::show();
}

void InsertLinkDialog::handleFindOutline()
{
    findOutlineByNameDialog->show(outlines);
    findOutlineByNameDialog->exec();
}

void InsertLinkDialog::handleFindOutlineChoice()
{
    if(findOutlineByNameDialog->getChoice()) {
        Outline* choice = static_cast<Outline*>(
            findOutlineByNameDialog->getChoice()
        );
        linkTextEdit->setText(QString::fromStdString(choice->getName()));

        string p = RepositoryIndexer::makePathRelative(
            activeRepository, currentOutline->getKey(), choice->getKey()
        );
        pathToLinuxDelimiters(p, p);
        pathEdit->setText(QString::fromStdString(p));
    }
}

void InsertLinkDialog::handleFindNote()
{
    findNoteByNameDialog->clearScope();
    findNoteByNameDialog->show(notes);
    findNoteByNameDialog->exec();
}

void InsertLinkDialog::handleFindNoteChoice()
{
    if(findNoteByNameDialog->getChoice()) {
        Note* choice = static_cast<Note*>(findNoteByNameDialog->getChoice());
        linkTextEdit->setText(QString::fromStdString(choice->getName()));

        string dstPath{};
        dstPath += choice->getOutline()->getKey();
        dstPath += "#";
        dstPath += choice->getMangledName();
        string p = RepositoryIndexer::makePathRelative(
            activeRepository, currentOutline->getKey(), dstPath
        );
        pathEdit->setText(QString::fromStdString(p));
    }
}

void InsertLinkDialog::handleFindFile()
{
    QString homeDirectory = QStandardPaths::locate(
        QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory
    );

    QFileDialog fileDialog{this};
    fileDialog.setWindowTitle(tr("Choose File"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setDirectory(homeDirectory);
    fileDialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames{};
    if(fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
        if(fileNames.size()==1) {
            pathEdit->setText(fileNames[0]);
        } // else too many files
    } // else directory closed / nothing choosen
}

void InsertLinkDialog::handleFindDirectory()
{
    QString homeDirectory = QStandardPaths::locate(
        QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory
    );

    QFileDialog fileDialog{this};
    fileDialog.setWindowTitle(tr("Choose Directory"));
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setDirectory(homeDirectory);
    fileDialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames{};
    if(fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
        if(fileNames.size()==1) {
            pathEdit->setText(fileNames[0]);
        } // else too many files
    } // else directory closed / nothing choosen
}

} // m8r namespace

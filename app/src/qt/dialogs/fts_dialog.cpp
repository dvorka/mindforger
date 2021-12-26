/*
 fts_dialog.cpp     MindForger thinking notebook

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
#include "fts_dialog.h"

namespace m8r {

using namespace std;

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

    // buttons
    openButton = new QPushButton{tr("&Open")};
    openButton->setEnabled(false);
    searchButton = new QPushButton{tr("&Search")};
    searchButton->setDefault(true);
    searchButton->setEnabled(false);
    closeButton = new QPushButton{tr("&Cancel")};

    // assembly
    QVBoxLayout* searchLayout = new QVBoxLayout{};
    searchLayout->addWidget(label);
    searchLayout->addWidget(lineEdit);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(editorSearchModeChecks);
    searchLayout->addWidget(searchModeRadios);

    resultSplit = new QSplitter{Qt::Vertical, parent};
    // Os listing
    resultListingView = new NotesTableView(resultSplit);
    resultListingPresenter = new NotesTablePresenter(resultListingView);
    resultSplit->addWidget(resultListingView);
    // matches within O
    resultPreview = new QTextBrowser(resultSplit);
    resultPreview->setOpenExternalLinks(false);
    resultSplit->addWidget(resultPreview);
    resultSplit->setVisible(false);

    // signals (intra view only - other signals in presenter)
    QObject::connect(
        lineEdit, SIGNAL(textChanged(const QString &)),
        this, SLOT(enableSearchButton(const QString&)));
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(openButton);
    buttonLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(resultSplit);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Full-text Search"));
    setSizeSearchFacet();
    updateFacet();
    setModal(true);
}

FtsDialog::~FtsDialog()
{
    delete label;
    delete lineEdit;
    delete completer;
    delete searchModeRadios;
    delete searchButton;
    delete closeButton;

    delete resultPreview;
    delete resultListingView;
    delete resultSplit;
}

void FtsDialog::refreshResult(std::vector<Note*>* notes)
{
    resultListingPresenter->refresh(notes);
}

void FtsDialog::updateFacet()
{
    if(scopeType == ResourceType::NOTE) {
        editorSearchModeChecks->setVisible(true);
        searchModeRadios->setVisible(false);
        resultSplit->setVisible(false);
        openButton->setVisible(false);
        // ugly & stupid, but dialog size is set only when called twice
        setSizeSearchFacet();
        setSizeSearchFacet();
    } else {
        editorSearchModeChecks->setVisible(false);
        searchModeRadios->setVisible(true);
        openButton->setVisible(true);
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

void FtsDialog::setSizeSearchFacet()
{
    resize(fontMetrics().averageCharWidth()*90, fontMetrics().height()*12);
}

void FtsDialog::setSizeResultFacet()
{
    resize(fontMetrics().averageCharWidth()*90, fontMetrics().height()*35);
}

void FtsDialog::enableSearchButton(const QString& text)
{
    if(text.isEmpty()) {
        searchButton->setEnabled(false);
        resultSplit->setVisible(false);
        // ugly & stupid, but dialog size is set only when called twice
        setSizeSearchFacet();
        setSizeSearchFacet();
    } else {
        searchButton->setEnabled(true);
    }
}

void FtsDialog::searchAndAddPatternToHistory()
{
    if(scopeType == ResourceType::NOTE) {
        emit signalNoteScopeSearch();
    } else {
        if(lineEdit->text().size()) {
            completerStrings.insert(0, lineEdit->text());
            ((QStringListModel*)completer->model())->setStringList(completerStrings);

            if(getResultSize() > 0) {
                resultPreview->setHtml(QString{});
                resultSplit->setVisible(true);
                setSizeResultFacet();

                // select the first match
                resultListingView->setFocus();
                resultListingView->setCurrentIndex(
                    resultListingPresenter->getModel()->index(0, 0));
            }
        } else {
            resultSplit->setVisible(false);
            setSizeSearchFacet();
        }
    }
}

} // m8r namespace

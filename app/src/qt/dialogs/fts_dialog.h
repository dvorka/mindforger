/*
 fts_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8RUI_FTS_DIALOG_H
#define M8RUI_FTS_DIALOG_H

#include <QtWidgets>

#include "../notes_table_view.h"
#include "../notes_table_presenter.h"

#include "../../lib/src/model/resource_types.h"
#include "../../lib/src/model/outline.h"

namespace m8r {

class FtsDialog : public QDialog
{
    Q_OBJECT

private:
    QLineEdit* lineEdit;
    QCheckBox* caseCheckBox;
    QGroupBox* editorSearchModeChecks;
    QGroupBox* searchModeRadios;
    QCheckBox* editorIgnoreCheck;
    QCheckBox* editorWordCheck;
    QCheckBox* editorReverseCheck;
    QRadioButton* exactRadio;
    QRadioButton* ignoreRadio;
    QRadioButton* regexRadio;

    QSplitter* resultSplit;
    NotesTableView* resultListingView;
    // IMPROVE move to FTS dialog presenter
    NotesTablePresenter* resultListingPresenter;
    QTextBrowser* resultPreview;

    QPushButton* openButton;
    QPushButton* searchButton;
    QPushButton* closeButton;

    ResourceType scopeType;
    Outline* scope;

protected:
    QStringList completerStrings;

    QCompleter* completer;
    QLabel* label;

public:
    explicit FtsDialog(QWidget* parent);
    FtsDialog(const FtsDialog&) = delete;
    FtsDialog(const FtsDialog&&) = delete;
    FtsDialog &operator=(const FtsDialog&) = delete;
    FtsDialog &operator=(const FtsDialog&&) = delete;
    ~FtsDialog();

    QPushButton* getSearchButton() const { return searchButton; }
    QPushButton* getOpenButton() const { return openButton; }
    NotesTableView* getResultListingView() const { return resultListingView; }
    NotesTablePresenter* getResultListingPresenter() const { return resultListingPresenter; }
    QTextBrowser* getResultPreview() const { return resultPreview; }

    void setScope(ResourceType t, Outline* s);
    void clearScope() {
        setScope(ResourceType::REPOSITORY, nullptr);
    }
    ResourceType getScopeType() { return scopeType; }
    Outline* getScope() { return scope; }

    QString getSearchPattern() const { return lineEdit->text(); }
    void setSearchPattern(const QString& searchPattern) { lineEdit->setText(searchPattern); }
    bool isEditorCaseInsensitive() const { return editorIgnoreCheck->isChecked(); }
    bool isEditorWords() const { return editorWordCheck->isChecked(); }
    bool isEditorReverse() const { return editorReverseCheck->isChecked(); }
    bool isExact() const { return exactRadio->isChecked(); }
    bool isCaseInsensitive() const { return ignoreRadio->isChecked(); }
    bool isRegex() const { return regexRadio->isChecked(); }

    void show() {
        lineEdit->selectAll();
        lineEdit->setFocus();
        QDialog::show();
    }
    void hideResult() {
        resultSplit->setVisible(false);
        setSizeSearchFacet();
    }
    void updateFacet();

    void refreshResult(std::vector<Note*>* notes);
    int getResultSize() const { return resultListingPresenter->getModel()->rowCount(); }

private:
    void setSizeSearchFacet();
    void setSizeResultFacet();

signals:
    void signalNoteScopeSearch();

public slots:
    void searchAndAddPatternToHistory();

private slots:
    void enableSearchButton(const QString &text);
};

}
#endif // M8RUI_FTS_DIALOG_H

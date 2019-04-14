/*
 fts_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
    QPushButton* findButton;
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

    void setScope(ResourceType t, Outline* s);
    void clearScope() {
        setScope(ResourceType::REPOSITORY, nullptr);
    }
    ResourceType getScopeType() { return scopeType; }
    Outline* getScope() { return scope; }

    QString getSearchPattern() const { return lineEdit->text(); }
    bool isEditorCaseInsensitive() const { return editorIgnoreCheck->isChecked(); }
    bool isEditorWords() const { return editorWordCheck->isChecked(); }
    bool isEditorReverse() const { return editorReverseCheck->isChecked(); }
    bool isExact() const { return exactRadio->isChecked(); }
    bool isCaseInsensitive() const { return ignoreRadio->isChecked(); }
    bool isRegex() const { return regexRadio->isChecked(); }
    QPushButton* getFindButton() const { return findButton; }

    void show() { lineEdit->selectAll(); lineEdit->setFocus(); QDialog::show(); }
    void updateFacet();

private slots:
    void enableFindButton(const QString &text);
    void addExpressionToHistory();
};

}
#endif // M8RUI_FTS_DIALOG_H

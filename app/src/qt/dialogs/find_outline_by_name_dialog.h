/*
 find_outline_by_name_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_FIND_OUTLINE_BY_NAME_DIALOG_H
#define M8RUI_FIND_OUTLINE_BY_NAME_DIALOG_H

#include <vector>

#include <QtWidgets>

#include "../../lib/src/mind/ontology/thing_class_rel_triple.h"

namespace m8r {

class FindOutlineByNameDialog : public QDialog
{
    Q_OBJECT

    class MyLineEdit : public QLineEdit
    {
    private:
        QListView* target;
    public:
        explicit MyLineEdit(QListView* t, QWidget* parent)
            : QLineEdit(parent), target(t)
        {}
        void keyPressEvent(QKeyEvent* event) override {
            if(event->key() == Qt::Key_Down) {
                // find the first visible row
                for(int row = 0; row<target->model()->rowCount(); row++) {
                    if(!target->isRowHidden(row)) {
                        target->setCurrentIndex(target->model()->index(row,0));
                        break;
                    }
                }
                target->setFocus();
            }
            QLineEdit::keyPressEvent(event);
        }
    };

private:
    MyLineEdit* lineEdit;
    QListView* listView;
    QStringList listViewStrings;
    QStringListModel listViewModel;
    QCheckBox* caseCheckBox;
    QCheckBox* keywordsCheckBox;
    QPushButton* closeButton;

    Thing* choice;
    std::vector<Thing*> things;

protected:
    QLabel* label;
    QCheckBox* scopeCheckBox;
    QPushButton* findButton;

public:
    explicit FindOutlineByNameDialog(QWidget* parent);
    FindOutlineByNameDialog(const FindOutlineByNameDialog&) = delete;
    FindOutlineByNameDialog(const FindOutlineByNameDialog&&) = delete;
    FindOutlineByNameDialog &operator=(const FindOutlineByNameDialog&) = delete;
    FindOutlineByNameDialog &operator=(const FindOutlineByNameDialog&&) = delete;
    ~FindOutlineByNameDialog();

    QString getSearchedString() const { return lineEdit->text(); }
    void setSearchedString(const QString& text) { lineEdit->setText(text); }
    QCheckBox* getCaseCheckbox() const { return caseCheckBox; }
    QCheckBox* getKeywordsCheckbox() const { return keywordsCheckBox; }
    QPushButton* getFindButton() const { return findButton; }
    Thing* getChoice() const { return choice; }

    void show(std::vector<Thing*>& outlines, std::vector<std::string>* customizedNames=nullptr, bool showScopeCheck=false);

signals:
    void searchFinished();

private slots:
    void enableFindButton(const QString &text);
    void handleChoice();
    void handleReturn();
};

}
#endif // M8RUI_FIND_OUTLINE_BY_NAME_DIALOG_H

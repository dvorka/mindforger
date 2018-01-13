/*
 find_outline_by_name_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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

namespace m8r {

class FindOutlineByNameDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* label;
    QLineEdit* lineEdit;
    QListView* listView;
    QStringList listViewStrings;
    QStringList filteredListViewStrings;
    QStringListModel listViewModel;
    QCheckBox* caseCheckBox;
    QPushButton* findButton;
    QPushButton* closeButton;

    QString choice;

public:
    explicit FindOutlineByNameDialog(QWidget* parent);
    FindOutlineByNameDialog(const FindOutlineByNameDialog&) = delete;
    FindOutlineByNameDialog(const FindOutlineByNameDialog&&) = delete;
    FindOutlineByNameDialog &operator=(const FindOutlineByNameDialog&) = delete;
    FindOutlineByNameDialog &operator=(const FindOutlineByNameDialog&&) = delete;
    ~FindOutlineByNameDialog();

    QString getSearchedString() const { return lineEdit->text(); }
    QCheckBox* getCaseCheckbox(void) const { return caseCheckBox; }
    QPushButton* getFindButton(void) const { return findButton; }
    QString& getChoice(void) { return choice; }

    void show(std::vector<std::string>* outlineNames);

signals:
    void searchFinished(void);

private slots:
    void enableFindButton(const QString &text);
    void handleChoice(void);
    void handleReturn(void);
};

}
#endif // M8RUI_FIND_OUTLINE_BY_NAME_DIALOG_H

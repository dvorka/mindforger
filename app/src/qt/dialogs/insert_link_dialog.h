/*
 insert_link_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_INSERT_LINK_DIALOG_H
#define M8RUI_INSERT_LINK_DIALOG_H

#include <vector>

#include <QtWidgets>

#include "find_outline_by_name_dialog.h"
#include "find_note_by_name_dialog.h"

namespace m8r {

class InsertLinkDialog : public QDialog
{
    Q_OBJECT

private:
    std::vector<Thing*> outlines;
    std::vector<Note*> notes;

    FindOutlineByNameDialog* findOutlineByNameDialog;
    FindNoteByNameDialog* findNoteByNameDialog;

    QLabel* linkTextLabel;
    QLineEdit* linkTextEdit;
    QLabel* pathLabel;
    QLineEdit* pathEdit;

    QPushButton* findOutlineButton;
    QPushButton* findNoteButton;
    QPushButton* findFileButton;
    QPushButton* findDirectoryButton;

    QCheckBox* copyToRepoCheckBox;

    QPushButton* insertButton;
    QPushButton* closeButton;

public:
    explicit InsertLinkDialog(QWidget* parent);
    InsertLinkDialog(const InsertLinkDialog&) = delete;
    InsertLinkDialog(const InsertLinkDialog&&) = delete;
    InsertLinkDialog &operator=(const InsertLinkDialog&) = delete;
    InsertLinkDialog &operator=(const InsertLinkDialog&&) = delete;
    ~InsertLinkDialog();

    void show(std::vector<Thing*>& outlines, std::vector<Note*>& notes);
    QPushButton* getInsertButton() const { return insertButton; }
    QString getLinkText() { return linkTextEdit->text(); }
    QString getPathText() { return pathEdit->text(); }

private slots:
    void handleFindOutline();
    void handleFindNote();
    void handleFindOutlineChoice();
    void handleFindNoteChoice();
    void handleFindFile();
    void handleFindDirectory();
};

}
#endif // M8RUI_INSERT_LINK_DIALOG_H

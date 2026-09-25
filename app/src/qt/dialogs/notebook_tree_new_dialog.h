/*
 notebook_tree_new_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_NOTEBOOK_TREE_NEW_DIALOG_H
#define M8RUI_NOTEBOOK_TREE_NEW_DIALOG_H

#include <QtWidgets>

#include "../../lib/src/model/notebook_tree.h"

namespace m8r {

/**
 * @brief Dialog to create a new Notebook tree or rename an existing one.
 */
class NotebookTreeNewDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* nameLabel;
    QLineEdit* nameEdit;

    QPushButton* createButton;
    QPushButton* closeButton;

    NotebookTree* notebookTreeToEdit;

public:
    explicit NotebookTreeNewDialog(QWidget* parent);
    NotebookTreeNewDialog(const NotebookTreeNewDialog&) = delete;
    NotebookTreeNewDialog(const NotebookTreeNewDialog&&) = delete;
    NotebookTreeNewDialog &operator=(const NotebookTreeNewDialog&) = delete;
    NotebookTreeNewDialog &operator=(const NotebookTreeNewDialog&&) = delete;
    ~NotebookTreeNewDialog();

    QString getNotebookTreeName() const { return nameEdit->text(); }
    NotebookTree* getNotebookTreeToEdit() const { return notebookTreeToEdit; }

    void show(NotebookTree* notebookTreeToEdit=nullptr);

signals:
    void createFinished();

private slots:
    void handleCreate();
};

}
#endif // M8RUI_NOTEBOOK_TREE_NEW_DIALOG_H

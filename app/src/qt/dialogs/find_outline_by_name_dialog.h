/*
 find_outline_by_name_dialog.h     MindForger thinking notebook

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
        FindOutlineByNameDialog* dialog;
        QListView* target;
    public:
        explicit MyLineEdit(QListView* t, FindOutlineByNameDialog* parent);
        void keyPressEvent(QKeyEvent* event) override;
    };

    // filters listViewModel's rows against 'names' for efficient searching
    class NameFilterProxyModel : public QSortFilterProxyModel
    {
    public:
        explicit NameFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}

        void setNames(const QVector<QString>* n) { names = n; }

        void setFilterState(const QString& text, bool keywords, bool ignoreCase) {
            filterText = text;
            keywordsMode = keywords;
            caseSensitivity = ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive;
            invalidateFilter();
        }

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

    private:
        const QVector<QString>* names{};
        QString filterText{};
        bool keywordsMode{true};
        Qt::CaseSensitivity caseSensitivity{Qt::CaseInsensitive};
    };

private:
    MyLineEdit* lineEdit;
    QListView* listView;
    QStringList listViewStrings;
    QStringListModel listViewModel;
    NameFilterProxyModel* proxyModel;
    QCheckBox* caseCheckBox;
    QCheckBox* keywordsCheckBox;
    QPushButton* closeButton;

    Thing* choice;
    std::vector<Thing*> things;
    // things' names as QStrings, precomputed once in show()
    QVector<QString> cachedNames;
    // debounces filterNow() so that rapid typing/backspacing triggers 1 filter pass
    QTimer* filterDebounceTimer;

    // maps a source model row to the Thing behind it (nullptr if there is none)
    Thing* getThing(const QModelIndex& sourceIndex) const;
    // runs a pending debounced filter pass so that the proxy model is up to date
    void flushPendingFilter();

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
    void setSearchedString(const QString& text) {lineEdit->setText(text); }
    QCheckBox* getCaseCheckbox() const { return caseCheckBox; }
    QCheckBox* getKeywordsCheckbox() const { return keywordsCheckBox; }
    QPushButton* getFindButton() const { return findButton; }
    Thing* getChoice() const { return choice; }

    void show(
        std::vector<Thing*>& outlines,
        std::vector<std::string>* customizedNames=nullptr,
        bool showScopeCheck=false,
        bool init=true
    );

signals:
    void searchFinished();

private slots:
    void enableFindButton(const QString &text);
    void filterNow();
    void handleChoice();
    void handleReturn();
};

}
#endif // M8RUI_FIND_OUTLINE_BY_NAME_DIALOG_H

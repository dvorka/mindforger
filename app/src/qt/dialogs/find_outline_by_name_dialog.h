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
        QListView* target;
    public:
        explicit MyLineEdit(QListView* t, QWidget* parent)
            : QLineEdit(parent), target(t)
        {}
        void keyPressEvent(QKeyEvent* event) override {
            if(event->key() == Qt::Key_Down) {
                // give focus to the 1st (only visible rows remain in the filtered proxy
                // model) row - consume the event so that it is NOT also handled by
                // QLineEdit which would scroll the list view instead
                if(target->model()->rowCount()>0) {
                    QModelIndex index = target->model()->index(0,0);
                    target->setCurrentIndex(index);
                    target->scrollTo(index, QAbstractItemView::PositionAtTop);
                }
                target->setFocus();
                event->accept();
                return;
            }
            QLineEdit::keyPressEvent(event);
        }
    };

    // filters listViewModel's rows against 'names' (things' names, kept in lockstep with
    // listViewModel's rows by show()) instead of QListView::setRowHidden() being called
    // once per row from C++ - hiding/showing thousands of rows via per-row widget calls
    // is what made backspacing over a long query noticeably slower than typing it
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
    // things' names as QStrings, precomputed once in show() so that the filter pass in
    // filterNow() doesn't reallocate a QString from std::string for every one of e.g.
    // 21k notes on every keystroke; also what NameFilterProxyModel filters against
    QVector<QString> cachedNames;
    // debounces filterNow() so that rapid typing/backspacing (incl. key-repeat) triggers
    // a single filter pass instead of one full O(things) pass per keystroke
    QTimer* filterDebounceTimer;

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

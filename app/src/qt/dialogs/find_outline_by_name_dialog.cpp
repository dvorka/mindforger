/*
 find_outline_by_name_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "find_outline_by_name_dialog.h"

#include "../gear/qutils.h"

namespace m8r {

using namespace std;

FindOutlineByNameDialog::MyLineEdit::MyLineEdit(
        QListView* t,
        FindOutlineByNameDialog* parent)
    : QLineEdit(parent), dialog(parent), target(t)
{
}

void FindOutlineByNameDialog::MyLineEdit::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Down) {
        // debounced filter pass may still be pending -> must be run first otherwise
        // the 1st row might not match what has just been typed
        dialog->flushPendingFilter();

        // give focus to the 1st
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

FindOutlineByNameDialog::FindOutlineByNameDialog(QWidget *parent)
    : QDialog(parent)
{
    // widgets
    listView = new QListView(this);
    // list view model must be set - enables the use of string lists controlling its content
    proxyModel = new NameFilterProxyModel(this);
    proxyModel->setSourceModel(&listViewModel);
    proxyModel->setNames(&cachedNames);
    listView->setModel(proxyModel);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    label = new QLabel{tr("&Notebook name:")};
    lineEdit = new MyLineEdit{listView, this};
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox{tr("&ignore case")};
    caseCheckBox->setChecked(true);
    keywordsCheckBox = new QCheckBox{tr("&keywords match")};
    keywordsCheckBox->setChecked(true);
    scopeCheckBox = new QCheckBox{tr("&current Notebook's Notes only")};
    scopeCheckBox->setChecked(false);
    scopeCheckBox->setVisible(false);


    findButton = new QPushButton{tr("&Open Notebook")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton{tr("&Cancel")};

    // debounce expensive filter pass so that fast typing/backspacing collapses to 1 pass
    filterDebounceTimer = new QTimer(this);
    filterDebounceTimer->setSingleShot(true);
    filterDebounceTimer->setInterval(60);

    // signals
    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString&)));
    connect(filterDebounceTimer, SIGNAL(timeout()), this, SLOT(filterNow()));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(handleReturn()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(handleChoice()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout *mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    mainLayout->addWidget(listView);
    mainLayout->addWidget(caseCheckBox);
    mainLayout->addWidget(keywordsCheckBox);
    mainLayout->addWidget(scopeCheckBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Find Notebook by Name"));
    // height is set to make sure listview gets enough lines
    resize(fontMetrics().averageCharWidth()*55, fontMetrics().height()*30);
    setModal(true);
}

FindOutlineByNameDialog::~FindOutlineByNameDialog()
{
    delete label;
    delete lineEdit;
    delete listView;
    delete caseCheckBox;
    delete findButton;
    delete closeButton;
}

void FindOutlineByNameDialog::show(
        vector<Thing*>& ts,
        vector<string>* customizedNames,
        bool showScopeCheck,
        bool init)
{
    choice = nullptr;

    scopeCheckBox->setEnabled(false); // TODO WIP
    if(showScopeCheck) {
        scopeCheckBox->setVisible(true);
        scopeCheckBox->setChecked(true);
    } else {
        scopeCheckBox->setVisible(false);
    }

    things.clear();
    listViewStrings.clear();
    cachedNames.clear();
    bool useCustomNames = customizedNames!=nullptr && customizedNames->size()>0;
    if(ts.size()) {
        for(size_t i=0; i<ts.size(); i++) {
            things.push_back(ts[i]);
            // name used for filtering (see filterNow()) - cached once here rather than
            // re-derived from std::string on every keystroke
            QString name = ts.at(i)->getName().size()
                ? QString::fromStdString(ts[i]->getName())
                : QString{};
            cachedNames.push_back(name);
            if(useCustomNames) {
                listViewStrings << QString::fromStdString(customizedNames->at(i));
            } else {
                listViewStrings << name;
            }
        }
    }
    // model must be refreshed even when there is nothing to show - avoid displaying old rows
    listViewModel.setStringList(listViewStrings);
    findButton->setEnabled(things.size());

    // filter pass is run synchronously (not debounced) here
    if(init) {
        lineEdit->clear();
        lineEdit->setFocus();
    }
    filterDebounceTimer->stop();
    filterNow();

    QDialog::show();
}

void FindOutlineByNameDialog::enableFindButton(const QString&)
{
    filterDebounceTimer->start();
}

void FindOutlineByNameDialog::filterNow()
{
    bool keywords = keywordsCheckBox->isEnabled() && keywordsCheckBox->isChecked();
    proxyModel->setFilterState(lineEdit->text(), keywords, caseCheckBox->isChecked());
    findButton->setEnabled(proxyModel->rowCount());
}

bool FindOutlineByNameDialog::NameFilterProxyModel::filterAcceptsRow(
    int sourceRow, const QModelIndex& sourceParent) const
{
    Q_UNUSED(sourceParent);

    if(names==nullptr || sourceRow<0 || sourceRow>=names->size()) {
        return false;
    }
    if(filterText.isEmpty()) {
        return true;
    }

    const QString& s = names->at(sourceRow);
    if(keywordsMode) {
        return stringMatchByKeywords(filterText, s, caseSensitivity==Qt::CaseInsensitive);
    } else {
        return s.startsWith(filterText, caseSensitivity);
    }
}

void FindOutlineByNameDialog::flushPendingFilter()
{
    if(filterDebounceTimer->isActive()) {
        filterDebounceTimer->stop();
        filterNow();
    }
}

Thing* FindOutlineByNameDialog::getThing(const QModelIndex& sourceIndex) const
{
    // the proxy model's source rows and things are kept in lockstep by show()
    if(!sourceIndex.isValid()
        || sourceIndex.row()<0
        || static_cast<size_t>(sourceIndex.row())>=things.size()
    ) {
        MF_DEBUG(
            "FindOutlineByNameDialog: no thing for source row "
            << sourceIndex.row() << " of " << things.size() << endl
        );
        return nullptr;
    }

    return things[sourceIndex.row()];
}

void FindOutlineByNameDialog::handleReturn()
{
    // flush a pending debounced filter so the proxy model below is up to date
    flushPendingFilter();

    if(findButton->isEnabled()) {
        if(proxyModel->rowCount()>0) {
            QModelIndex sourceIndex = proxyModel->mapToSource(proxyModel->index(0,0));
            choice = getThing(sourceIndex);
        }

        QDialog::close();
        emit searchFinished();
    }
}

void FindOutlineByNameDialog::handleChoice()
{
    // flush a pending debounced filter so the proxy model below is up to date
    flushPendingFilter();

    if(listView->currentIndex().isValid()) {
        QModelIndex sourceIndex = proxyModel->mapToSource(listView->currentIndex());
        choice = getThing(sourceIndex);

        QDialog::close();
        emit searchFinished();
    }
}

} // m8r namespace

/*
 find_note_by_metadata_dialog.cpp     MindForger thinking notebook

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
#include "find_note_by_metadata_dialog.h"

#include "../../lib/src/gear/datetime_utils.h"
#include "../../lib/src/model/outline.h"

#include "../html_delegate.h"

namespace m8r {

using namespace std;

FindNoteByMetadataDialog::FindNoteByMetadataDialog(QWidget* parent)
    : QDialog(parent),
      choice{nullptr}
{
    // widgets
    model = new QStandardItemModel{this};
    proxyModel = new QSortFilterProxyModel{this};
    proxyModel->setSourceModel(model);
    // sort by the raw values (bytes, counts, timestamps) instead of the shown text
    proxyModel->setSortRole(ROLE_SORT);
    // rows are sorted once they all are added - not on every added row
    proxyModel->setDynamicSortFilter(false);

    tableView = new QTableView{this};
    tableView->setModel(proxyModel);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSortingEnabled(true);
    tableView->setWordWrap(false);
    tableView->verticalHeader()->setVisible(false);
    // fixed row height (ResizeToContents kills performance of big tables) w/ space for timestamp badges
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);
    // timestamps are rendered as HTML badges (like in Notebook outline)
    HtmlDelegate* htmlDelegate = new HtmlDelegate{};
    htmlDelegate->setParent(tableView);
    tableView->setItemDelegateForColumn(COLUMN_CREATED, htmlDelegate);
    tableView->setItemDelegateForColumn(COLUMN_MODIFIED, htmlDelegate);
    initColumns();

    openButton = new QPushButton{tr("&Open Note")};
    openButton->setEnabled(false);
    closeButton = new QPushButton{tr("&Cancel")};
    // Enter in the table activates the row - buttons must not catch it as well
    openButton->setAutoDefault(false);
    closeButton->setAutoDefault(false);

    // signals
    connect(tableView, SIGNAL(activated(QModelIndex)), this, SLOT(handleChoice()));
    connect(openButton, SIGNAL(clicked()), this, SLOT(handleChoice()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(
        tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(handleSelectionChanged()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(tableView);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(openButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Find Note by Metadata"));
    resize(fontMetrics().averageCharWidth()*100, fontMetrics().height()*30);
    setModal(true);
}

FindNoteByMetadataDialog::~FindNoteByMetadataDialog()
{
    delete tableView;
    delete openButton;
    delete closeButton;
}

void FindNoteByMetadataDialog::initColumns()
{
    model->setHorizontalHeaderLabels(
        QStringList{}
        << tr("Note")
        << tr("Notebook")
        << tr("Size")
        << tr("Rs")
        << tr("Ws")
        << tr("Created")
        << tr("Modified"));
    model->horizontalHeaderItem(COLUMN_SIZE)->setToolTip(tr("Size of the Note text in bytes"));
    model->horizontalHeaderItem(COLUMN_READS)->setToolTip(tr("Reads"));
    model->horizontalHeaderItem(COLUMN_WRITES)->setToolTip(tr("Writes"));

    // table fits the dialog: names get the remaining space (clipped), other columns have fixed width
    QHeaderView* header = tableView->horizontalHeader();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Fixed);
    header->setSectionResizeMode(COLUMN_NOTE, QHeaderView::Stretch);
    header->setSectionResizeMode(COLUMN_NOTEBOOK, QHeaderView::Stretch);
    const int charWidth = fontMetrics().averageCharWidth();
    tableView->setColumnWidth(COLUMN_SIZE, charWidth*9);
    tableView->setColumnWidth(COLUMN_READS, charWidth*5);
    tableView->setColumnWidth(COLUMN_WRITES, charWidth*5);
    tableView->setColumnWidth(COLUMN_CREATED, charWidth*12);
    tableView->setColumnWidth(COLUMN_MODIFIED, charWidth*12);
}

void FindNoteByMetadataDialog::show(const vector<Note*>& notes, bool showNotebook)
{
    choice = nullptr;

    // remove rows only - columns keep their layout
    model->removeRows(0, model->rowCount());
    tableView->setColumnHidden(COLUMN_NOTEBOOK, !showNotebook);

    for(Note* note:notes) {
        addRow(note);
    }
    // the largest Notes first - proxy is sorted explicitly as the header sorts
    // only if its indicator is changed (which is not the case on re-open)
    tableView->horizontalHeader()->setSortIndicator(COLUMN_SIZE, Qt::DescendingOrder);
    proxyModel->sort(COLUMN_SIZE, Qt::DescendingOrder);

    if(model->rowCount() > 0) {
        tableView->selectRow(0);
    }
    tableView->setFocus();

    QDialog::show();
}

void FindNoteByMetadataDialog::addRow(Note* note)
{
    QList<QStandardItem*> items{};

    // note
    const QString name{QString::fromStdString(note->getName())};
    QStandardItem* item = new QStandardItem{name};
    item->setToolTip(name);
    item->setData(QVariant::fromValue(note), ROLE_NOTE);
    item->setData(name.toLower(), ROLE_SORT);
    items += item;

    // notebook
    const QString outlineName{
        note->getOutline() ? QString::fromStdString(note->getOutline()->getName()) : QString{}};
    item = new QStandardItem{outlineName};
    item->setToolTip(outlineName);
    item->setData(outlineName.toLower(), ROLE_SORT);
    items += item;

    // numbers
    const qulonglong size = static_cast<qulonglong>(note->getDescriptionSize());
    const unsigned reads = note->getReads();
    const unsigned writes = note->getRevision();
    for(qulonglong n:{size, static_cast<qulonglong>(reads), static_cast<qulonglong>(writes)}) {
        item = new QStandardItem{QString::number(n)};
        item->setData(n, ROLE_SORT);
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        items += item;
    }

    // timestamps: pretty HTML badges like in Notebook outline, sorted by the timestamp
    for(time_t t:{note->getCreated(), note->getModified()}) {
        item = new QStandardItem{QString::fromStdString(datetimeToPrettyHtml(t))};
        item->setToolTip(QString::fromStdString(datetimeToString(t)));
        item->setData(static_cast<qlonglong>(t), ROLE_SORT);
        items += item;
    }

    model->appendRow(items);
}

void FindNoteByMetadataDialog::handleSelectionChanged()
{
    openButton->setEnabled(tableView->selectionModel()->hasSelection());
}

void FindNoteByMetadataDialog::handleChoice()
{
    const QModelIndex index = tableView->currentIndex();
    if(!index.isValid()) {
        return;
    }

    choice = index.sibling(index.row(), COLUMN_NOTE).data(ROLE_NOTE).value<Note*>();
    if(choice) {
        QDialog::close();
        emit searchFinished();
    }
}

} // m8r namespace

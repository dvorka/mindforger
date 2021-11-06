/*
 edit_tags_panel.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "edit_tags_panel.h"

namespace m8r {

using namespace std;

EditTagsPanel::EditTagsPanel(
        Ontology& ontology,
        QWidget *parent)
    : QGroupBox(tr("Tags"), parent), ontology(ontology)
{
    // widgets
    lineEdit = new MyLineEdit{this, parent};
    lineEdit->setToolTip(tr("Hit Ctrl+Enter to add tag"));
    completer = new QCompleter{completerStrings, this};
    completer->setCompletionMode(QCompleter::CompletionMode::UnfilteredPopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    listView = new QListView{this};
    // list view model must be set - use of this type of mode enable the use of string lists controlling its content
    listView->setModel(&listViewModel);
    // disable ability to edit list items
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    addButton = new QPushButton{tr("Add"), this};
    removeButton = new QPushButton{tr("Remove"), this};

    // assembly
    QWidget* w = new QWidget(this);
    QHBoxLayout* h = new QHBoxLayout{w};
    h->addWidget(lineEdit);
    h->addWidget(addButton);
    h->setContentsMargins(QMargins(0,0,0,0));
    w->setLayout(h);
    layout = new QVBoxLayout{this};
    layout->addWidget(w);
    layout->addWidget(listView);
    layout->addWidget(removeButton);
    setLayout(layout);

    // signals
    QObject::connect(addButton, SIGNAL(clicked()), this, SLOT(slotAddTag()));
    QObject::connect(removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveTag()));
}

EditTagsPanel::~EditTagsPanel()
{
}

void EditTagsPanel::clearTagList()
{
    lineEdit->clear();
    listViewStrings.clear();
    ((QStringListModel*)listView->model())->setStringList(listViewStrings);
}

void EditTagsPanel::refreshOntologyTags()
{
    completerStrings.clear();
    if(ontology.getTags().size()) {
        for(const Tag* t:ontology.getTags().values()) {
            if(!stringistring(string("none"), t->getName())) {
                completerStrings << QString::fromStdString(t->getName());
            }
        }
    }
    ((QStringListModel*)completer->model())->setStringList(completerStrings);
}

void EditTagsPanel::refresh(const vector<const Tag*>* noteTags)
{
    lineEdit->clear();
    clearTagList();
    refreshOntologyTags();

    listViewStrings.clear();
    if(noteTags->size()) {
        for(const Tag* t:*noteTags) {
            if(!stringistring(string("none"), t->getName())) {
                listViewStrings << QString::fromStdString(t->getName());
            }
        }
    }
    ((QStringListModel*)listView->model())->setStringList(listViewStrings);
}

const std::vector<const Tag*>& EditTagsPanel::getTags()
{
    tags.clear();
    if(listViewStrings.size()) {
        for(QString& s:listViewStrings) {
            tags.push_back(ontology.findOrCreateTag(s.toStdString()));
        }
    }
    return tags;
}


vector<string>& EditTagsPanel::getTagsAsStrings(vector<string>& tags) const {
    if(listViewStrings.size()) {
        for(const QString& s:listViewStrings) {
            tags.push_back(s.toStdString());
        }
    }
    return tags;
}

set<string>& EditTagsPanel::getTagsAsStringSet(set<string>& tagSet) const {
    if(listViewStrings.size()) {
        for(const QString& s:listViewStrings) {
            tagSet.insert(s.toStdString());
        }
    }
    return tagSet;
}

void EditTagsPanel::setTags(const std::vector<const Tag*>& tags)
{
    clearTagList();
    for(const Tag* t:tags) {
        lineEdit->setText(QString::fromStdString(t->getName()));
        slotAddTag();
    }
    lineEdit->clear();
}

void EditTagsPanel::setTagsAsStrings(const std::vector<string>& tagsStrings)
{
    clearTagList();
    for(string s:tagsStrings) {
        lineEdit->setText(QString::fromStdString(s));
        slotAddTag();
    }
    lineEdit->clear();
}

void EditTagsPanel::setTagsAsStrings(const std::set<string>& tagsStrings)
{
    clearTagList();
    for(string s:tagsStrings) {
        lineEdit->setText(QString::fromStdString(s));
        slotAddTag();
    }
    lineEdit->clear();
}

void EditTagsPanel::slotAddTag()
{
    if(!lineEdit->text().isEmpty()) {
        if(!listViewStrings.contains(lineEdit->text())) {
            // escape Tag as , character is forbidden
            string rawTag{lineEdit->text().toStdString()};
            QString escapedTag = QString::fromStdString(Tag::escape(rawTag));
            lineEdit->setText(escapedTag);
            listViewStrings << escapedTag;
            ((QStringListModel*)listView->model())->setStringList(listViewStrings);
            emit signalTagSelectionChanged();
        }
    }
}

void EditTagsPanel::slotRemoveTag()
{
    if(listView->currentIndex().isValid()) {
        listViewStrings.removeAt(listView->currentIndex().row());
        ((QStringListModel*)listView->model())->setStringList(listViewStrings);
        emit signalTagSelectionChanged();
    }
}

} // m8r namespace

/*
 edit_tags_panel.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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

EditTagsPanel::TagLineEdit::TagLineEdit(EditTagsPanel* tagsPanel, QWidget* parent)
    : QLineEdit(parent), tagsPanel(tagsPanel)
{}

void EditTagsPanel::TagLineEdit::keyPressEvent(QKeyEvent* event)
{
    if(event->modifiers() & Qt::ControlModifier){
        switch(event->key()) {
        case Qt::Key_Return: // Qt::Key_Enter is keypad Enter
            tagsPanel->slotAddTag();
            break;
        }
    } else {
        if(!text().size()) {
            switch(event->key()) {
            case Qt::Key_Down:
                tagsPanel->setFocusTagList();
                break;
            }
        }
    }
    QLineEdit::keyPressEvent(event);

    // notify about key pressed
    tagsPanel->customLineEditKeyPressEvent(event);
}

EditTagsPanel::TagsListView::TagsListView(EditTagsPanel* tagsPanel, QWidget* parent)
    : QListView(parent),
      tagsPanel{tagsPanel}
{}

void EditTagsPanel::TagsListView::keyPressEvent(QKeyEvent* event)
{
    if(event->modifiers() & Qt::ControlModifier){
        switch(event->key()) {
        case Qt::Key_D:
            MF_DEBUG("Edit tags: Cmd-D" << endl);
            tagsPanel->slotRemoveTag();
            break;
        }
    } else {
        switch(event->key()) {
        case Qt::Key_Delete:
            MF_DEBUG("Edit tags: DEL" << endl);
            tagsPanel->slotRemoveTag();
            break;
        }
    }
    QListView::keyPressEvent(event);
}

EditTagsPanel::EditTagsPanel(
        MfWidgetMode mode,
        Ontology& ontology,
        QWidget* parent)
    : QGroupBox(tr("Tags"), parent),
      mode{mode},
      ontology(ontology)
{
    // widgets
    lineEdit = new TagLineEdit{this, this};
    lineEdit->setToolTip(
#ifdef __APPLE__
        tr("Hit ⌘↩ to add tag")
#else
        tr("Hit Ctrl+Enter to add tag")
#endif
    );
    completer = new QCompleter{completerStrings, this};
    completer->setCompletionMode(QCompleter::CompletionMode::UnfilteredPopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    listView = new TagsListView{this, this};
    // list view model must be set - use of this type of mode enable the use of string lists controlling its content
    listView->setModel(&listViewModel);
    // disable ability to edit list items
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    addButton = new QPushButton{
        mode == MfWidgetMode::FIND_MODE?tr("Add Filter Tag"):tr("Create New Tag"),
        this
    };
    addButton->setToolTip(
#ifdef __APPLE__
        tr("Hit ⌘↩ to add tag")
#else
        tr("Hit Ctrl+Enter to add tag")
#endif
    );

    removeButton = new QPushButton{tr("Remove Tag"), this};

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
    addButton->setText(
        mode == MfWidgetMode::FIND_MODE?tr("Add Filter Tag"):tr("Create New Tag")
    );
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

void EditTagsPanel::customLineEditKeyPressEvent(QKeyEvent* event)
{
    UNUSED_ARG(event);
    if(!lineEdit->text().isEmpty()
       && completerStrings.contains(lineEdit->text())
    ) {
        addButton->setText(
            mode == MfWidgetMode::FIND_MODE?tr("Add Filter Tag"):tr("Add Existing Tag")
        );
    } else {
        if(mode == MfWidgetMode::FIND_MODE) {
            addButton->setText(
                lineEdit->text().isEmpty()?tr("Add Filter Tag"):tr("Unknown Tag")
            );
        } else {
            addButton->setText(tr("Create New Tag"));
        }
    }
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

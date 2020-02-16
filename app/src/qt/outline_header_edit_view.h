/*
 outline_header_edit_view.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_OUTLINE_HEADER_EDIT_VIEW_H
#define M8RUI_OUTLINE_HEADER_EDIT_VIEW_H

#include <QtWidgets>

#include "../../lib/src/model/outline.h"

#include "note_editor_view.h"
#include "widgets/edit_name_panel.h"
#include "widgets/edit_buttons_panel.h"

namespace m8r {

class OutlineHeaderEditView : public QWidget
{
    Q_OBJECT

private:
    Outline* currentOutline;

    EditNamePanel* topNamePanel;
    NoteEditorView* noteEditor;
    EditButtonsPanel* bottomButtonsPanel;

public:
    explicit OutlineHeaderEditView(QWidget* parent);
    OutlineHeaderEditView(const OutlineHeaderEditView&) = delete;
    OutlineHeaderEditView(const OutlineHeaderEditView&&) = delete;
    OutlineHeaderEditView &operator=(const OutlineHeaderEditView&) = delete;
    OutlineHeaderEditView &operator=(const OutlineHeaderEditView&&) = delete;
    ~OutlineHeaderEditView();

    void setOutlineHeaderEditDialog(OutlineHeaderEditDialog* outlineHeaderEditDialog) {
        bottomButtonsPanel->setOutlineHeaderEditDialog(outlineHeaderEditDialog);
    }
    void setOutline(Outline* outline, std::string mdDescription) {
        currentOutline = outline;
        topNamePanel->setOutline(outline);
        bottomButtonsPanel->setOutline(outline);
        noteEditor->setPlainText(QString::fromStdString(mdDescription));
    }
    void setEditorShowLineNumbers(bool show) { noteEditor->setShowLineNumbers(show); }
    void setStatusBar(const StatusBarView* statusBar) { noteEditor->setStatusBar(statusBar); }

    void focusName() { topNamePanel->focusName(); }
    QString getName() const { return topNamePanel->getName(); }
    QString getDescription() const { return noteEditor->toPlainText(); }
    bool isDescriptionEmpty() const { return noteEditor->toPlainText().isEmpty(); }
    QString getSelectedText() const { return noteEditor->getSelectedText(); }
    NoteEditorView* getHeaderEditor() const { return noteEditor; }
    EditButtonsPanel* getButtonsPanel() const { return bottomButtonsPanel; }

    void giveEditorFocus() { noteEditor->setFocus(); }

private slots:
    void slotOpenOutlineHeaderPropertiesEditor();
    void slotSaveOutlineHeader();
    void slotCloseEditor();
    void slotSaveAndCloseEditor();

signals:
    void signalSaveAndCloseEditor();
    void signalCloseEditor();
    void signalSaveOutlineHeader();
};

}
#endif // M8RUI_OUTLINE_HEADER_EDIT_VIEW_H

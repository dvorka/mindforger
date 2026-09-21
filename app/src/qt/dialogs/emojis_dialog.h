/*
 emojis_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_EMOJIS_DIALOG_H
#define M8RUI_EMOJIS_DIALOG_H

#include <vector>

#include <QtWidgets>

#include "../../lib/src/representations/emoji.h"

namespace m8r {

/**
 * @brief Dialog with clickable and searchable emoji and other special characters.
 *
 * The dialog is fully keyboard driven - user types to filter the characters
 * by name/keyword and then inserts either by Ctrl+<digit>, or by moving
 * to a character with arrows and pressing Enter.
 */
class EmojisDialog : public QDialog
{
    Q_OBJECT

    static const int COLUMNS = 11;
    static const int SHORTCUTS = 9;

    /**
     * @brief Section of the dialog - a title and a grid of emoji buttons.
     */
    struct Section {
        QLabel* title;
        QWidget* grid;
        QGridLayout* gridLayout;
        std::vector<QPushButton*> buttons;
    };

public:
    explicit EmojisDialog(QWidget* parent);
    EmojisDialog(const EmojisDialog&) = delete;
    EmojisDialog(const EmojisDialog&&) = delete;
    EmojisDialog& operator=(const EmojisDialog&) = delete;
    EmojisDialog& operator=(const EmojisDialog&&) = delete;
    ~EmojisDialog();

signals:
    void emojiSelected(const QString& c);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void slotCharacterButtonClicked();
    void slotFilterChanged(const QString& filter);
    void slotShortcutActivated();

private:
    QLineEdit* filterEdit;
    QScrollArea* scrollArea;
    QPushButton* closeButton;

    std::vector<Section> sections;
    QHash<QPushButton*, const Emoji*> buttonEmojis;
    std::vector<QPushButton*> matchingButtons;

    void addSection(QBoxLayout* sectionsLayout, const EmojiSection& emojiSection);
    QString getSectionName(const EmojiSection& emojiSection) const;
    void decorateButton(QPushButton* button, const Emoji& emoji, int index);

    void insertEmoji(QPushButton* button);
    void insertMatchingEmoji(int index);
    void focusButton(int index);
    int getFocusedButtonIndex() const;
};

}

#endif // M8RUI_EMOJIS_DIALOG_H

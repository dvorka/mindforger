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

#include <initializer_list>

#include <QtWidgets>

namespace m8r {

/**
 * @brief Dialog with clickable emoji and other special characters.
 *
 * Clicking a character emits emojiSelected() so that the caller can
 * insert it to whatever name/description field is currently edited.
 * The dialog stays open after a click so that several characters can
 * be inserted in a row - user closes it explicitly once done.
 */
class EmojisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmojisDialog(QWidget* parent);
    EmojisDialog(const EmojisDialog&) = delete;
    EmojisDialog(const EmojisDialog&&) = delete;
    EmojisDialog& operator=(const EmojisDialog&) = delete;
    EmojisDialog& operator=(const EmojisDialog&&) = delete;
    ~EmojisDialog();

signals:
    void emojiSelected(const QString& c);

private slots:
    void slotCharacterButtonClicked();

private:
    void addCharactersSection(
        QBoxLayout* mainLayout,
        const QString& title,
        std::initializer_list<QString> characters,
        int columns);
};

}

#endif // M8RUI_EMOJIS_DIALOG_H

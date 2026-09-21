/*
 emojis_dialog.cpp     MindForger thinking notebook

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
#include "emojis_dialog.h"

#include "../../lib/src/representations/unicode.h"

namespace m8r {

EmojisDialog::EmojisDialog(QWidget* parent)
    : QDialog(parent),
      navigator{COLUMNS}
{
    setWindowTitle(tr("Emojis"));

    QVBoxLayout* mainLayout = new QVBoxLayout{this};

    QLabel* introLabel = new QLabel{
        tr("Type to filter, insert with Enter or Ctrl+[number]:"),
        this};
    introLabel->setWordWrap(true);
    mainLayout->addWidget(introLabel);

    filterEdit = new QLineEdit{this};
    filterEdit->setClearButtonEnabled(true);
    filterEdit->setPlaceholderText(tr("Emoji name or keyword like 'rocket'"));
    mainLayout->addWidget(filterEdit);

    QWidget* sectionsWidget = new QWidget{this};
    QVBoxLayout* sectionsLayout = new QVBoxLayout{sectionsWidget};

    for(const EmojiSection& emojiSection:Emojis::getInstance().getSections()) {
        addSection(sectionsLayout, emojiSection);
    }

    sectionsLayout->addStretch();

    scrollArea = new QScrollArea{this};
    scrollArea->setWidget(sectionsWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    QLabel* linksLabel = new QLabel{
        tr(
            "More special unicode characters: "
            "<a href='https://unicode-table.com/en/'>Unicode Table</a>, "
            "<a href='https://emojipedia.org/'>Emojipedia</a>"),
        this};
    linksLabel->setOpenExternalLinks(true);
    linksLabel->setWordWrap(true);
    mainLayout->addWidget(linksLabel);

    closeButton = new QPushButton{tr("&Close"), this};
    // Enter must insert the chosen emoji > it must NOT close the dialog
    closeButton->setAutoDefault(false);
    closeButton->setDefault(false);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout* buttonsLayout = new QHBoxLayout{};
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonsLayout);

    for(int digit=1; digit<=SHORTCUTS; digit++) {
        new QShortcut(
            QKeySequence(Qt::CTRL+Qt::Key_0+digit),
            this, SLOT(slotShortcutActivated()));
        new QShortcut(
            QKeySequence(Qt::ALT+Qt::Key_0+digit),
            this, SLOT(slotShortcutActivated()));
    }

    connect(
        filterEdit, SIGNAL(textChanged(QString)),
        this, SLOT(slotFilterChanged(QString)));
    slotFilterChanged("");

    resize(560, 560);
}

EmojisDialog::~EmojisDialog()
{
}

QString EmojisDialog::getSectionName(const EmojiSection& emojiSection) const
{
    // section names are translated here - lupdate must find the literals to add
    const QHash<QString, QString> names{
        {"Emoji", tr("Emoji")},
        {"Greek alphabet", tr("Greek alphabet")},
        {"Math and statistics", tr("Math and statistics")},
        {"Physics", tr("Physics")}
    };

    const QString name = QString::fromStdString(emojiSection.getName());
    return names.value(name, name);
}

void EmojisDialog::addSection(
    QBoxLayout* sectionsLayout,
    const EmojiSection& emojiSection)
{
    Section section{};

    section.title = new QLabel{
        QString{"<b>%1:</b>"}.arg(getSectionName(emojiSection)), this};
    sectionsLayout->addWidget(section.title);

    section.grid = new QWidget{this};
    section.gridLayout = new QGridLayout{section.grid};
    section.gridLayout->setSpacing(2);

    for(const Emoji& emoji:emojiSection.getEmojis()) {
        QPushButton* button = new QPushButton{section.grid};
        button->setFixedSize(40, 32);
        // Enter is handled by the dialog - it must not trigger a random button
        button->setAutoDefault(false);
        button->setDefault(false);
        connect(
            button, SIGNAL(clicked()),
            this, SLOT(slotCharacterButtonClicked()));

        buttonEmojis.insert(button, &emoji);
        section.buttons.push_back(button);
    }

    sectionsLayout->addWidget(section.grid);
    sections.push_back(section);
}

void EmojisDialog::decorateButton(
    QPushButton* button,
    const Emoji& emoji,
    int index)
{
    const QString character = QString::fromStdString(emoji.getCharacter());
    const QString name = QString::fromStdString(emoji.getName());

    if(index < SHORTCUTS) {
        // subscript digit shows which Alt+<digit> shortcut inserts the emoji
        button->setText(
            character
            + QChar{static_cast<ushort>(U_CODE_SUBSCRIPT_ONE + index)});
        button->setToolTip(tr("Insert '%1' - Ctrl+%2").arg(name).arg(index+1));
    } else {
        button->setText(character);
        button->setToolTip(tr("Insert '%1'").arg(name));
    }
}

void EmojisDialog::slotFilterChanged(const QString& filter)
{
    const std::vector<std::string> words
        = Emojis::toSearchWords(filter.toStdString());

    matchingButtons.clear();
    std::vector<int> sectionSizes{};
    for(Section& section:sections) {
        // drop the previous positions - matching buttons are re-added below
        while(QLayoutItem* item = section.gridLayout->takeAt(0)) {
            delete item;
        }

        int row{0}, col{0}, matches{0};
        for(QPushButton* button:section.buttons) {
            const Emoji* emoji = buttonEmojis.value(button, nullptr);
            if(emoji && emoji->matches(words)) {
                decorateButton(
                    button, *emoji, static_cast<int>(matchingButtons.size()));
                section.gridLayout->addWidget(button, row, col);
                button->show();
                matchingButtons.push_back(button);
                matches++;

                if(++col >= COLUMNS) {
                    col = 0;
                    row++;
                }
            } else {
                button->hide();
            }
        }

        // an empty section would show up as a title with a gap below it
        section.title->setVisible(matches > 0);
        section.grid->setVisible(matches > 0);

        sectionSizes.push_back(matches);
    }

    navigator.setSections(sectionSizes);
}

void EmojisDialog::insertEmoji(QPushButton* button)
{
    const Emoji* emoji = buttonEmojis.value(button, nullptr);
    if(emoji) {
        emit emojiSelected(QString::fromStdString(emoji->getCharacter()));
    }
}

void EmojisDialog::insertMatchingEmoji(int index)
{
    if(index >= 0 && index < static_cast<int>(matchingButtons.size())) {
        insertEmoji(matchingButtons[static_cast<size_t>(index)]);
    }
}

void EmojisDialog::slotShortcutActivated()
{
    const QShortcut* shortcut = qobject_cast<QShortcut*>(sender());
    if(!shortcut) {
        return;
    }

    // the last char of the shortcut is the digit which is shown on the char to be inserted
    const QString digit
        = shortcut->key().toString(QKeySequence::PortableText).right(1);
    insertMatchingEmoji(digit.toInt()-1);
}

int EmojisDialog::getFocusedButtonIndex() const
{
    const QWidget* focused = focusWidget();
    for(size_t i=0; i<matchingButtons.size(); i++) {
        if(matchingButtons[i] == focused) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void EmojisDialog::focusButton(int index)
{
    if(matchingButtons.empty()) {
        return;
    }

    // moving beyond the first/last character stops at it
    if(index < 0) {
        index = 0;
    }
    if(index >= static_cast<int>(matchingButtons.size())) {
        index = static_cast<int>(matchingButtons.size()) - 1;
    }

    QPushButton* button = matchingButtons[static_cast<size_t>(index)];
    button->setFocus();
    scrollArea->ensureWidgetVisible(button);
}

void EmojisDialog::keyPressEvent(QKeyEvent* event)
{
    // characters are navigated as a single list - Up/Down move by a grid row
    const int focused = getFocusedButtonIndex();
    switch(event->key()) {
    case Qt::Key_Down:
        focusButton(focused<0 ? 0 : navigator.getItemBelow(focused));
        event->accept();
        return;
    case Qt::Key_Up:
        if(focused >= 0) {
            const int above = navigator.getItemAbove(focused);
            if(above == GridNavigator::NO_ITEM) {
                // leaving the first row gets the focus back to the filter
                filterEdit->setFocus();
            } else {
                focusButton(above);
            }
            event->accept();
            return;
        }
        break;
    case Qt::Key_Left:
        if(focused >= 0) {
            focusButton(focused-1);
            event->accept();
            return;
        }
        break;
    case Qt::Key_Right:
        if(focused >= 0) {
            focusButton(focused+1);
            event->accept();
            return;
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if(focused >= 0) {
            // insert the chosen character
            insertEmoji(matchingButtons[static_cast<size_t>(focused)]);
        } else if(closeButton->hasFocus()) {
            // Close button is not the default one - Enter must still close
            close();
        } else if(!matchingButtons.empty()) {
            // insert the first character matching the filter
            insertEmoji(matchingButtons[0]);
        }
        event->accept();
        return;
    default:
        break;
    }

    QDialog::keyPressEvent(event);
}

void EmojisDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    // dialog always opens w/ the whole catalog and the keyboard in the filter
    filterEdit->clear();
    filterEdit->setFocus();
    scrollArea->verticalScrollBar()->setValue(0);
}

void EmojisDialog::slotCharacterButtonClicked()
{
    insertEmoji(qobject_cast<QPushButton*>(sender()));
}

} // namespace m8r

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

namespace m8r {

EmojisDialog::EmojisDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Emojis"));

    QVBoxLayout* mainLayout = new QVBoxLayout{this};

    QLabel* introLabel = new QLabel{
        tr("Click a character to insert it to the currently edited "
           "name or description:"),
        this};
    introLabel->setWordWrap(true);
    mainLayout->addWidget(introLabel);

    QWidget* sectionsWidget = new QWidget{this};
    QVBoxLayout* sectionsLayout = new QVBoxLayout{sectionsWidget};

    QScrollArea* scrollArea = new QScrollArea{this};
    scrollArea->setWidget(sectionsWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // IMPROVE load emojis from the main MF configuration file
    addCharactersSection(
        sectionsLayout,
        tr("Emoji"),
        {
            "🎯", "🚀", "🌟", "🔧", "🐞", "🧪", "📚", "🔗", "⛑", "🚧", "👁️", "🗑",
            "❗", "❌", "✔", "📌", "✂️", "📎", "📄", "📋", "📝", "📅", "📈", "🖼️",
            "🔴", "🔵", "🟣", "🟢", "🔮", "♥", "💙", "💛", "💚", "🚫", "⚽", "⚙️",
            "🙂", "😃", "🥶", "🥰", "🐻", "🐸", "🤖", "💩", "👻", "🎉", "💣", "☠️",
            "💪", "👍", "🤞", "🤙", "👌", "🙏", "🤦", "🦑", "🐙", "👾", "🐉",
            "🛠", "🔧", "🔨", "💎", "🛡", "💥", "🔥", "🧬", "🧙‍", "🧠", "🔋", "⦀",
            "🛸", "📡", "🌊", "🎖", "🍔", "🥋", "💍", "🥔", "🎨", "🌻", "🌲",
            "📣", "📢", "🧲", "🏁", "🚩", "💯"
        },
        11);
    addCharactersSection(
        sectionsLayout,
        tr("Greek alphabet"),
        {
            "Α", "α", "Β", "β", "Γ", "γ", "Δ", "δ", "Ε", "ε",
            "Ζ", "ζ", "Η", "η", "Θ", "θ", "Ι", "ι", "Κ", "κ",
            "Λ", "λ", "Μ", "μ", "Ν", "ν", "Ξ", "ξ", "Ο", "ο",
            "Π", "π", "Ρ", "ρ", "Σ", "σ", "ς", "Τ", "τ", "Υ", "υ",
            "Φ", "φ", "Χ", "χ", "Ψ", "ψ", "Ω", "ω"
        },
        10);
    addCharactersSection(sectionsLayout, tr("Math and statistics"), {"x̄"}, 12);
    addCharactersSection(sectionsLayout, tr("Physics"), {"°"}, 12);

    sectionsLayout->addStretch();

    QLabel* linksLabel = new QLabel{
        tr(
            "More special unicode characters: "
            "<a href='https://unicode-table.com/en/'>Unicode Table</a>, "
            "<a href='https://emojipedia.org/'>Emojipedia</a>"),
        this};
    linksLabel->setOpenExternalLinks(true);
    linksLabel->setWordWrap(true);
    mainLayout->addWidget(linksLabel);

    QPushButton* closeButton = new QPushButton{tr("&Close"), this};
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout* buttonsLayout = new QHBoxLayout{};
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonsLayout);

    resize(500, 500);
}

EmojisDialog::~EmojisDialog()
{
}

void EmojisDialog::addCharactersSection(
    QBoxLayout* mainLayout,
    const QString& title,
    std::initializer_list<QString> characters,
    int columns)
{
    QLabel* titleLabel = new QLabel{QString{"<b>%1:</b>"}.arg(title), this};
    mainLayout->addWidget(titleLabel);

    QWidget* gridWidget = new QWidget{this};
    QGridLayout* gridLayout = new QGridLayout{gridWidget};
    gridLayout->setSpacing(2);

    int row{0}, col{0};
    for(const QString& c : characters) {
        QPushButton* button = new QPushButton{c, gridWidget};
        button->setFixedSize(36, 32);
        button->setToolTip(tr("Click to insert '%1'").arg(c));
        connect(
            button, SIGNAL(clicked()),
            this, SLOT(slotCharacterButtonClicked()));
        gridLayout->addWidget(button, row, col);

        if(++col >= columns) {
            col = 0;
            row++;
        }
    }

    mainLayout->addWidget(gridWidget);
}

void EmojisDialog::slotCharacterButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if(button) {
        emit emojiSelected(button->text());
    }
}

} // namespace m8r

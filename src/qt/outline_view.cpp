/*
 outline_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "outline_view.h"

namespace m8r {

OutlineView::OutlineView(QWidget *parent)
    : QWidget(parent)
{
    // KISS & ONCE: show the name only - details to be rendered by Outline header view
    nameLabel = new QPushButton{parent};
    nameLabel->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
    // make button to look like label
    nameLabel->setStyleSheet("border: 0px");
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(nameFont.pointSize()+nameFont.pointSize()/4);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    headerVerticalLayout.addWidget(nameLabel);

    // tree of note
    outlineTreeView = new OutlineTreeView(parent);
    headerVerticalLayout.addWidget(outlineTreeView);

    setLayout(&headerVerticalLayout);
}

void OutlineView::refreshHeader(const std::string& name)
{
    if(!name.empty()) {
        QFontMetrics metrics(nameLabel->font());
        // IMPROVE nameLabel has incorrect size before rendered for the first time - find a better solution than fixed width
        QString elidedText
            = metrics.elidedText(QString::fromStdString(name), Qt::ElideRight, nameLabel->width()<100?1920/2:nameLabel->width());
        nameLabel->setText(elidedText);
    } else {
        nameLabel->setText("");
    }
}

OutlineView::~OutlineView()
{
}

}

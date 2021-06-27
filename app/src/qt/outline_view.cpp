/*
 outline_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

using namespace std;

OutlineView::OutlineView(QWidget *parent)
    : QWidget(parent)
{
    // KISS & ONCE: show the name only - details to be rendered by Outline header view
    nameLabel = new OutlineNamePushButton{parent};
    nameLabel->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
    nameLabel->setToolTip(tr("Click this Notebook name to open its Markdown preview in the right panel"));
    // make button to look like label
    nameLabel->setStyleSheet("QPushButton{ border: 0px; }");
#ifdef NO_LONGER_NEEDED
    string css{};
    css += "QPushButton{ border: 0px; } QPushButton:hover{ background-color:";
    css += LookAndFeels::getInstance().getHighlightColor().toStdString();
    css += "; }";
    nameLabel->setStyleSheet(QString::fromStdString(css));
#endif
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(nameFont.pointSize()+nameFont.pointSize()/4);
    nameFont.setBold(true);
    // subjective - this is too much: nameFont.setUnderline(true);
    nameLabel->setFont(nameFont);
    headerVerticalLayout.addWidget(nameLabel);

    // tree of Ns
    outlineTreeView = new OutlineTreeView(parent);
    headerVerticalLayout.addWidget(outlineTreeView);

    setLayout(&headerVerticalLayout);
}

OutlineView::~OutlineView()
{
}

void OutlineView::refreshHeader(const std::string& name)
{
    if(!name.empty()) {
        QFontMetrics metrics(nameLabel->font());
        // IMPROVE nameLabel has incorrect size before rendered for the first time - find a better solution than fixed width
        QString elidedText
            = metrics.elidedText(
               QString::fromStdString(name),
               Qt::ElideRight,
               width()-20);
        nameLabel->setText(elidedText);
    } else {
        nameLabel->setText("");
    }
}

} // m8r namespace

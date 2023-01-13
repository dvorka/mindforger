/*
 urgency_combo_box.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "urgency_combo_box.h"

namespace m8r {

UrgencyComboBox::UrgencyComboBox(QWidget *parent)
    : QComboBox{parent}
{
    addItem("");
    QString s{};
    for(int i=1; i<=5; i++) {
        s.clear();
        for(int j=1; j<=5; j++) {
            // unicode U260x table: https://www.unicode.org/charts/PDF/U2600.pdf
            if(j<=i) {
                s += QChar(U_CODE_URGENCY_ON);
            } else {
                s += QChar(U_CODE_URGENCY_OFF);
            }
        }
        addItem(s, QVariant::fromValue<int>(i));
    }
}

UrgencyComboBox::~UrgencyComboBox() = default;

}

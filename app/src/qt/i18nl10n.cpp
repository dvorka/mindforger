/*
 i18n.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "i18nl10n.h"

namespace m8r {

using namespace std;

void l10n(QApplication& mindforgerApplication)
{
    // Qt to delete translator
    QTranslator* mfTranslator = new QTranslator();
    QString translatorTargetAudience{};
    if(Configuration::getInstance().isUiNerdTargetAudience()) {
        translatorTargetAudience.append("nerd_");
    }
    // loader does fallback: :/translations/mindforger_us_EN.qm > :/translations/mindforger_us.qm
    QString translationPath{":/translations/mindforger_"+translatorTargetAudience+QLocale::system().name()+".qm"};
    MF_DEBUG("Loading locale " << translationPath.toStdString() << endl);
    if(mfTranslator->load(translationPath)) {
        if(!mindforgerApplication.installTranslator(mfTranslator)) {
#ifdef MF_DEBUG_L10N
            cerr << "Error: unable to install translator " << translationPath.toStdString() << endl;
#endif
        }
    }
#ifdef MF_DEBUG_L10N
    else {
        cerr << "Error: unable to load translator " << translationPath.toStdString() << endl;
    }
#endif
}

} // m8r

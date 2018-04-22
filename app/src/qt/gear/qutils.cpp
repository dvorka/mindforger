/*
 qutils.cpp     MindForger thinking notebook

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
#include "qutils.h"

using namespace std;

namespace m8r {

void initRandomizer()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

bool stringMatchByKeywords(const QString& keywords, const QString& s, bool caseSensitive)
{
    if(keywords.size()) {
        QStringList ks = keywords.split(" ");
        if(ks.size()) {
            Qt::CaseSensitivity c = caseSensitive?Qt::CaseInsensitive:Qt::CaseSensitive;
            for(QString& k:ks) {
                if(!s.contains(k, c)) {
                    return false;
                }
            }
        }
    }

    // no keywords ~ nothing filters || all match
    return true;
}


void timetToQDate(const time_t t, QDate& qdate)
{
    tm dd = *localtime(&t);
    qdate.setDate(dd.tm_year, dd.tm_mon, dd.tm_mday);
}

void qdateToTm(const QDate& qdate, struct tm& t)
{
    t.tm_year = qdate.year();
    t.tm_mon = qdate.month();
    t.tm_mday = qdate.day();
}

} // m8r namespace

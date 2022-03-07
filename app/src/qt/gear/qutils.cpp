/*
 qutils.cpp     MindForger thinking notebook

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
#include "qutils.h"

using namespace std;

namespace m8r {

void initRandomizer()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    qsrand(static_cast<uint>(QTime(0,0,0).secsTo(QTime::currentTime())));
#endif
// else new versions of Qt have QRandomGenerator already seeded
}

QString stringFormatIntAsUs(int i)
{
    return QString::fromStdString(stringIntFormat(std::to_string(i)));
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
    // see struct_tm.h::tm documentation
    qdate.setDate(
        dd.tm_year + 1900,
        dd.tm_mon + 1,
        dd.tm_mday
    );
}

void qdateToTm(const QDate& qdate, struct tm& t)
{
    // see struct_tm.h::tm documentation
    t.tm_year = qdate.year() - 1900;
    t.tm_mon = qdate.month() - 1;
    t.tm_mday = qdate.day();
}

} // m8r namespace

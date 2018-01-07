/*
 look_n_feel.cpp     MindForger thinking notebook

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
#include "look_n_feel.h"

namespace m8r {

LookAndFeels::LookAndFeels(void)
{
    themeNames << "yin" << "yang" << "black";
}

void LookAndFeels::init(QApplication* mindforgerApplication)
{
    this->mindforgerApplication = mindforgerApplication;
}

bool LookAndFeels::isThemeNameValid(const QString& themeName) const
{
    if(!themeName.isEmpty() && themeNames.contains(themeName)) {
        return true;
    } else {
        return false;
    }
}

void LookAndFeels::setTheme(const QString& themeName)
{
    if("black" == themeName.toStdString()) {
        setBlackTheme();
    } else if("yang" == themeName.toStdString()) {
        setYangTheme();
    } else if("yin" == themeName.toStdString()) {
        setYinTheme();
    }
}

void LookAndFeels::setBlackTheme(void)
{
    textColor = QString("#FFF");
    cliTextColor = QColor(0x99,0xb1,0xff);

    /* The valid keys can be retrieved using the keys() function. Typically they include
     * "windows" and "fusion". Depending on the platform, "windowsxp", "windowsvista" and
     * "macintosh" may be available. Note that keys are case insensitive
     */
    mindforgerApplication->setStyle(QStyleFactory::create("fusion"));

    // Terminal green: QColor(0, 140, 0)
    // Terminal blue : QColor(42, 130, 218)
    palette.setColor(QPalette::Foreground, Qt::red);
    palette.setColor(QPalette::Shadow, Qt::magenta);
    palette.setColor(QPalette::BrightText, Qt::green);
    palette.setColor(QPalette::Light, Qt::blue);
    palette.setColor(QPalette::Midlight, Qt::yellow);
    palette.setColor(QPalette::Mid, Qt::darkYellow);
    palette.setColor(QPalette::Dark, Qt::darkRed);

    palette.setColor(QPalette::AlternateBase, Qt::black);
    palette.setColor(QPalette::Base, QColor(20,20,20));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Button, QColor(10,10,10));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);

    palette.setColor(QPalette::Link, QColor(42,130,218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    mindforgerApplication->setPalette(palette);

    // IMPROVE tooltips are set in two ways
    mindforgerApplication->setStyleSheet("QToolTip { color: #ffffff; background-color: #000000; border: 1px solid white; }");
}

/*
 * Built-in yin (dark) theme definition.
 */
void LookAndFeels::setYinTheme(void)
{
    textColor = QString("#FFF");
    cliTextColor = Qt::green;

    mindforgerApplication->setStyle(QStyleFactory::create("fusion"));
    mindforgerApplication->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    // Terminal green: QColor(0, 140, 0)
    // Terminal blue : QColor(42, 130, 218)
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25,25,25));
    palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53,53,53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Link, QColor(0, 140, 0));
    palette.setColor(QPalette::Highlight, QColor(0, 140, 0));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    mindforgerApplication->setPalette(palette);

    // IMPROVE tooltips via palette does NOT work > CSS is used instead
    mindforgerApplication->setStyleSheet("QToolTip { color: #ffffff; background-color: #008c00; border: 1px solid white; }");
    //palette.setColor(QPalette::ToolTipBase, Qt::blue);
    //palette.setColor(QPalette::ToolTipText, Qt::red);
}

/*
 * Built-in yang (light) theme definition.
 */
void LookAndFeels::setYangTheme(void)
{
    textColor = QString("#000");
    cliTextColor = Qt::black;

    mindforgerApplication->setStyle(QStyleFactory::create("fusion"));

    // Ubuntu orange:
    //   100% #E95420 QColor(221, 72, 20)
    //    90% #EB6536 QColor(235, 101, 54)
    //    80% #ED764D QColor(237, 118, 77)
    palette.setColor(QPalette::Link, QColor(237, 118, 77));
    palette.setColor(QPalette::Highlight, QColor(237, 118, 77));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    mindforgerApplication->setStyleSheet("QToolTip { color: #ffffff; background-color: #ED764D; border: 1px solid white; }");

    mindforgerApplication->setPalette(palette);
}

}

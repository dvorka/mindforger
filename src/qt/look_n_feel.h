/*
 look_n_feel.h     MindForger thinking notebook

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
#ifndef M8RUI_LOOK_N_FEEL_H
#define M8RUI_LOOK_N_FEEL_H

#include <QtWidgets>
#include <QSet>

namespace m8r {

class LookAndFeels
{
private:
    /**
     * @brief Active palette.
     */
    QPalette palette;
    /**
     * @brief Active stylesheet.
     */
    char* stylesheet;

    QSet<QString> themeNames;

    int fontPointSize;

public:
    static LookAndFeels& getInstance(void)
    {
        static LookAndFeels SINGLETON;
        return SINGLETON;
    }

private:
    QApplication* mindforgerApplication;
    QString textColor;
    QColor cliTextColor;

    explicit LookAndFeels(void);

public:
    LookAndFeels(const LookAndFeels&) = delete;
    LookAndFeels(const LookAndFeels&&) = delete;
    LookAndFeels &operator=(const LookAndFeels&) = delete;
    LookAndFeels &operator=(const LookAndFeels&&) = delete;

    /* Yin (dark) theme is supposed to be terminal (dark&green)
     * "hacker" style view suitable in the night and early
     * mornings when your eyes prefer less light.
     */
    void setYinTheme(void);

    /* Yang (white) theme aims to be distraction-free Kindle style
     * theme suitable for comfortable knowledge reading and
     * navigation.
     */
    void setYangTheme(void);

    /* Extra (custom) theme(s)...
     */
    void setBlackTheme(void);

    void init(QApplication* mindforgerApplication);
    bool isThemeNameValid(const QString&) const;
    void setFontPointSize(int fontPointSize) { this->fontPointSize = fontPointSize; }
    int getFontPointSize(void) const { return fontPointSize; }
    void setTheme(const QString&);

    const QString& getTextColor(void) const { return textColor; }
    const QColor& getCliTextColor(void) const { return cliTextColor; }
};

}

#endif // M8RUI_LOOK_N_FEEL_H

/*
 look_n_feel.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "../../lib/src/representations/html/html_outline_representation.h"

namespace m8r {

/**
 * @brief L&F singleton class.
 *
 * The reason to make this class singleton is that it's used through frontend
 * components and widgets. Therefore passing of L&F instance to each and every
 * instance of the GUI object tree would be inefficient.
 */
class LookAndFeels : public HtmlColorsRepresentation
{
public:
    static LookAndFeels& getInstance()
    {
        static LookAndFeels SINGLETON{};
        return SINGLETON;
    }
private:
    explicit LookAndFeels();
    virtual ~LookAndFeels();

private:
    /**
     * @brief Active palette.
     */
    QPalette palette;

    QSet<QString> themeNames;

    int fontPointSize;

private:
    Configuration& config;
    QApplication* mindforgerApplication;

    // menu
    QString menuStylesheet;

    // viewer
    QString textColor;
    QString backgroundColor;
    QString highlightColor;

    // editor
    QString editorBackgroundColor;
    QString editorLineNumbersForegroundColor;
    QString editorLineNumbersBackgroundColor;

    QColor editorBold;
    QColor editorBolder;
    QColor editorItalic;
    QColor editorItalicer;
    QColor editorStrikethrough;
    QColor editorLink;
    QColor editorCodeblock;
    QColor editorList;
    QColor editorTaskDone;
    QColor editorTaskWip;
    QColor editorTaskTodo;
    QColor editorHtmlTag;
    QColor editorHtmlEntity;
    QColor editorHtmlAttrName;
    QColor editorHtmlAttrValue;
    QColor editorHtmlComment;
    QColor editorError;

    // HTML
    QString htmlTextColor;
    QString htmlBackgroundColor;

    // CLI
    QColor cliTextColor;

public:
    LookAndFeels(const LookAndFeels&) = delete;
    LookAndFeels(const LookAndFeels&&) = delete;
    LookAndFeels &operator=(const LookAndFeels&) = delete;
    LookAndFeels &operator=(const LookAndFeels&&) = delete;

    /* Dark theme is supposed to be terminal (dark&green)
     * "hacker" style view suitable in the night and early
     * mornings when your eyes prefer less light.
     */
    void setDarkTheme();

    /* Light theme aims to be distraction-free Kindle style
     * theme suitable for comfortable knowledge reading and
     * navigation.
     */
    void setLightTheme(bool fixedFont=false);

    /* Extra (custom) theme(s)...
     */
    void setBlackTheme(bool fixedFont=false);

    /* Set operating system native theme.
     */
    void setNativeTheme(bool fixedFont=false);

    void init(QApplication* mindforgerApplication);
    bool isThemeNative() const;
    bool isThemeNameValid(const QString&) const;
    void setFontPointSize(int fontPointSize) { this->fontPointSize = fontPointSize; }
    int getFontPointSize() const { return fontPointSize; }
    void setTheme(const QString&);

    const QString& getMenuStylesheet() const { return menuStylesheet; }

    const QString& getBackgroundColor() const { return backgroundColor; }
    const QString& getTextColor() const { return textColor; }
    const QString& getEditorBackgroundColor() const { return editorBackgroundColor; }
    const QString& getHighlightColor() const { return highlightColor; }

    const QString& getEditorLineNumbersForegroundColor() const { return editorLineNumbersForegroundColor; }
    const QString& getEditorLineNumbersBackgroundColor() const { return editorLineNumbersBackgroundColor; }

    QColor& getEditorBold() { return editorBold; }
    QColor& getEditorBolder() { return editorBolder; }
    QColor& getEditorItalic() { return editorItalic; }
    QColor& getEditorItalicer() { return editorItalicer; }
    QColor& getEditorStrikethrough() { return editorStrikethrough; }
    QColor& getEditorLink() { return editorLink; }
    QColor& getEditorList() { return editorList; }
    QColor& getEditorTaskDone() { return editorTaskDone; }
    QColor& getEditorTaskWip() { return editorTaskWip; }
    QColor& getEditorTaskTodo() { return editorTaskTodo; }
    QColor& getEditorCodeblock() { return editorCodeblock; }
    QColor& getEditorHtmlTag() { return editorHtmlTag; }
    QColor& getEditorHtmlEntity() { return editorHtmlEntity; }
    QColor& getEditorHtmlAttrName() { return editorHtmlAttrName; }
    QColor& getEditorHtmlAttrValue() { return editorHtmlAttrValue; }
    QColor& getEditorHtmlComment() { return editorHtmlComment; }
    QColor& getEditorError() { return editorError; }

    // HTML
    virtual std::string& getHtmlTextColor();
    virtual std::string& getHtmlBackgroundColor();

    // CLI
    const QColor& getCliTextColor() const { return cliTextColor; }

    // bean
    QApplication* getQApplication() const { return mindforgerApplication; }
};

}

#endif // M8RUI_LOOK_N_FEEL_H

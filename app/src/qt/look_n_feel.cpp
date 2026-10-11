/*
 look_n_feel.cpp     MindForger thinking notebook

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
#include "look_n_feel.h"

namespace m8r {

using namespace std;

LookAndFeels::LookAndFeels()
    : config(Configuration::getInstance())
{
    themeNames
        << UI_THEME_DARK
        << UI_THEME_LIGHT
#ifndef __APPLE__
       << UI_THEME_LIGHT_WITH_FIXED_FONT
#endif
        << UI_THEME_BLACK
#ifndef __APPLE__
       << UI_THEME_BLACK_WITH_FIXED_FONT
#endif
        << UI_THEME_NATIVE
#ifndef __APPLE__
        << UI_THEME_NATIVE_WITH_FIXED_FONT
#endif
        ;
}

LookAndFeels::~LookAndFeels()
{
}

void LookAndFeels::init(QApplication* mindforgerApplication)
{
    this->mindforgerApplication = mindforgerApplication;
}

QFont LookAndFeels::getFixedFont()
{
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
#ifdef _WIN32
    // system fixed font is Courier New on Windows, Consolas is the Windows
    // native monospace font (shipped since Vista) designed for ClearType
    font.setFamily("Consolas");
#endif
    return font;
}

#ifdef _WIN32
/*
 * Menu icons are pale gray (designed for dark menus) which makes them hard
 * to read on light Windows menus - especially when a checkable item is checked
 * and its icon is painted on the (light blue) checked item background.
 */
void LookAndFeels::polishMenuIcons(QMenuBar* menuBar)
{
    if(mindforgerApplication->palette().color(QPalette::Window).lightness() < 128) {
        // dark menu background - icons are readable
        return;
    }

    QSet<QAction*> actions{};
    for(QMenu* menu: menuBar->findChildren<QMenu*>()) {
        actions.insert(menu->menuAction());
        for(QAction* action: menu->actions()) {
            actions.insert(action);
        }
    }
    for(QAction* action: actions) {
        if(!action->icon().isNull()) {
            action->setIcon(darkenPaleIcon(action->icon()));
        }
    }
}

/*
 * Toolbar icons are pale gray too - on light toolbars they look as if they
 * were disabled.
 */
void LookAndFeels::polishToolBarIcons(QToolBar* toolBar)
{
    if(mindforgerApplication->palette().color(QPalette::Window).lightness() < 128) {
        // dark toolbar background - icons are readable
        return;
    }

    for(QAction* action: toolBar->actions()) {
        if(!action->icon().isNull()) {
            action->setIcon(darkenPaleIcon(action->icon()));
        }
    }
}

QIcon LookAndFeels::darkenPaleIcon(const QIcon& icon)
{
    QIcon darkIcon{};
    for(int size: {16, 20, 24, 32, 48, 64}) {
        QImage image = icon.pixmap(QSize{size, size}).toImage()
            .convertToFormat(QImage::Format_ARGB32);
        for(int y=0; y<image.height(); y++) {
            for(int x=0; x<image.width(); x++) {
                QColor c = image.pixelColor(x, y);
                // only light grays are darkened - colorful accents are kept
                if(c.alpha() && c.hsvSaturation() < 40 && c.value() > 100) {
                    c.setHsv(c.hsvHue(), c.hsvSaturation(), c.value()*45/100, c.alpha());
                    image.setPixelColor(x, y, c);
                }
            }
        }
        image.setDevicePixelRatio(1.0);
        // disabled mode pixmaps are generated (faded) from normal ones by Qt
        darkIcon.addPixmap(QPixmap::fromImage(image));
    }
    return darkIcon;
}
#endif

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
    MF_DEBUG("Setting MindForger Qt theme: " << themeName.toStdString() << endl);

    activeThemeName = themeName;

    if(UI_THEME_LIGHT == themeName.toStdString()) {
        setLightTheme(false);
    } else if(UI_THEME_LIGHT_WITH_FIXED_FONT == themeName.toStdString()) {
        setLightTheme(true);
    } else if(UI_THEME_DARK == themeName.toStdString()) {
        setDarkTheme();
    } else if(UI_THEME_BLACK == themeName.toStdString()) {
        setBlackTheme(false);
    } else if(UI_THEME_BLACK_WITH_FIXED_FONT == themeName.toStdString()) {
        setBlackTheme(true);
    } else if(UI_THEME_NATIVE_WITH_FIXED_FONT == themeName.toStdString()) {
        setNativeTheme(true);
    } else if(UI_THEME_NATIVE == themeName.toStdString()) {
        setNativeTheme(false);
    }
}

/*
 * Built-in dark theme definition.
 */
void LookAndFeels::setDarkTheme()
{
    textColor = QString("#FFF");
    backgroundColor =  QString("#353535");
    highlightColor = QString("#008C00"); // RGB: 0, 140, 0

    editorBackgroundColor = backgroundColor;
    editorLineNumbersForegroundColor = QString("#777777");
    editorLineNumbersBackgroundColor = backgroundColor;

    editorBold.setRgb(0xFF,0xFF,0x00);
    editorBolder.setRgb(0xFF,0xFF,0x00);
    editorItalic.setRgb(0x00,0xAA,0x00);
    editorItalicer.setRgb(0x00,0xAA,0x00);
    editorStrikethrough.setRgb(0x88,0x88,0x88);
    editorLink.setRgb(0x00,0xFF,0xFF);
    editorList.setRgb(0x00,0x99,0x00);
    editorTaskDone.setRgb(0x00,0x99,0x00);
    editorTaskWip.setRgb(0xFF,0xA5,0x00);
    editorTaskTodo.setRgb(0x99,0x00,0x00);
    editorCodeblock.setRgb(0x99,0x99,0x99);
    editorHtmlTag.setRgb(0xAA,0x00,0xAA);
    editorHtmlEntity.setRgb(0xAA,0x00,0xAA);
    editorHtmlAttrName.setRgb(0xFF,0x00,0xFF);
    editorHtmlAttrValue.setRgb(0x88,0x88,0x88);
    editorHtmlComment.setRgb(0x66,0x66,0x66);
    editorError.setRgb(0xFF,0x00,0x00);

    cliTextColor = Qt::green;

    mindforgerApplication->setStyle(QStyleFactory::create("fusion"));
    mindforgerApplication->setFont(getFixedFont());

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
    mindforgerApplication->setStyleSheet(
        "QToolTip { color: #ffffff; background-color: #008c00; border: 1px solid white; }"
    );

    menuStylesheet = QString{
        "QMenu::separator { background: #444; height: 1px; margin-left: 10px; margin-right: 10px; }"
        "QMenuBar::item:disabled { color: #555; }"
        "QMenu::item:disabled { color: #555; background: "}+backgroundColor+QString{"; }"};
}

/*
 * Built-in light theme definition.
 */
void LookAndFeels::setLightTheme(bool fixedFont)
{
    textColor = QString("#000");
    backgroundColor = QString("#FFF");
    highlightColor = QString("#ED764D"); // RGB: 237, 118, 77

    editorBackgroundColor = QString("#FFF");
    editorLineNumbersForegroundColor = QString("#BBBBBB");
    editorLineNumbersBackgroundColor = QString("#EEEEEE");

    // magenta 0xBB,0x00,0xBB
    // cyan 0x00,0x88,0x88
    // blue 0x00,0x00,0xFF
    // green 0x00,0x55,0x00

    editorBold.setRgb(0xFF,0x69,0x00);
    editorBolder.setRgb(0xFF,0x69,0x00);
    editorItalic.setRgb(0x55,0x00,0x55);
    editorItalicer.setRgb(0x55,0x00,0x55);
    editorStrikethrough.setRgb(0xAA,0xAA,0xAA);
    editorLink.setRgb(0x00,0x00,0xFF);
    editorList.setRgb(0x00,0x99,0x00);
    editorTaskDone.setRgb(0x00,0x99,0x00);
    editorTaskWip.setRgb(0xFF,0xA5,0x00);
    editorTaskTodo.setRgb(0x99,0x00,0x00);
    editorCodeblock.setRgb(0x00,0x88,0x88);
    editorHtmlTag.setRgb(0x00,0x00,0xFF);
    editorHtmlEntity.setRgb(0x00,0x00,0xFF);
    editorHtmlAttrName.setRgb(0x00,0x00,0xFF);
    editorHtmlAttrValue.setRgb(0x88,0x88,0x88);
    editorHtmlComment.setRgb(0xAA,0xAA,0xAA);
    editorError.setRgb(0xFF,0x00,0x00);

    cliTextColor = Qt::black;

#ifndef __APPLE__
    if(fixedFont) {
        mindforgerApplication->setFont(getFixedFont());
    }
#else
    Q_UNUSED(fixedFont);
#endif
    mindforgerApplication->setStyle(QStyleFactory::create("fusion"));

    // Ubuntu orange:
    //   100% #E95420 QColor(221, 72, 20)
    //    90% #EB6536 QColor(235, 101, 54)
    //    80% #ED764D QColor(237, 118, 77)
    palette.setColor(QPalette::Link, QColor(237, 118, 77));
    palette.setColor(QPalette::Highlight, QColor(237, 118, 77));
    palette.setColor(QPalette::HighlightedText, Qt::white);
#ifdef _WIN32
    setWindowsDisabledTextColors(palette);
#endif

    mindforgerApplication->setPalette(palette);

    mindforgerApplication->setStyleSheet("QToolTip { color: #ffffff; background-color: #ED764D; border: 1px solid white; }");

    menuStylesheet = QString("QMenu::separator { background: #ccc; height: 1px; margin-left: 10px; margin-right: 10px; }");
}

void LookAndFeels::setBlackTheme(bool fixedFont)
{
    textColor = QString("#FFF");
    backgroundColor = QString("#000");
    highlightColor = QString("#2A82DA"); // RGB: 42, 130, 218

    editorBackgroundColor = QString("#000");
    editorLineNumbersForegroundColor = QString("#555555");
    editorLineNumbersBackgroundColor = QString("#353535");

    editorBold.setRgb(0xFF,0xFF,0x00);
    editorBolder.setRgb(0xFF,0xFF,0x00);
    editorItalic.setRgb(0x00,0xAA,0x00);
    editorItalicer.setRgb(0x00,0xAA,0x00);
    editorStrikethrough.setRgb(0x88,0x88,0x88);
    editorLink.setRgb(0x00,0xFF,0xFF);
    editorList.setRgb(0x00,0x99,0x00);
    editorTaskDone.setRgb(0x00,0x99,0x00);
    editorTaskWip.setRgb(0xFF,0xA5,0x00);
    editorTaskTodo.setRgb(0x99,0x00,0x00);
    editorCodeblock.setRgb(0x99,0x99,0x99);
    editorHtmlTag.setRgb(0xAA,0x00,0xAA);
    editorHtmlEntity.setRgb(0xAA,0x00,0xAA);
    editorHtmlAttrName.setRgb(0xFF,0x00,0xFF);
    editorHtmlAttrValue.setRgb(0x88,0x88,0x88);
    editorHtmlComment.setRgb(0x66,0x66,0x66);
    editorError.setRgb(0xFF,0x00,0x00);

    cliTextColor = QColor(0x99,0xb1,0xff);

#ifndef __APPLE__
    if(fixedFont) {
        mindforgerApplication->setFont(getFixedFont());
    }
#else
    Q_UNUSED(fixedFont);
#endif

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

    menuStylesheet = QString{
            "QMenu::separator { background: #444; height: 1px; margin-left: 10px; margin-right: 10px; }"
            "QMenuBar::item:disabled { color: #555; }"
            "QMenu::item:disabled { color: #555; background: "
        }
        + backgroundColor
        + QString{"; }"};
}

void LookAndFeels::setNativeTheme(bool fixedFont)
{
#if defined(__APPLE__)
    Q_UNUSED(fixedFont);
    #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        // new(er) Qt versions detect light/dark themes, however, it does NOT set style correctly in case of dark theme:
        // - Big Sur 11.6 + Qt 5.15.2
        //   - bug: wrong QtTextEditor font - black, but should be white
        //   - bug: CLI font - black, but should be green - root cause: MF changes palette to set green text > skip @ macOS

        // system theme detection (quess) make sense ONLY in case of NATIVE theme (window color driven by OS)
        MF_DEBUG("  OS theme (light/dark ~ #ececec/#323232): " << QPalette().color(QPalette::ColorRole::Window).name().toStdString() << endl);
        if(QPalette().color(QPalette::ColorRole::Window).name().toStdString() == "#323232") {
            MF_DEBUG("    macOS dark mode detected - PATCHING MindForger editor pallette" << endl);

            // IMPROVE editor active line background fill (none @ macOS): NoteEditorView::highlightCurrentLine()

            editorBold.setRgb(0xFF,0xFF,0x00);
            editorBolder.setRgb(0xFF,0xFF,0x00);
            editorItalic.setRgb(0x00,0xAA,0x00);
            editorItalicer.setRgb(0x00,0xAA,0x00);
            editorStrikethrough.setRgb(0x88,0x88,0x88);
            editorLink.setRgb(0x00,0xFF,0xFF);
            editorList.setRgb(0x00,0x99,0x00);
            editorTaskDone.setRgb(0x00,0x99,0x00);
            editorTaskWip.setRgb(0xFF,0xA5,0x00);
            editorTaskTodo.setRgb(0x99,0x00,0x00);
            editorCodeblock.setRgb(0x99,0x99,0x99);
            editorHtmlTag.setRgb(0xAA,0x00,0xAA);
            editorHtmlEntity.setRgb(0xAA,0x00,0xAA);
            editorHtmlAttrName.setRgb(0xFF,0x00,0xFF);
            editorHtmlAttrValue.setRgb(0x88,0x88,0x88);
            editorHtmlComment.setRgb(0x66,0x66,0x66);
            editorError.setRgb(0xFF,0x00,0x00);
        }

    #endif
#else
    if(fixedFont) {
        mindforgerApplication->setFont(getFixedFont());
    }
#endif

#ifdef _WIN32
    setWindowsNativeColors();
#endif
}

#ifdef _WIN32
/*
 * Windows GrayText (#6D6D6D) used for disabled menu (bar) items is hard
 * to distinguish from enabled (black) items on light backgrounds.
 */
void LookAndFeels::setWindowsDisabledTextColors(QPalette& palette)
{
    const QColor disabledText{0xA0,0xA0,0xA0};
    palette.setColor(QPalette::Disabled, QPalette::Text, disabledText);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, disabledText);
}

/*
 * Windows native theme colors - application palette is kept as provided by
 * the OS (accent highlight for text selection like in Notepad), item views
 * and MindForger specific widgets get Windows Explorer like colors.
 */
void LookAndFeels::setWindowsNativeColors()
{
    palette = mindforgerApplication->palette();
    setWindowsDisabledTextColors(palette);
    mindforgerApplication->setPalette(palette);

    // Windows 10 Explorer selection: light blue w/ black text - menus and
    // QTreeView rows are painted this way by Windows style, however, Qt
    // paints QTableView (Notebooks, outline tree, ...) selection using
    // palette, therefore it is set explicitly for item views only
    const QColor explorerSelection{0xCC,0xE8,0xFF};
    QPalette itemViewPalette{palette};
    for(auto group: {QPalette::Active, QPalette::Inactive}) {
        itemViewPalette.setColor(group, QPalette::Highlight, explorerSelection);
        itemViewPalette.setColor(group, QPalette::HighlightedText, palette.color(QPalette::Text));
    }
    QApplication::setPalette(itemViewPalette, "QAbstractItemView");

    textColor = palette.color(QPalette::WindowText).name();
    backgroundColor = palette.color(QPalette::Window).name();
    highlightColor = explorerSelection.name();
    selectionColor = explorerSelection;
    selectionTextColor = palette.color(QPalette::Text);
    selectionBorderColor.setRgb(0x99,0xD1,0xFF);

    editorBackgroundColor = palette.color(QPalette::Base).name();
    editorLineNumbersForegroundColor = QString("#999999");
    editorLineNumbersBackgroundColor = backgroundColor;

    // Visual Studio light inspired syntax highlighting
    editorBold.setRgb(0xA3,0x15,0x15);
    editorBolder.setRgb(0xA3,0x15,0x15);
    editorItalic.setRgb(0x79,0x5E,0x26);
    editorItalicer.setRgb(0x79,0x5E,0x26);
    editorStrikethrough.setRgb(0xA0,0xA0,0xA0);
    editorLink.setRgb(0x00,0x66,0xCC);
    editorList.setRgb(0x00,0x78,0xD7);
    editorTaskDone.setRgb(0x10,0x7C,0x10);
    editorTaskWip.setRgb(0xCA,0x50,0x10);
    editorTaskTodo.setRgb(0xC4,0x2B,0x1C);
    editorCodeblock.setRgb(0x00,0x80,0x80);
    editorHtmlTag.setRgb(0x80,0x00,0x00);
    editorHtmlEntity.setRgb(0x80,0x00,0x00);
    editorHtmlAttrName.setRgb(0xE5,0x00,0x00);
    editorHtmlAttrValue.setRgb(0x00,0x00,0xFF);
    editorHtmlComment.setRgb(0x00,0x80,0x00);
    editorError.setRgb(0xE8,0x11,0x23);

    cliTextColor = palette.color(QPalette::Text);
}
#endif

bool LookAndFeels::isThemeNative() const
{
    // active theme might differ from configured one (CLI option)
    return UI_THEME_NATIVE == activeThemeName.toStdString()
        || UI_THEME_NATIVE_WITH_FIXED_FONT == activeThemeName.toStdString();
}

std::string& LookAndFeels::getHtmlTextColor()
{
    static string dark{"#000"}, light{"#FFF"};

    if(config.isUiHtmlTheme()) {
        if(config.getUiHtmlCssPath() == UI_HTML_THEME_CSS_LIGHT) {
            // light CSS theme
            return dark;
        } else {
            // dark CSS theme
            return light;
        }
    } else {
        if(config.getUiThemeName() == UI_THEME_LIGHT
           || config.getUiThemeName() == UI_THEME_LIGHT_WITH_FIXED_FONT
        ) {
            // light RAW theme
            return dark;
        } else {
            // dark RAW themes
            return light;
        }
    }
}

std::string& LookAndFeels::getHtmlBackgroundColor()
{
    static string dark{"#353535"}, light{"#FFF"};

    if(config.isUiHtmlTheme()) {
        if(config.getUiHtmlCssPath() == UI_HTML_THEME_CSS_LIGHT) {
            // light CSS theme
            return light;
        } else {
            // dark CSS theme
            return dark;
        }
    } else {
        if(config.getUiThemeName() == UI_THEME_LIGHT
           || config.getUiThemeName() == UI_THEME_LIGHT_WITH_FIXED_FONT
        ) {
            // light RAW theme
            return light;
        } else {
            // dark RAW themes
            return dark;
        }
    }
}


} // m8r namespace

/*
 status_bar_view.cpp     MindForger thinking notebook

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
#include "status_bar_view.h"

namespace m8r {

const char* StatusBarView::INDICATOR_OBJECT_NAME = "statusBarIndicator";

namespace {

struct IndicatorDescriptor {
    const char* letter;
    const char* enabledTooltip;
    const char* disabledTooltip;
};

// indexed by StatusBarIndicator
const IndicatorDescriptor INDICATOR_DESCRIPTORS[INDICATOR_COUNT] = {
    {
        "A",
        QT_TRANSLATE_NOOP("QObject", "Autolinking enabled"),
        QT_TRANSLATE_NOOP("QObject", "Autolinking disabled")
    },
    {
        "D",
        QT_TRANSLATE_NOOP("QObject", "Diagram support enabled"),
        QT_TRANSLATE_NOOP("QObject", "Diagram support disabled")
    },
    {
        "L",
        QT_TRANSLATE_NOOP("QObject", "Live spell check enabled"),
        QT_TRANSLATE_NOOP("QObject", "Live spell check disabled")
    },
    {
        "M",
        QT_TRANSLATE_NOOP("QObject", "Math support enabled"),
        QT_TRANSLATE_NOOP("QObject", "Math support disabled")
    },
    {
        "S",
        QT_TRANSLATE_NOOP("QObject", "Source code syntax highlighting enabled"),
        QT_TRANSLATE_NOOP("QObject", "Source code syntax highlighting disabled")
    },
    {
        "T",
        QT_TRANSLATE_NOOP("QObject", "Thinking enabled"),
        QT_TRANSLATE_NOOP("QObject", "Thinking disabled")
    },
    {
        "W",
        QT_TRANSLATE_NOOP("QObject", "Wingman available"),
        QT_TRANSLATE_NOOP("QObject", "Wingman not configured")
    }
};

} // anonymous namespace

StatusBarView::StatusBarView(QStatusBar* qStatusBar, LookAndFeels& lookAndFeel)
    : lookAndFeel(lookAndFeel)
{
    this->statusBar = qStatusBar;

    font.setFamily(statusBar->font().family());
    font.setPointSize(lookAndFeel.getFontPointSize());
    statusBar->setFont(font);

    // enabled diode looks like a selected item (dark ~ green, light ~ orange,
    // black ~ blue - all w/ white letter), disabled diode uses theme dimmed
    // gray; native theme does not define colors, therefore OS palette is used
    const QPalette& palette = statusBar->palette();
    indicatorEnabledStyle = createIndicatorStyle(
        palette.color(QPalette::Highlight).name(),
        palette.color(QPalette::HighlightedText).name());
    indicatorDisabledStyle = createIndicatorStyle(
        lookAndFeel.getEditorLineNumbersForegroundColor().isEmpty()
            ? palette.color(QPalette::Mid).name()
            : lookAndFeel.getEditorLineNumbersForegroundColor(),
        lookAndFeel.getTextColor().isEmpty()
            ? palette.color(QPalette::WindowText).name()
            : lookAndFeel.getTextColor());

    for(int i=0; i<INDICATOR_COUNT; i++) {
        indicators[i] = new QLabel{INDICATOR_DESCRIPTORS[i].letter, statusBar};
        indicators[i]->setObjectName(INDICATOR_OBJECT_NAME);
        indicators[i]->setAlignment(Qt::AlignCenter);
        // permanent widgets are shown on the right side and never hidden by messages
        statusBar->addPermanentWidget(indicators[i]);
        showIndicator(static_cast<StatusBarIndicator>(i), false);
    }
}

QString StatusBarView::createIndicatorStyle(
    const QString& backgroundColor, const QString& textColor)
{
    // style is scoped by object name, otherwise it would be inherited
    // by the tooltip label
    QString style{"QLabel#"};
    style += INDICATOR_OBJECT_NAME;
    style += " { color: ";
    style += textColor;
    style +=
        ";"
        " font-weight: bold;"
        " border-radius: 3px;"
        " padding: 0px 4px;"
        // bigger bottom margin lifts the diode up to compensate top spacing
        // which is added by QStatusBar layout - diode is vertically centered
        " margin: 0px 1px 7px 1px;"
        " background-color: ";
    style += backgroundColor;
    style += "; }";
    return style;
}

void StatusBarView::showIndicator(StatusBarIndicator indicator, bool enabled) const
{
    if(indicator<0 || indicator>=INDICATOR_COUNT) {
        return;
    }

    indicators[indicator]->setStyleSheet(
        enabled ? indicatorEnabledStyle : indicatorDisabledStyle);
    indicators[indicator]->setToolTip(
        QObject::tr(
            enabled
            ? INDICATOR_DESCRIPTORS[indicator].enabledTooltip
            : INDICATOR_DESCRIPTORS[indicator].disabledTooltip
        )
    );
}

void StatusBarView::showInfo(const QString& message) const
{
    // IMPROVE set style sheet - code below causes SIGSEGV (lookAndFeel nullptr)
    QString style;
    style += "color: ";
    style += lookAndFeel.getTextColor();
    style += ";";
    statusBar->setStyleSheet(style);

    statusBar->showMessage(message);
}

void StatusBarView::showWarning(const QString& message) const
{
    // IMPROVE set style sheet > ORANGE
    statusBar->showMessage(message);
}

void StatusBarView::showError(const QString& message) const
{
    // IMPROVE set style sheet > RED
    statusBar->showMessage(message);
}

StatusBarView::~StatusBarView()
{
}

} // namespace

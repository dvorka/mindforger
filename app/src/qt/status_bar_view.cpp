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

const char* StatusBarView::INDICATORS_PANEL_OBJECT_NAME = "statusBarIndicators";
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
    // size grip is hidden when the window is maximized, which would shift
    // the permanent widgets - it is disabled to keep the right margin fixed
    // (window can be still resized using its borders)
    statusBar->setSizeGripEnabled(false);

    // capsule colors are derived from the active theme: enabled segment looks
    // like a selected item (dark ~ green, light ~ orange, black ~ blue - all w/
    // white letter), disabled segment letter is faded; native theme does not
    // define colors, therefore OS palette is used
    const QPalette& palette = statusBar->palette();
    const QColor textColor = palette.color(QPalette::WindowText);
    const QColor windowColor = palette.color(QPalette::Window);
    const QColor borderColor{
        lookAndFeel.getEditorLineNumbersForegroundColor().isEmpty()
            ? palette.color(QPalette::Mid)
            : QColor{lookAndFeel.getEditorLineNumbersForegroundColor()}
    };
    const QColor disabledTextColor{
        // fade letter by blending it with the window background
        (textColor.red()*45 + windowColor.red()*55)/100,
        (textColor.green()*45 + windowColor.green()*55)/100,
        (textColor.blue()*45 + windowColor.blue()*55)/100
    };
    for(int i=0; i<INDICATOR_COUNT; i++) {
        indicatorStyles[i][false] = createIndicatorStyle(
            i, disabledTextColor, Qt::transparent, borderColor);
        indicatorStyles[i][true] = createIndicatorStyle(
            i,
            palette.color(QPalette::HighlightedText),
            palette.color(QPalette::Highlight),
            borderColor);
    }

    indicatorsPanel = new QFrame{statusBar};
    indicatorsPanel->setObjectName(INDICATORS_PANEL_OBJECT_NAME);
    QString panelStyle{"QFrame#"};
    panelStyle += INDICATORS_PANEL_OBJECT_NAME;
    panelStyle += " { border: 1px solid ";
    panelStyle += toCssColor(borderColor);
    panelStyle +=
        "; border-radius: 4px;"
        // bigger bottom margin lifts the capsule up to compensate top spacing
        // which is added by QStatusBar layout - capsule is vertically centered;
        // right margin is the style's layout margin, which is used by the main
        // window's central layout, so the capsule is aligned w/ the views above
        // (+3px compensate the frame of the views)
        " margin: 0px ";
    panelStyle += QString::number(
        statusBar->style()->pixelMetric(QStyle::PM_LayoutRightMargin) + 3);
    panelStyle += "px 7px 1px; }";
    indicatorsPanel->setStyleSheet(panelStyle);
    QHBoxLayout* panelLayout = new QHBoxLayout{indicatorsPanel};
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(0);

    for(int i=0; i<INDICATOR_COUNT; i++) {
        indicators[i] = new QLabel{INDICATOR_DESCRIPTORS[i].letter, indicatorsPanel};
        indicators[i]->setObjectName(INDICATOR_OBJECT_NAME);
        indicators[i]->setAlignment(Qt::AlignCenter);
        panelLayout->addWidget(indicators[i]);
        showIndicator(static_cast<StatusBarIndicator>(i), false);
    }
    // permanent widgets are shown on the right side and never hidden by messages
    statusBar->addPermanentWidget(indicatorsPanel);
}

QString StatusBarView::toCssColor(const QColor& color)
{
    return QString{"rgba(%1, %2, %3, %4)"}
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha());
}

QString StatusBarView::createIndicatorStyle(
    int indicator,
    const QColor& textColor,
    const QColor& backgroundColor,
    const QColor& separatorColor)
{
    // style is scoped by object name, otherwise it would be inherited
    // by the tooltip label
    QString style{"QLabel#"};
    style += INDICATOR_OBJECT_NAME;
    style += " { font-weight: bold; padding: 0px 5px; color: ";
    style += toCssColor(textColor);
    style += "; background-color: ";
    style += toCssColor(backgroundColor);
    style += ";";
    // segments are separated by a thin line
    if(indicator > 0) {
        style += " border-left: 1px solid ";
        style += toCssColor(separatorColor);
        style += ";";
    }
    // Qt does not clip children to the rounded panel border, therefore
    // outer segments round their outer corners themselves
    if(indicator == 0) {
        style += " border-top-left-radius: 3px; border-bottom-left-radius: 3px;";
    }
    if(indicator == INDICATOR_COUNT-1) {
        style += " border-top-right-radius: 3px; border-bottom-right-radius: 3px;";
    }
    style += " }";
    return style;
}

void StatusBarView::showIndicator(StatusBarIndicator indicator, bool enabled) const
{
    if(indicator<0 || indicator>=INDICATOR_COUNT) {
        return;
    }

    indicators[indicator]->setStyleSheet(indicatorStyles[indicator][enabled]);
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

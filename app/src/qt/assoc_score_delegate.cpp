/*
 assoc_score_delegate.cpp     MindForger thinking notebook

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
#include "assoc_score_delegate.h"

#include "look_n_feel.h"

namespace m8r {

AssocScoreDelegate::AssocScoreDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

AssocScoreDelegate::~AssocScoreDelegate()
{
}

void AssocScoreDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    QVariant scoreData = index.data(ROLE_SCORE);
    if(!scoreData.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    const float score = qBound(0.f, scoreData.toFloat(), 1.f);

    // paint cell background (selection, hover) w/o text
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = QString();
    QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    // colors: theme highlight (native theme has none > palette), selected row has highlight background
    const bool selected = opt.state & QStyle::State_Selected;
    QColor text = opt.palette.color(selected ? QPalette::HighlightedText : QPalette::Text);
    QColor fill{LookAndFeels::getInstance().getHighlightColor()};
    if(selected || !fill.isValid()) {
        fill = selected ? text : opt.palette.color(QPalette::Highlight);
    }
    QColor track{text};
    track.setAlphaF(0.15);
    QColor number{text};
    number.setAlphaF(0.7);

    // layout: [ bar ][ gap ][ number ]
    const int padding = 6;
    const int barHeight = 6;
    const QString label = QString::number(qRound(score*100.f));
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    const int labelWidth = opt.fontMetrics.horizontalAdvance(QStringLiteral("100"));
#else
    const int labelWidth = opt.fontMetrics.width(QStringLiteral("100"));
#endif
    const QRect cell = opt.rect.adjusted(padding, 0, -padding, 0);
    const int barWidth = qMax(0, cell.width() - labelWidth - padding);
    const QRectF bar{
        static_cast<qreal>(cell.left()),
        cell.top() + (cell.height() - barHeight) / 2.0,
        static_cast<qreal>(barWidth),
        static_cast<qreal>(barHeight)};
    const qreal radius = barHeight / 2.0;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    // track
    painter->setBrush(track);
    painter->drawRoundedRect(bar, radius, radius);
    // fill - at least a dot so that even the weakest association is visible
    if(barWidth > 0) {
        QRectF filled{bar};
        filled.setWidth(qMax(static_cast<qreal>(barHeight), bar.width()*score));
        painter->setBrush(fill);
        painter->drawRoundedRect(filled, radius, radius);
    }

    // rounded percentage
    painter->setPen(number);
    painter->drawText(
        QRect{cell.right() - labelWidth, cell.top(), labelWidth, cell.height()},
        Qt::AlignRight | Qt::AlignVCenter,
        label);

    painter->restore();
}

} // m8r namespace

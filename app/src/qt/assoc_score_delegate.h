/*
 assoc_score_delegate.h     MindForger thinking notebook

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
#ifndef M8RUI_ASSOC_SCORE_DELEGATE_H
#define M8RUI_ASSOC_SCORE_DELEGATE_H

#include <QtWidgets>

namespace m8r {

/**
 * @brief Association score delegate.
 *
 * Paints association score as a meter - a thin rounded bar in the theme
 * highlight color w/ the rounded percentage next to it.
 */
class AssocScoreDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    // item data role which holds the score <0,1> as float
    static constexpr int ROLE_SCORE = Qt::UserRole + 2;

    explicit AssocScoreDelegate(QObject* parent);
    AssocScoreDelegate(const AssocScoreDelegate&) = delete;
    AssocScoreDelegate(const AssocScoreDelegate&&) = delete;
    AssocScoreDelegate &operator=(const AssocScoreDelegate&) = delete;
    AssocScoreDelegate &operator=(const AssocScoreDelegate&&) = delete;
    ~AssocScoreDelegate();

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

}
#endif // M8RUI_ASSOC_SCORE_DELEGATE_H

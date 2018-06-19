/*
 ner_result_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_NER_RESULT_DIALOG_H
#define M8RUI_NER_RESULT_DIALOG_H

#include <QtWidgets>

#include "../../../../lib/src/mind/ai/nlp/ner_named_entity.h"

#include "../ner_leaderboard_model.h"
#include "../ner_leaderboard_view.h"

namespace m8r {

class NerResultDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* label;
    NerLeaderboardModel* leaderboardModel;
    NerLeaderboardView* leaderboardView;
    QPushButton* closeButton;
    QPushButton* findButton;

public:
    explicit NerResultDialog(QWidget* parent);
    NerResultDialog(const NerResultDialog&) = delete;
    NerResultDialog(const NerResultDialog&&) = delete;
    NerResultDialog &operator=(const NerResultDialog&) = delete;
    NerResultDialog &operator=(const NerResultDialog&&) = delete;
    ~NerResultDialog();

    QPushButton* getFindButton() const { return findButton; }

    void show(std::vector<NerNamedEntity>& entities);

signals:
    void searchFinished();

private slots:
    void handleChoice();
};

}
#endif // M8RUI_NER_RESULT_DIALOG_H

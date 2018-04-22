/*
 forget_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_TIME_SCOPE_DIALOG_H
#define M8RUI_TIME_SCOPE_DIALOG_H

#include <QtWidgets>

namespace m8r {

class TimeScopeDialog : public QDialog
{
    Q_OBJECT

private:
    QCheckBox* enableCheck;

    QSpinBox* yearSpin;
    QSpinBox* monthSpin;
    QSpinBox* daySpin;
    QSpinBox* hourSpin;
    QSpinBox* minuteSpin;

    QPushButton* setButton;
    QPushButton* closeButton;

public:
    explicit TimeScopeDialog(QWidget* parent);
    TimeScopeDialog(const TimeScopeDialog&) = delete;
    TimeScopeDialog(const TimeScopeDialog&&) = delete;
    TimeScopeDialog &operator=(const TimeScopeDialog&) = delete;
    TimeScopeDialog &operator=(const TimeScopeDialog&&) = delete;
    ~TimeScopeDialog();

    QPushButton* getSetButton() { return setButton; }
    int getYears() const { return yearSpin->value(); }
    int getMonths() const { return monthSpin->value(); }
    int getDays() const { return daySpin->value(); }
    int getHours() const { return hourSpin->value(); }
    int getMinutes() const { return minuteSpin->value(); }

    bool isThreasholdSet() const { return enableCheck->isChecked(); }
    void show(bool e, int y=0, int m=0, int d=0, int h=0, int mm=0) {
        enableCheck->setChecked(e);
        enableDisable(e);

        yearSpin->setValue(y);
        monthSpin->setValue(m);
        daySpin->setValue(d);
        hourSpin->setValue(h);
        minuteSpin->setValue(mm);
        QDialog::show();
    }

private slots:
    void enableDisable(bool enable);
};

}
#endif // M8RUI_TIME_SCOPE_DIALOG_H

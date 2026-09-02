/*
 add_llm_provider_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_ADD_LLM_PROVIDER_DIALOG_H
#define M8RUI_ADD_LLM_PROVIDER_DIALOG_H

#include <QtWidgets>

#include "../../lib/src/config/configuration.h"

namespace m8r {

class AddLlmProviderDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* questionLabel;
    QComboBox* providerTypeCombo;
    QPushButton* nextButton;
    QPushButton* cancelButton;
    
    WingmanLlmProviders selectedProviderType;

public:
    explicit AddLlmProviderDialog(QWidget* parent);
    AddLlmProviderDialog(const AddLlmProviderDialog&) = delete;
    AddLlmProviderDialog(const AddLlmProviderDialog&&) = delete;
    AddLlmProviderDialog& operator=(const AddLlmProviderDialog&) = delete;
    AddLlmProviderDialog& operator=(const AddLlmProviderDialog&&) = delete;
    ~AddLlmProviderDialog();

    WingmanLlmProviders getSelectedProviderType() const { return selectedProviderType; }
    
    void show();

private slots:
    void handleNext();
};

}
#endif // M8RUI_ADD_LLM_PROVIDER_DIALOG_H

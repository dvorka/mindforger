/*
 configuration_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_CONFIGURATION_DIALOG_H
#define M8RUI_CONFIGURATION_DIALOG_H

#include <QtWidgets>

#include "../../lib/src/config/configuration.h"

class QDialogButtonBox;
class QTabWidget;

namespace m8r {

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

    class MarkdownTab;
    class MindTab;
    class AppTab;

private:
    QTabWidget* tabWidget;
    AppTab* appTab;
    MarkdownTab* mdTab;
    MindTab* mindTab;

    QDialogButtonBox *buttonBox;

public:
    explicit ConfigurationDialog(QWidget* parent);
    ConfigurationDialog(const ConfigurationDialog&) = delete;
    ConfigurationDialog(const ConfigurationDialog&&) = delete;
    ConfigurationDialog &operator=(const ConfigurationDialog&) = delete;
    ConfigurationDialog &operator=(const ConfigurationDialog&&) = delete;
    ~ConfigurationDialog();

    AppTab* getAppTab() { return appTab; }

    void show();

private slots:
    void saveSlot();
signals:
    void saveConfigSignal();
};

/**
 * @brief Mind tab.
 */
class ConfigurationDialog::MindTab : public QWidget
{
    Q_OBJECT

private:
    Configuration& config;

    QLabel* saveReadsMetadataLabel;
    QCheckBox* saveReadsMetadataCheck;
    QLabel* distributorSleepIntervalLabel;
    QSpinBox*  distributorSleepIntervalSpin;

public:
    explicit MindTab(QWidget* parent);
    ~MindTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();
};

/*
 * App tab
 */

class ConfigurationDialog::AppTab : public QWidget
{
    Q_OBJECT

private:
    Configuration& config;

    QLabel* themeLabel;
    QComboBox* themeCombo;
    QLabel* htmlCssThemeLabel;
    QComboBox* htmlCssThemeCombo;

    QLabel* editorKeyBindingLabel;
    QComboBox* editorKeyBindingCombo;
    QLabel* srcCodeHighlightEditorLabel;
    QCheckBox* srcCodeHighlightEditorCheck;
    QLabel* tabWidthLabel;
    QComboBox* tabWidthCombo;

public:
    explicit AppTab(QWidget* parent);
    ~AppTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();
};


/**
 * @brief Markdown tab.
 */
class ConfigurationDialog::MarkdownTab : public QWidget
{
    Q_OBJECT

private:
    Configuration& config;

    QLabel* mathSupportLabel;
    QCheckBox* mathSupportCheck;
    QLabel* diagramSupportLabel;
    QComboBox* diagramSupportCombo;
    QLabel* srcCodeHighlightingSupportLabel;
    QCheckBox* srcCodeHighlightSupportCheck;

public:
    explicit MarkdownTab(QWidget* parent);
    ~MarkdownTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();
};

}
#endif // M8RUI_CONFIGURATION_DIALOG_H

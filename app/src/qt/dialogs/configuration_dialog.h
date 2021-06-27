/*
 configuration_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

    class AppTab;
    class ViewerTab;
    class EditorTab;
    class NavigatorTab;
    class MindTab;
    class EisenhowerTab;

private:
    QTabWidget* tabWidget;
    AppTab* appTab;
    ViewerTab* viewerTab;
    EditorTab* editorTab;
    NavigatorTab* navigatorTab;
    MindTab* mindTab;
    EisenhowerTab* eisenhowerTab;

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

    QLabel* startupLabel;
    QComboBox* startupCombo;

    QCheckBox* showToolbarCheck;
    QCheckBox* uiExpertModeCheck;
    QCheckBox* nerdMenuCheck;

public:
    explicit AppTab(QWidget* parent);
    ~AppTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();
};

/**
 * @brief Navigator tab.
 */
class ConfigurationDialog::NavigatorTab : public QWidget
{
    Q_OBJECT

private:
    Configuration& config;

    QLabel* maxNodesLabel;
    QSpinBox*  maxNodesSpin;

public:
    explicit NavigatorTab(QWidget* parent);
    ~NavigatorTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();
};


/**
 * @brief Viewer tab.
 */
class ConfigurationDialog::ViewerTab : public QWidget
{
    Q_OBJECT

private:
    Configuration& config;

    QLabel* htmlCssThemeLabel;
    QComboBox* htmlCssThemeCombo;
    QLabel* zoomLabel;
    QSpinBox* zoomSpin;
    QCheckBox* mathSupportCheck;
    QCheckBox* fullOPreviewCheck;
    QLabel* diagramSupportLabel;
    QComboBox* diagramSupportCombo;
    QCheckBox* srcCodeHighlightSupportCheck;

public:
    explicit ViewerTab(QWidget* parent);
    ~ViewerTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();
};

/**
 * @brief Editor tab.
 */
class ConfigurationDialog::EditorTab : public QWidget
{
    Q_OBJECT

private:
    Configuration& config;

    QLabel* editorKeyBindingLabel;
    QComboBox* editorKeyBindingCombo;
    QLabel* editorFontLabel;
    QPushButton* editorFontButton;
    QCheckBox* editorMdSyntaxHighlightCheck;
    QCheckBox* editorAutocompleteCheck;
    QCheckBox* editorAutosaveCheck;
    QLabel* editorTabWidthLabel;
    QComboBox* editorTabWidthCombo;
    // TODO QCheckBox* editorQuoteSectionsCheck;
    QCheckBox* editorTabsAsSpacesCheck;

    QFont editorFont;

public:
    explicit EditorTab(QWidget* parent);
    ~EditorTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();

private slots:
    void getFont();
};

/**
 * @brief Eisenhower matrix tab.
 */
class ConfigurationDialog::EisenhowerTab : public QWidget
{
    Q_OBJECT

    class QuadrantPanel;

private:
    Configuration& config;

    QuadrantPanel* doSometimePanel;
    QuadrantPanel* doPlanPanel;
    QuadrantPanel* doSoonPanel;
    QuadrantPanel* doFirstPanel;

public:
    explicit EisenhowerTab(QWidget* parent);
    ~EisenhowerTab();

    // there and back is handled by Dialog's access to this class & Config singleton
    void refresh();
    void save();

};

class ConfigurationDialog::EisenhowerTab::QuadrantPanel : public QGroupBox
{
    Q_OBJECT

private:

    QCheckBox* check;
    QLabel* label;
    QLineEdit* tag;
    QComboBox* filter;

public:
    explicit QuadrantPanel(const char* title, QWidget* parent);
    virtual ~QuadrantPanel() {};

private:
    QComboBox* createFilterCombo();
    void setCustomEnabled(const bool enable);
private slots:
    void handleCheck();
};

}
#endif // M8RUI_CONFIGURATION_DIALOG_H

/*
 configuration_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "configuration_dialog.h"

namespace m8r {

using namespace std;

ConfigurationDialog::ConfigurationDialog(QWidget* parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;

    appTab = new AppTab{this};

    tabWidget->addTab(appTab, tr("Application"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // signals
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, appTab, &ConfigurationDialog::AppTab::saveSlot);

    QVBoxLayout *mainLayout = new QVBoxLayout{this};
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Adapt"));
    resize(fontMetrics().averageCharWidth()*55, 0);
    setModal(true);
}

ConfigurationDialog::~ConfigurationDialog()
{
    if(appTab) delete appTab;
}

/*
 * App tab
 */

ConfigurationDialog::AppTab::AppTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    QGroupBox* settingsGroup = new QGroupBox{tr("Settings"), this};

    themeLabel = new QLabel(tr("UI theme (requires restart)")+":", this),
    themeCombo = new QComboBox{this};
    themeCombo->addItem(QString{UI_THEME_LIGHT});
    themeCombo->addItem(QString{UI_THEME_DARK});
    themeCombo->addItem(QString{UI_THEME_BLACK});

    htmlCssThemeLabel = new QLabel(tr("HTML CSS theme")+":", this);
    htmlCssThemeCombo = new QComboBox{this};
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_LIGHT});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_DARK});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_RAW});

    editorKeyBindingLabel = new QLabel(tr("Editor key binding")+":", this);
    editorKeyBindingCombo = new QComboBox{this};
    editorKeyBindingCombo->addItem("emacs");
    editorKeyBindingCombo->addItem("vim");
    editorKeyBindingCombo->addItem("windows");

    // IMPROVE horizontal panel w/ label & check same line
    showOutlineEditButtonLabel = new QLabel(tr("Show Notebook edit button")+":", this);
    showOutlineEditButtonCheck = new QCheckBox(this);

    // IMPROVE horizontal panel w/ label & check same line
    saveReadsMetadataLabel = new QLabel(tr("Save reads metadata")+":", this);
    saveReadsMetadataCheck = new QCheckBox(this);

    // assembly
    QVBoxLayout* settingsLayout = new QVBoxLayout{this};
    settingsLayout->addWidget(themeLabel);
    settingsLayout->addWidget(themeCombo);
    settingsLayout->addWidget(htmlCssThemeLabel);
    settingsLayout->addWidget(htmlCssThemeCombo);
    settingsLayout->addWidget(editorKeyBindingLabel);
    settingsLayout->addWidget(editorKeyBindingCombo);
    settingsLayout->addWidget(showOutlineEditButtonLabel);
    settingsLayout->addWidget(showOutlineEditButtonCheck);
    settingsLayout->addWidget(saveReadsMetadataLabel);
    settingsLayout->addWidget(saveReadsMetadataCheck);
    settingsGroup->setLayout(settingsLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(settingsGroup);
    setLayout(boxesLayout);
}

ConfigurationDialog::AppTab::~AppTab()
{
    delete themeLabel;
    delete themeCombo;
    delete htmlCssThemeLabel;
    delete htmlCssThemeCombo;
    delete editorKeyBindingLabel;
    delete editorKeyBindingCombo;
    delete showOutlineEditButtonLabel;
    delete showOutlineEditButtonCheck;
    delete saveReadsMetadataLabel;
    delete saveReadsMetadataCheck;
}

void ConfigurationDialog::AppTab::refresh()
{
    int i = themeCombo->findText(QString::fromStdString(config.getUiThemeName()));
    if(i>=0) {
        themeCombo->setCurrentIndex(i);
    }

    i = htmlCssThemeCombo->findText(QString::fromStdString(config.getUiHtmlCssPath()));
    if(i>=0) {
        htmlCssThemeCombo->setCurrentIndex(i);
    }

    i = editorKeyBindingCombo->findText(QString::fromStdString(config.getEditorKeyBindingAsString()));
    if(i>=0) {
        editorKeyBindingCombo->setCurrentIndex(i);
    }

    showOutlineEditButtonCheck->setChecked(config.isUiShowNotebookEditButton());
    saveReadsMetadataCheck->setChecked(config.isSaveReadsMetadata());
}

void ConfigurationDialog::AppTab::clean()
{
}

void ConfigurationDialog::AppTab::saveSlot()
{
    config.setUiThemeName(themeCombo->itemText(themeCombo->currentIndex()).toStdString());
    config.setUiHtmlCssPath(htmlCssThemeCombo->itemText(htmlCssThemeCombo->currentIndex()).toStdString());
    config.setEditorKeyBindingByString(editorKeyBindingCombo->itemText(editorKeyBindingCombo->currentIndex()).toStdString());
    config.setUiShowNotebookEditButton(showOutlineEditButtonCheck->isChecked());
    config.setSaveReadsMetadata(saveReadsMetadataCheck->isChecked());

    emit saveConfigSignal();
}

/*
 * Dialog
 */

void ConfigurationDialog::show()
{
    appTab->refresh();

    QDialog::show();
}


} // m8r namespace

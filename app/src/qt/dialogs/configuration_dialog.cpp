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
    mdTab = new MarkdownTab{this};
    mindTab = new MindTab{this};

    tabWidget->addTab(appTab, tr("Application"));
    tabWidget->addTab(mdTab, tr("Markdown"));
    tabWidget->addTab(mindTab, tr("Mind"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // signals
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &ConfigurationDialog::saveSlot);

    QVBoxLayout *mainLayout = new QVBoxLayout{this};
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Adapt"));
    resize(fontMetrics().averageCharWidth()*65, 0);
    setModal(true);
}

ConfigurationDialog::~ConfigurationDialog()
{
    // tabs are auto-destructured thaks to parent/child hierarchy
}

void ConfigurationDialog::show()
{
    appTab->refresh();
    mdTab->refresh();
    mindTab->refresh();

    QDialog::show();
}

void ConfigurationDialog::saveSlot()
{
    appTab->save();
    mdTab->save();
    mindTab->save();

    emit saveConfigSignal();
}

/*
 * App tab
 */

ConfigurationDialog::AppTab::AppTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    themeLabel = new QLabel(tr("UI theme (requires restart)")+":", this),
    themeCombo = new QComboBox{this};
    themeCombo->addItem(QString{UI_THEME_LIGHT});
    themeCombo->addItem(QString{UI_THEME_DARK});
    themeCombo->addItem(QString{UI_THEME_BLACK});

    htmlCssThemeLabel = new QLabel(tr("Viewer theme CSS")+":", this);
    htmlCssThemeCombo = new QComboBox{this};
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_LIGHT});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_DARK});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_RAW});

    editorKeyBindingLabel = new QLabel(tr("Editor key binding")+":", this);
    editorKeyBindingCombo = new QComboBox{this};
    editorKeyBindingCombo->addItem("emacs");
    editorKeyBindingCombo->addItem("vim");
    editorKeyBindingCombo->addItem("windows");

    srcCodeHighlightEditorLabel = new QLabel(tr("Markdown syntax highlighting")+":", this);
    srcCodeHighlightEditorCheck = new QCheckBox(this);

    tabWidthLabel = new QLabel(tr("TAB width")+":", this);
    tabWidthCombo = new QComboBox(this);
    tabWidthCombo->addItem(QString{"4"});
    tabWidthCombo->addItem(QString{"8"});

    // assembly
    QVBoxLayout* appearanceLayout = new QVBoxLayout{this};
    appearanceLayout->addWidget(themeLabel);
    appearanceLayout->addWidget(themeCombo);
    appearanceLayout->addWidget(htmlCssThemeLabel);
    appearanceLayout->addWidget(htmlCssThemeCombo);
    QGroupBox* appearanceGroup = new QGroupBox{tr("Appearance"), this};
    appearanceGroup->setLayout(appearanceLayout);

    QVBoxLayout* editorLayout = new QVBoxLayout{this};
    editorLayout->addWidget(editorKeyBindingLabel);
    editorLayout->addWidget(editorKeyBindingCombo);
    editorLayout->addWidget(srcCodeHighlightEditorLabel);
    editorLayout->addWidget(srcCodeHighlightEditorCheck);
    editorLayout->addWidget(tabWidthLabel);
    editorLayout->addWidget(tabWidthCombo);
    QGroupBox* editorGroup = new QGroupBox{tr("Editor"), this};
    editorGroup->setLayout(editorLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(appearanceGroup);
    boxesLayout->addWidget(editorGroup);
    boxesLayout->addStretch();
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
    delete srcCodeHighlightEditorLabel;
    delete srcCodeHighlightEditorCheck;
    delete tabWidthLabel;
    delete tabWidthCombo;
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

    srcCodeHighlightEditorCheck->setChecked(config.isUiEditorEnableSyntaxHighlighting());
    tabWidthCombo->setCurrentIndex(tabWidthCombo->findText(QString::number(config.getUiEditorTabWidth())));
}

void ConfigurationDialog::AppTab::save()
{
    config.setUiThemeName(themeCombo->itemText(themeCombo->currentIndex()).toStdString());
    config.setUiHtmlCssPath(htmlCssThemeCombo->itemText(htmlCssThemeCombo->currentIndex()).toStdString());
    config.setEditorKeyBindingByString(editorKeyBindingCombo->itemText(editorKeyBindingCombo->currentIndex()).toStdString());
    config.setUiEditorEnableSyntaxHighlighting(srcCodeHighlightEditorCheck->isChecked());
    config.setUiEditorTabWidth(tabWidthCombo->itemText(tabWidthCombo->currentIndex()).toInt());
}

/*
 * Markdown tab
 */

ConfigurationDialog::MarkdownTab::MarkdownTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    QGroupBox* viewerGroup = new QGroupBox{tr("Viewer"), this};

    srcCodeHighlightingSupportLabel = new QLabel(tr("Source code syntax highlighting support")+":", this),
    srcCodeHighlightSupportCheck = new QCheckBox{this};

    mathSupportLabel = new QLabel(tr("Math support")+":", this),
    mathSupportCheck = new QCheckBox{this};

    diagramSupportLabel = new QLabel(tr("Diagram support")+":", this),
    diagramSupportCombo = new QComboBox{this};
    diagramSupportCombo->addItem(QString{"disable"});
    diagramSupportCombo->addItem(QString{"offline JavaScript lib"});
    diagramSupportCombo->addItem(QString{"online JavaScript lib"});

    // assembly
    QVBoxLayout* viewerLayout = new QVBoxLayout{this};
    viewerLayout->addWidget(srcCodeHighlightingSupportLabel);
    viewerLayout->addWidget(srcCodeHighlightSupportCheck);
    viewerLayout->addWidget(mathSupportLabel);
    viewerLayout->addWidget(mathSupportCheck);
    viewerLayout->addWidget(diagramSupportLabel);
    viewerLayout->addWidget(diagramSupportCombo);
    viewerGroup->setLayout(viewerLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(viewerGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::MarkdownTab::~MarkdownTab()
{
    delete srcCodeHighlightingSupportLabel;
    delete srcCodeHighlightSupportCheck;
    delete mathSupportLabel;
    delete mathSupportCheck;
    delete diagramSupportLabel;
    delete diagramSupportCombo;
}

void ConfigurationDialog::MarkdownTab::refresh()
{
    srcCodeHighlightSupportCheck->setChecked(config.isUiEnableSrcHighlightInMd());
    mathSupportCheck->setChecked(config.isUiEnableMathInMd());
    diagramSupportCombo->setCurrentIndex(config.getUiEnableDiagramsInMd());
}

void ConfigurationDialog::MarkdownTab::save()
{
    config.setUiEnableSrcHighlightInMd(srcCodeHighlightSupportCheck->isChecked());
    config.setUiEnableMathInMd(mathSupportCheck->isChecked());
    config.setUiEnableDiagramsInMd(static_cast<Configuration::JavaScriptLibSupport>(diagramSupportCombo->currentIndex()));
}

/*
 * Mind tab
 */

ConfigurationDialog::MindTab::MindTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    // IMPROVE horizontal panel w/ label & check same line
    saveReadsMetadataLabel = new QLabel(tr("Save reads metadata")+":", this);
    saveReadsMetadataCheck = new QCheckBox(this);

    distributorSleepIntervalLabel = new QLabel(tr("Async refresh interval (1 - 10.000ms)")+":", this);
    distributorSleepIntervalSpin = new QSpinBox(this);
    distributorSleepIntervalSpin->setMinimum(1);
    distributorSleepIntervalSpin->setMaximum(10000);

    // assembly
    QVBoxLayout* pLayout = new QVBoxLayout{this};
    pLayout->addWidget(saveReadsMetadataLabel);
    pLayout->addWidget(saveReadsMetadataCheck);
    pLayout->addWidget(distributorSleepIntervalLabel);
    pLayout->addWidget(distributorSleepIntervalSpin);
    QGroupBox* pGroup = new QGroupBox{tr("Persistence"), this};
    pGroup->setLayout(pLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(pGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::MindTab::~MindTab()
{
    delete saveReadsMetadataLabel;
    delete saveReadsMetadataCheck;
    delete distributorSleepIntervalLabel;
    delete distributorSleepIntervalSpin;
}

void ConfigurationDialog::MindTab::refresh()
{
    saveReadsMetadataCheck->setChecked(config.isSaveReadsMetadata());
    distributorSleepIntervalSpin->setValue(config.getDistributorSleepInterval());
}

void ConfigurationDialog::MindTab::save()
{
    config.setSaveReadsMetadata(saveReadsMetadataCheck->isChecked());
    config.setDistributorSleepInterval(distributorSleepIntervalSpin->value());
}

} // m8r namespace

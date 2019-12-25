/*
 configuration_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
    viewerTab = new ViewerTab{this};
    editorTab = new EditorTab{this};
    navigatorTab = new NavigatorTab{this};
    mindTab = new MindTab{this};

    tabWidget->addTab(appTab, tr("Application"));
    tabWidget->addTab(viewerTab, tr("Viewer"));
    tabWidget->addTab(editorTab, tr("Editor"));
    tabWidget->addTab(navigatorTab, tr("Navigator"));
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
    // tabs are auto-destructured thanks to parent/child hierarchy
}

void ConfigurationDialog::show()
{
    appTab->refresh();
    viewerTab->refresh();
    editorTab->refresh();
    navigatorTab->refresh();
    mindTab->refresh();

    QDialog::show();
}

void ConfigurationDialog::saveSlot()
{
    appTab->save();
    viewerTab->save();
    editorTab->save();
    navigatorTab->save();
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
    themeCombo->addItem(QString{UI_THEME_NATIVE});

    showToolbarCheck = new QCheckBox(tr("show toolbar"), this);
    nerdMenuCheck = new QCheckBox(tr("nerd menu (requires restart)"), this);

    // assembly
    QVBoxLayout* appearanceLayout = new QVBoxLayout{this};
    appearanceLayout->addWidget(themeLabel);
    appearanceLayout->addWidget(themeCombo);
    appearanceLayout->addWidget(showToolbarCheck);
    appearanceLayout->addWidget(nerdMenuCheck);
    QGroupBox* appearanceGroup = new QGroupBox{tr("Appearance"), this};
    appearanceGroup->setLayout(appearanceLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(appearanceGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::AppTab::~AppTab()
{
    delete themeLabel;
    delete themeCombo;
    delete showToolbarCheck;
}

void ConfigurationDialog::AppTab::refresh()
{
    int i = themeCombo->findText(QString::fromStdString(config.getUiThemeName()));
    if(i>=0) {
        themeCombo->setCurrentIndex(i);
    }
    showToolbarCheck->setChecked(config.isUiShowToolbar());
    nerdMenuCheck->setChecked(config.isUiNerdTargetAudience());
}

void ConfigurationDialog::AppTab::save()
{
    config.setUiThemeName(themeCombo->itemText(themeCombo->currentIndex()).toStdString());
    config.setUiShowToolbar(showToolbarCheck->isChecked());
    config.setUiNerdTargetAudience(nerdMenuCheck->isChecked());
}

/*
 * Viewer tab
 */

ConfigurationDialog::ViewerTab::ViewerTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    QGroupBox* viewerGroup = new QGroupBox{tr("HTML Viewer"), this};

    htmlCssThemeLabel = new QLabel(tr("Viewer theme CSS")+":", this);
    htmlCssThemeCombo = new QComboBox{this};
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_LIGHT});
    // htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_LIGHT_COMPACT});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_DARK});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_RAW});

    zoomLabel = new QLabel(tr("HTML zoom (100 is 100%, Ctrl + mouse wheel)")+":", this);
    zoomSpin = new QSpinBox(this);
    zoomSpin->setMinimum(25);
    zoomSpin->setMaximum(500);

    srcCodeHighlightSupportCheck = new QCheckBox{tr("source code syntax highlighting support"), this};

    mathSupportCheck = new QCheckBox{tr("math support"), this};

    diagramSupportLabel = new QLabel(tr("Diagram support")+":", this),
    diagramSupportCombo = new QComboBox{this};
    diagramSupportCombo->addItem(QString{"disable"});
    diagramSupportCombo->addItem(QString{"offline JavaScript lib"});
    diagramSupportCombo->addItem(QString{"online JavaScript lib"});

    // assembly
    QVBoxLayout* viewerLayout = new QVBoxLayout{this};
    viewerLayout->addWidget(htmlCssThemeLabel);
    viewerLayout->addWidget(htmlCssThemeCombo);
    viewerLayout->addWidget(zoomLabel);
    viewerLayout->addWidget(zoomSpin);
    viewerLayout->addWidget(diagramSupportLabel);
    viewerLayout->addWidget(diagramSupportCombo);
    viewerLayout->addWidget(srcCodeHighlightSupportCheck);
    viewerLayout->addWidget(mathSupportCheck);
    viewerGroup->setLayout(viewerLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(viewerGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::ViewerTab::~ViewerTab()
{
    delete htmlCssThemeLabel;
    delete htmlCssThemeCombo;
    delete zoomLabel;
    delete zoomSpin;
    delete srcCodeHighlightSupportCheck;
    delete mathSupportCheck;
    delete diagramSupportLabel;
    delete diagramSupportCombo;
}

void ConfigurationDialog::ViewerTab::refresh()
{
    int i = htmlCssThemeCombo->findText(QString::fromStdString(config.getUiHtmlCssPath()));
    if(i>=0) {
        htmlCssThemeCombo->setCurrentIndex(i);
    }

    zoomSpin->setValue(config.getUiHtmlZoom());
    srcCodeHighlightSupportCheck->setChecked(config.isUiEnableSrcHighlightInMd());
    mathSupportCheck->setChecked(config.isUiEnableMathInMd());
    diagramSupportCombo->setCurrentIndex(config.getUiEnableDiagramsInMd());
}

void ConfigurationDialog::ViewerTab::save()
{
    config.setUiHtmlCssPath(htmlCssThemeCombo->itemText(htmlCssThemeCombo->currentIndex()).toStdString());
    config.setUiHtmlZoom(zoomSpin->value());
    config.setUiEnableSrcHighlightInMd(srcCodeHighlightSupportCheck->isChecked());
    config.setUiEnableMathInMd(mathSupportCheck->isChecked());
    config.setUiEnableDiagramsInMd(static_cast<Configuration::JavaScriptLibSupport>(diagramSupportCombo->currentIndex()));
}

/*
 * Editor tab
 */

ConfigurationDialog::EditorTab::EditorTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    editorKeyBindingLabel = new QLabel(tr("Editor key binding")+":", this);
    editorKeyBindingCombo = new QComboBox{this};
    editorKeyBindingCombo->addItem("emacs");
    editorKeyBindingCombo->addItem("vim");
    editorKeyBindingCombo->addItem("windows");

    editorFontLabel = new QLabel(tr("Editor font")+":", this);
    editorFontButton = new QPushButton(QFontDatabase::systemFont(QFontDatabase::FixedFont).family());
    QObject::connect(editorFontButton, &QPushButton::clicked, this, &ConfigurationDialog::EditorTab::getFont);

    editorMdSyntaxHighlightCheck = new QCheckBox(tr("Markdown syntax highlighting"), this);
    editorAutocompleteCheck = new QCheckBox(tr("autocomplete"), this);
    //editorQuoteSectionsCheck = new QCheckBox(tr("quote sections (# in description)"), this);
    editorTabsAsSpacesCheck = new QCheckBox(tr("TABs as SPACEs"), this);

    editorTabWidthLabel = new QLabel(tr("TAB width")+":", this);
    editorTabWidthCombo = new QComboBox(this);
    editorTabWidthCombo->addItem(QString{"4"});
    editorTabWidthCombo->addItem(QString{"8"});

    // assembly
    QVBoxLayout* editorLayout = new QVBoxLayout{this};
    editorLayout->addWidget(editorKeyBindingLabel);
    editorLayout->addWidget(editorKeyBindingCombo);
    editorLayout->addWidget(editorFontLabel);
    editorLayout->addWidget(editorFontButton);
    editorLayout->addWidget(editorTabWidthLabel);
    editorLayout->addWidget(editorTabWidthCombo);
    editorLayout->addWidget(editorTabsAsSpacesCheck);
    editorLayout->addWidget(editorMdSyntaxHighlightCheck);
    editorLayout->addWidget(editorAutocompleteCheck);
    //editorLayout->addWidget(editorQuoteSectionsCheck);
    QGroupBox* editorGroup = new QGroupBox{tr("Markdown Editor"), this};
    editorGroup->setLayout(editorLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(editorGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::EditorTab::~EditorTab()
{
    delete editorKeyBindingLabel;
    delete editorKeyBindingCombo;
    delete editorFontLabel;
    delete editorFontButton;
    delete editorMdSyntaxHighlightCheck;
    delete editorAutocompleteCheck;
    delete editorTabWidthLabel;
    delete editorTabWidthCombo;
    //delete editorQuoteSectionsCheck;
    delete editorTabsAsSpacesCheck;
}

void ConfigurationDialog::EditorTab::refresh()
{
    int i = editorKeyBindingCombo->findText(QString::fromStdString(config.getEditorKeyBindingAsString()));
    if(i>=0) {
        editorKeyBindingCombo->setCurrentIndex(i);
    }

    editorFont.fromString(QString::fromStdString(config.getEditorFont()));
    editorFontButton->setText(editorFont.family());

    editorMdSyntaxHighlightCheck->setChecked(config.isUiEditorEnableSyntaxHighlighting());
    editorAutocompleteCheck->setChecked(config.isUiEditorEnableAutocomplete());
    editorTabWidthCombo->setCurrentIndex(editorTabWidthCombo->findText(QString::number(config.getUiEditorTabWidth())));
    //editorQuoteSectionsCheck->setChecked(config.isMarkdownQuoteSections());
    editorTabsAsSpacesCheck->setChecked(config.isUiEditorTabsAsSpaces());
}

void ConfigurationDialog::EditorTab::save()
{
    config.setEditorKeyBindingByString(editorKeyBindingCombo->itemText(editorKeyBindingCombo->currentIndex()).toStdString());
    config.setEditorFont(editorFont.family().append(",").append(QString::number(editorFont.pointSize())).toStdString());
    config.setUiEditorEnableSyntaxHighlighting(editorMdSyntaxHighlightCheck->isChecked());
    config.setUiEditorEnableAutocomplete(editorAutocompleteCheck->isChecked());
    config.setUiEditorTabWidth(editorTabWidthCombo->itemText(editorTabWidthCombo->currentIndex()).toInt());
    //config.setMarkdownQuoteSections(editorQuoteSectionsCheck->isChecked());
    config.setUiEditorTabsAsSpaces(editorTabsAsSpacesCheck->isChecked());
}

void ConfigurationDialog::EditorTab::getFont()
{
    QFont font;
    font.fromString(QString::fromStdString(config.getEditorFont()));

    bool ok;
    editorFont = QFontDialog::getFont(&ok, font, this);
    if(ok) {
         editorFontButton->setText(editorFont.family());
    }
}

/*
 * Mind tab
 */

ConfigurationDialog::MindTab::MindTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    // IMPROVE horizontal panel w/ label & check same line
    saveReadsMetadataCheck = new QCheckBox(tr("save reads metadata"), this);

    distributorSleepIntervalLabel = new QLabel(tr("Async refresh interval (1 - 10.000ms)")+":", this);
    distributorSleepIntervalSpin = new QSpinBox(this);
    distributorSleepIntervalSpin->setMinimum(1);
    distributorSleepIntervalSpin->setMaximum(10000);

    // assembly
    QVBoxLayout* pLayout = new QVBoxLayout{this};
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

/*
 * Navigator tab
 */

ConfigurationDialog::NavigatorTab::NavigatorTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    maxNodesLabel = new QLabel(tr("Max graph nodes (150 by default)")+":", this);
    maxNodesSpin = new QSpinBox(this);
    maxNodesSpin->setMinimum(1);
    maxNodesSpin->setMaximum(5000);

    // assembly
    QVBoxLayout* pLayout = new QVBoxLayout{this};
    pLayout->addWidget(maxNodesLabel);
    pLayout->addWidget(maxNodesSpin);
    QGroupBox* pGroup = new QGroupBox{tr("Knowledge Graph Navigator"), this};
    pGroup->setLayout(pLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(pGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::NavigatorTab::~NavigatorTab()
{
    delete maxNodesLabel;
    delete maxNodesSpin;
}

void ConfigurationDialog::NavigatorTab::refresh()
{
    maxNodesSpin->setValue(config.getNavigatorMaxNodes());
}

void ConfigurationDialog::NavigatorTab::save()
{
    config.setNavigatorMaxNodes(maxNodesSpin->value());
}

} // m8r namespace

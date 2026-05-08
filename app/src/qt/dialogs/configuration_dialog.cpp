/*
 configuration_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "add_llm_provider_dialog.h"
#include "openai_config_dialog.h"
#include "ollama_config_dialog.h"
#include "openrouter_config_dialog.h"

namespace m8r {

using namespace std;

ConfigurationDialog::ConfigurationDialog(QWidget* parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;

    appTab = new AppTab{this};
    viewerTab = new ViewerTab{this};
    editorTab = new EditorTab{this};
    markdownTab = new MarkdownTab{this};
    navigatorTab = new NavigatorTab{this};
    mindTab = new MindTab{this};
    wingmanTab = new WingmanTab{this};

    tabWidget->addTab(appTab, tr("Application"));
    tabWidget->addTab(wingmanTab, tr("Wingman"));
    tabWidget->addTab(viewerTab, tr("Viewer"));
    tabWidget->addTab(editorTab, tr("Editor"));
    tabWidget->addTab(markdownTab, tr("Markdown"));
    tabWidget->addTab(mindTab, tr("Mind"));
    tabWidget->addTab(navigatorTab, tr("Navigator"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // signals
    QObject::connect(
        buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(
        buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(
        buttonBox, &QDialogButtonBox::accepted, this, &ConfigurationDialog::saveSlot);

    QVBoxLayout* mainLayout = new QVBoxLayout{this};
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Adapt"));
    resize(fontMetrics().averageCharWidth()*75, 0);
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
    markdownTab->refresh();
    navigatorTab->refresh();
    mindTab->refresh();
    wingmanTab->refresh();

    QDialog::show();
}

void ConfigurationDialog::saveSlot()
{
    appTab->save();
    viewerTab->save();
    editorTab->save();
    markdownTab->save();
    navigatorTab->save();
    mindTab->save();
    wingmanTab->save();

    // callback: notify components on config change using signals defined in
    // the main window presenter
    emit saveConfigSignal();
}

/*
 * App tab
 */

ConfigurationDialog::AppTab::AppTab(QWidget *parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    themeLabel = new QLabel(
        tr("UI theme (<font color='#ff0000'>requires restart</font>)")+":", this);
    menuLabel = new QLabel(
        tr("Menu (<font color='#ff0000'>requires restart</font>)")+":", this);
    themeCombo = new QComboBox{this};
    themeCombo->addItem(QString{UI_THEME_LIGHT});
#ifndef __APPLE__
    themeCombo->addItem(QString{UI_THEME_LIGHT_WITH_FIXED_FONT});
#endif
    themeCombo->addItem(QString{UI_THEME_DARK});
    themeCombo->addItem(QString{UI_THEME_BLACK});
#ifndef __APPLE__
    themeCombo->addItem(QString{UI_THEME_BLACK_WITH_FIXED_FONT});
#endif
    themeCombo->addItem(QString{UI_THEME_NATIVE});
#ifndef __APPLE__
    themeCombo->addItem(QString{UI_THEME_NATIVE_WITH_FIXED_FONT});
#endif

    startupLabel = new QLabel(tr("Show the following view on application start")+":", this);
    startupCombo = new QComboBox{this};
    startupCombo->addItem(QString{START_TO_OUTLINES});
    startupCombo->addItem(QString{START_TO_OUTLINES_TREE});
    startupCombo->addItem(QString{START_TO_TAGS});
    startupCombo->addItem(QString{START_TO_RECENT});
#ifdef MF_BUG
    // must be fixed as it currently crashes
    startupCombo->addItem(QString{START_TO_EISENHOWER_MATRIX});
#endif
    startupCombo->addItem(QString{START_TO_HOME_OUTLINE});

    appFontSizeLabel = new QLabel(
        tr("Application font size - 0 is system (<font color='#ff0000'>requires restart</font>)")+":", this);
    appFontSizeSpin = new QSpinBox(this);
    appFontSizeSpin->setMinimum(0);
    appFontSizeSpin->setMaximum(68);

    showToolbarCheck = new QCheckBox(tr("show toolbar"), this);
    showToolbarCheck->setChecked(true);
    uiExpertModeCheck = new QCheckBox(
        tr("I don't need buttons - I know all keyboard shortcuts!"), this);
    nerdMenuCheck = new QCheckBox(tr("nerd terminology"), this);

    // assembly
    QVBoxLayout* startupLayout = new QVBoxLayout{this};
    startupLayout->addWidget(startupLabel);
    startupLayout->addWidget(startupCombo);
    QGroupBox* startupGroup = new QGroupBox{tr("Startup"), this};
    startupGroup->setLayout(startupLayout);

    QVBoxLayout* appearanceLayout = new QVBoxLayout{this};
    appearanceLayout->addWidget(themeLabel);
    appearanceLayout->addWidget(themeCombo);
    appearanceLayout->addWidget(appFontSizeLabel);
    appearanceLayout->addWidget(appFontSizeSpin);
    appearanceLayout->addWidget(menuLabel);
    appearanceLayout->addWidget(nerdMenuCheck);
    QGroupBox* appearanceGroup = new QGroupBox{
        tr("Appearance"),
        this};
    appearanceGroup->setLayout(appearanceLayout);

    QVBoxLayout* controlsLayout = new QVBoxLayout{this};
    controlsLayout->addWidget(showToolbarCheck);
    controlsLayout->addWidget(uiExpertModeCheck);
    QGroupBox* controlsGroup = new QGroupBox{tr("Controls"), this};
    controlsGroup->setLayout(controlsLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(startupGroup);
    boxesLayout->addWidget(appearanceGroup);
    boxesLayout->addWidget(controlsGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::AppTab::~AppTab()
{
    delete themeLabel;
    delete themeCombo;
    delete appFontSizeLabel;
    delete appFontSizeSpin;
    delete startupLabel;
    delete startupCombo;
    delete showToolbarCheck;
}

void ConfigurationDialog::AppTab::refresh()
{
    int i = startupCombo->findText(QString::fromStdString(config.getStartupView()));
    if(i>=0) {
        startupCombo->setCurrentIndex(i);
    }
    i = themeCombo->findText(QString::fromStdString(config.getUiThemeName()));
    if(i>=0) {
        themeCombo->setCurrentIndex(i);
    }
    showToolbarCheck->setChecked(config.isUiShowToolbar());
    appFontSizeSpin->setValue(config.getUiAppFontSize());
    uiExpertModeCheck->setChecked(config.isUiExpertMode());
    nerdMenuCheck->setChecked(config.isUiNerdTargetAudience());
}

void ConfigurationDialog::AppTab::save()
{
    config.setStartupView(startupCombo->itemText(startupCombo->currentIndex()).toStdString());
    config.setUiThemeName(themeCombo->itemText(themeCombo->currentIndex()).toStdString());
    config.setUiShowToolbar(showToolbarCheck->isChecked());
    config.setUiAppFontSize(appFontSizeSpin->value());
    config.setUiExpertMode(uiExpertModeCheck->isChecked());
    config.setUiNerdTargetAudience(nerdMenuCheck->isChecked());
}

/*
 * Viewer tab
 */

ConfigurationDialog::ViewerTab::ViewerTab(QWidget* parent)
    : QWidget(parent), config(Configuration::getInstance())
{
    zoomLabel = new QLabel(tr("HTML zoom (100 is 100%, Ctrl + mouse wheel)")+":", this);
    zoomSpin = new QSpinBox(this);
    zoomSpin->setMinimum(25);
    zoomSpin->setMaximum(500);

    srcCodeHighlightSupportCheck = new QCheckBox{
        tr("source code syntax highlighting support"), this};

    mathSupportCheck = new QCheckBox{tr("math support"), this};
    fullOPreviewCheck = new QCheckBox{tr("whole notebook preview"), this};
    doubleClickViewerToEditCheck = new QCheckBox{
        tr("double click HTML preview to edit"), this};

    diagramSupportLabel = new QLabel(tr("Diagram support")+":", this);
    diagramSupportCombo = new QComboBox{this};
    diagramSupportCombo->addItem(QString{"disable"});
    // TODO: to be stabilized diagramSupportCombo->addItem(QString{"offline JavaScript lib"});
    diagramSupportCombo->addItem(QString{"online JavaScript lib"});

    htmlCssThemeLabel = new QLabel(tr("Viewer theme CSS")+":", this);
    htmlCssThemeCombo = new QComboBox{this};
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_LIGHT});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_DARK});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_RAW});
    htmlCssThemeCombo->addItem(QString{UI_HTML_THEME_CSS_CUSTOM});
    htmlCssLineEdit = new QLineEdit(this);
    htmlCssLineEdit->setDisabled(true);
    htmlCssFindFileButton = new QPushButton(tr("Find Custom CSS File"));

    // signals
    QObject::connect(
        htmlCssFindFileButton, SIGNAL(clicked()),
        this, SLOT(slotFindCssFile()));
    QObject::connect(
        htmlCssThemeCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotCssChoiceChanged(int)));

    // assembly
    QGroupBox* viewerGroup = new QGroupBox{tr("HTML Viewer"), this};
    QVBoxLayout* viewerLayout = new QVBoxLayout{this};
    viewerLayout->addWidget(doubleClickViewerToEditCheck);
    viewerLayout->addWidget(fullOPreviewCheck);
    viewerLayout->addWidget(srcCodeHighlightSupportCheck);
    viewerLayout->addWidget(mathSupportCheck);
    viewerLayout->addWidget(diagramSupportLabel);
    viewerLayout->addWidget(diagramSupportCombo);
    viewerLayout->addWidget(zoomLabel);
    viewerLayout->addWidget(zoomSpin);
    viewerGroup->setLayout(viewerLayout);

    QGroupBox* viewerCssGroup = new QGroupBox{tr("HTML Viewer CSS"), this};
    QVBoxLayout* viewerCssLayout = new QVBoxLayout{this};
    viewerCssLayout->addWidget(htmlCssThemeLabel);
    viewerCssLayout->addWidget(htmlCssThemeCombo);
    viewerCssLayout->addWidget(htmlCssLineEdit);
    viewerCssLayout->addWidget(htmlCssFindFileButton);
    viewerCssGroup->setLayout(viewerCssLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(viewerGroup);
    boxesLayout->addWidget(viewerCssGroup);
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
    delete fullOPreviewCheck;
    delete diagramSupportLabel;
    delete diagramSupportCombo;
    delete doubleClickViewerToEditCheck;
}

void ConfigurationDialog::ViewerTab::refresh()
{
    int i = htmlCssThemeCombo->findText(QString::fromStdString(config.getUiHtmlCssPath()));
    if(i>=0) {
        htmlCssThemeCombo->setCurrentIndex(i);
        htmlCssLineEdit->clear();
    } else {
        htmlCssThemeCombo->setCurrentText(QString{UI_HTML_THEME_CSS_CUSTOM});
        htmlCssLineEdit->setText(config.getUiHtmlCssPath());
    }

    zoomSpin->setValue(config.getUiHtmlZoom());
    srcCodeHighlightSupportCheck->setChecked(config.isUiEnableSrcHighlightInMd());
    mathSupportCheck->setChecked(config.isUiEnableMathInMd());
    fullOPreviewCheck->setChecked(config.isUiFullOPreview());
    diagramSupportCombo->setCurrentIndex(config.getUiEnableDiagramsInMd());
    doubleClickViewerToEditCheck->setChecked(config.isUiDoubleClickNoteViewToEdit());
}

void ConfigurationDialog::ViewerTab::save()
{
    string css{UI_HTML_THEME_CSS_LIGHT};
    if(string{UI_HTML_THEME_CSS_CUSTOM}
       == htmlCssThemeCombo->itemText(htmlCssThemeCombo->currentIndex()).toStdString()
    ) {
        if(htmlCssLineEdit->text().size()) {
            css = htmlCssLineEdit->text().toStdString();
        } // else default CSS
    } else {
        css = htmlCssThemeCombo->itemText(htmlCssThemeCombo->currentIndex()).toStdString();
    }
    config.setUiHtmlCssPath(css);

    config.setUiHtmlZoom(zoomSpin->value());
    config.setUiEnableSrcHighlightInMd(srcCodeHighlightSupportCheck->isChecked());
    config.setUiEnableMathInMd(mathSupportCheck->isChecked());
    config.setUiFullOPreview(fullOPreviewCheck->isChecked());
    config.setUiEnableDiagramsInMd(
        static_cast<Configuration::JavaScriptLibSupport>(diagramSupportCombo->currentIndex())
    );
    config.setUiDoubleClickNoteViewToEdit(doubleClickViewerToEditCheck->isChecked());
}

void ConfigurationDialog::ViewerTab::slotFindCssFile()
{
    QString homeDirectory = QStandardPaths::locate(
        QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory
    );

    QFileDialog fileDialog{this};
    fileDialog.setWindowTitle(tr("Choose CSS File"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setDirectory(homeDirectory);
    fileDialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames{};
    if(fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
        if(fileNames.size()==1) {
            htmlCssLineEdit->setText(fileNames[0]);
            htmlCssThemeCombo->setCurrentText(QString{UI_HTML_THEME_CSS_CUSTOM});
            return;
        } // else too many files
    } // else directory closed / nothing choosen

    // set default CSS
    htmlCssThemeCombo->setCurrentText(QString{UI_HTML_THEME_CSS_LIGHT});
}

void ConfigurationDialog::ViewerTab::slotCssChoiceChanged(int index)
{
    if(string{UI_HTML_THEME_CSS_CUSTOM} == htmlCssThemeCombo->itemText(index).toStdString()
       && !htmlCssLineEdit->text().size()
       && isVisible()
    ) {
        this->slotFindCssFile();
    }
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
    editorKeyBindingCombo->addItem("windows");

    editorFontLabel = new QLabel(tr("Editor font")+":", this);
    editorFontButton = new QPushButton(QFontDatabase::systemFont(QFontDatabase::FixedFont).family());
    QObject::connect(editorFontButton, &QPushButton::clicked, this, &ConfigurationDialog::EditorTab::getFont);

    editorSpellCheckHelp = new QLabel(
        tr("Spell check dictionaries <a href='"
           "https://github.com/dvorka/mindforger/wiki/Installation#spell-check"
           "'>configuration documentation</a>"
        ),
        this
    );
    editorSpellCheckHelp->setTextFormat(Qt::RichText);
    editorSpellCheckHelp->setTextInteractionFlags(Qt::TextBrowserInteraction);
    editorSpellCheckHelp->setOpenExternalLinks(true);
    editorSpellCheckLive = new QCheckBox(tr("live spell check"), this);
    editorSpellCheckLanguageCombo = new QComboBox{this};
    editorSpellCheckLanguageCombo->clear();
    std::vector<std::string> langs = config.getUiEditorSpellCheckLanguages();
    if(langs.size()) {
        for(auto l: langs) {
            editorSpellCheckLanguageCombo->addItem(QString::fromStdString(l));
        }
        editorSpellCheckLive->setChecked(config.isUiEditorLiveSpellCheck());
    } else {
        editorSpellCheckLive->setEnabled(false);
        editorSpellCheckLive->setDisabled(true);
        editorSpellCheckLanguageCombo->setDisabled(true);
    }

    //editorQuoteSectionsCheck = new QCheckBox(tr("quote sections (# in description)"), this);
    editorTabsAsSpacesCheck = new QCheckBox(tr("TABs as SPACEs"), this);
    editorAutosaveCheck = new QCheckBox(tr("autosave Note on editor close"), this);

    editorTabWidthLabel = new QLabel(tr("TAB width")+":", this);
    editorTabWidthCombo = new QComboBox(this);
    editorTabWidthCombo->addItem(QString{"4"});
    editorTabWidthCombo->addItem(QString{"8"});

    externalEditorCmdLabel = new QLabel(tr("External editor command")+":", this);
    externalEditorCmdEdit = new QLineEdit("", this);

    // assembly
    QVBoxLayout* editorLayout = new QVBoxLayout{this};
    editorLayout->addWidget(editorSpellCheckLive);
    editorLayout->addWidget(editorSpellCheckLanguageCombo);
    editorLayout->addWidget(editorSpellCheckHelp);
    editorLayout->addWidget(editorTabsAsSpacesCheck);
    editorLayout->addWidget(editorTabWidthLabel);
    editorLayout->addWidget(editorTabWidthCombo);
    editorLayout->addWidget(editorFontLabel);
    editorLayout->addWidget(editorFontButton);
    editorLayout->addWidget(editorKeyBindingLabel);
    editorLayout->addWidget(editorKeyBindingCombo);
    editorLayout->addWidget(externalEditorCmdLabel);
    editorLayout->addWidget(externalEditorCmdEdit);
    editorLayout->addWidget(editorAutosaveCheck);
    //editorLayout->addWidget(editorQuoteSectionsCheck);
    QGroupBox* editorGroup = new QGroupBox{tr("Editor"), this};
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
    delete editorSpellCheckLive;
    delete editorSpellCheckLanguageCombo;
    delete editorTabWidthLabel;
    delete editorTabWidthCombo;
    delete externalEditorCmdLabel;
    delete externalEditorCmdEdit;
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

    editorSpellCheckLive->setChecked(config.isUiEditorLiveSpellCheck());
    if(editorSpellCheckLive->isEnabled()
       && editorSpellCheckLanguageCombo->count()
    ) {
        if(config.getUiEditorSpellCheckDefaultLanguage().size()) {
            editorSpellCheckLanguageCombo->setCurrentIndex(
               editorSpellCheckLanguageCombo->findText(
                   QString::fromStdString(config.getUiEditorSpellCheckDefaultLanguage())
               )
            );
        } else {
            editorSpellCheckLanguageCombo->setCurrentIndex(0);
        }
    }
    editorTabWidthCombo->setCurrentIndex(
        editorTabWidthCombo->findText(
            QString::number(config.getUiEditorTabWidth())
        )
    );
    externalEditorCmdEdit->setText(QString::fromStdString(config.getExternalEditorCmd()));
    //editorQuoteSectionsCheck->setChecked(config.isMarkdownQuoteSections());
    editorTabsAsSpacesCheck->setChecked(config.isUiEditorTabsAsSpaces());
    editorAutosaveCheck->setChecked(config.isUiEditorAutosave());
}

void ConfigurationDialog::EditorTab::save()
{
    config.setEditorKeyBindingByString(
        editorKeyBindingCombo->itemText(
            editorKeyBindingCombo->currentIndex()
        ).toStdString()
    );
    config.setEditorFont(editorFont.family().append(",").append(QString::number(editorFont.pointSize())).toStdString());
    config.setUiEditorLiveSpellCheck(editorSpellCheckLive->isChecked());
    if(editorSpellCheckLanguageCombo->isEnabled() && editorSpellCheckLanguageCombo->count()) {
        config.setUiEditorSpellCheckDefaultLanguage(
            editorSpellCheckLanguageCombo->itemText(
                editorSpellCheckLanguageCombo->currentIndex()
            ).toStdString()
        );
    } else {
        config.clearUiEditorSpellCheckDefaultLanguage();
    }
    config.setUiEditorTabWidth(editorTabWidthCombo->itemText(editorTabWidthCombo->currentIndex()).toInt());
    config.setExternalEditorCmd(externalEditorCmdEdit->text().toStdString());
    //config.setMarkdownQuoteSections(editorQuoteSectionsCheck->isChecked());
    config.setUiEditorTabsAsSpaces(editorTabsAsSpacesCheck->isChecked());
    config.setUiEditorAutosave(editorAutosaveCheck->isChecked());
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
 * Markdown tab
 */

ConfigurationDialog::MarkdownTab::MarkdownTab(QWidget *parent)
    : QWidget(parent),
      config(Configuration::getInstance())
{
    editorMdSyntaxHighlightCheck = new QCheckBox(
        tr("syntax highlighting"),
        this
    );
    editorAutocompleteCheck = new QCheckBox(
        tr("autocomplete text"),
        this
    );
    editorSmartEditorCheck = new QCheckBox(
        tr("autocomplete lists, blocks and {([`_ characters"),
        this
    );
    editorSmartEditorCheck->setChecked(true);
    editorMdSectionEscapingCheck = new QCheckBox(
        tr("SPACE-based # in section escaping (HTML otherwise)"),
        this
    );
    editorMdSectionEscapingCheck->setChecked(true);

    // assembly
    QVBoxLayout* renderingLayout = new QVBoxLayout{this};
    renderingLayout->addWidget(editorMdSyntaxHighlightCheck);
    QGroupBox* renderingGroup = new QGroupBox{tr("Rendering"), this};
    renderingGroup->setLayout(renderingLayout);

    QVBoxLayout* autocompleteLayout = new QVBoxLayout{this};
    autocompleteLayout->addWidget(editorAutocompleteCheck);
    autocompleteLayout->addWidget(editorSmartEditorCheck);
    QGroupBox* editorGroup = new QGroupBox{tr("Autocompletion"), this};
    editorGroup->setLayout(autocompleteLayout);

    QVBoxLayout* escapingLayout = new QVBoxLayout{this};
    escapingLayout->addWidget(editorMdSectionEscapingCheck);
    QGroupBox* escapingGroup = new QGroupBox{tr("Escaping"), this};
    escapingGroup->setLayout(escapingLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(renderingGroup);
    boxesLayout->addWidget(editorGroup);
    boxesLayout->addWidget(escapingGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

ConfigurationDialog::MarkdownTab::~MarkdownTab()
{
    delete editorMdSyntaxHighlightCheck;
    delete editorAutocompleteCheck;
    delete editorSmartEditorCheck;
}

void ConfigurationDialog::MarkdownTab::refresh()
{
    editorMdSyntaxHighlightCheck->setChecked(config.isUiEditorEnableSyntaxHighlighting());
    editorAutocompleteCheck->setChecked(config.isUiEditorEnableAutocomplete());
    editorSmartEditorCheck->setChecked(config.isUiEditorEnableSmartEditor());
    editorMdSectionEscapingCheck->setChecked(config.isUiEditorSpaceSectionEscaping());
}

void ConfigurationDialog::MarkdownTab::save()
{
    config.setUiEditorEnableSyntaxHighlighting(editorMdSyntaxHighlightCheck->isChecked());
    config.setUiEditorEnableAutocomplete(editorAutocompleteCheck->isChecked());
    config.setUiEditorEnableSmartEditor(editorSmartEditorCheck->isChecked());
    config.setUiEditorSpaceSectionEscaping(editorMdSectionEscapingCheck->isChecked());
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
    QGroupBox* pGroup = new QGroupBox{tr("Persistence"), this};
    pGroup->setLayout(pLayout);

    QVBoxLayout* nLayout = new QVBoxLayout{this};
    nLayout->addWidget(distributorSleepIntervalLabel);
    nLayout->addWidget(distributorSleepIntervalSpin);
    QGroupBox* nGroup = new QGroupBox{tr("Notifications"), this};
    nGroup->setLayout(nLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(pGroup);
    boxesLayout->addWidget(nGroup);
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

/*
 * Wingman tab
 */

ConfigurationDialog::WingmanTab::WingmanTab(QWidget* parent)
    : QWidget(parent),
      config(Configuration::getInstance())
{
    helpLabel = new QLabel(
        tr("Wingman LLMs"), this);

    QLabel* providerLabel = new QLabel(tr("Use LLM:"), this);
    llmProvidersCombo = new QComboBox(this);

    addProviderButton = new QPushButton(tr("&Add LLM "), this);

    QHBoxLayout* llmRow1Layout = new QHBoxLayout();
    llmRow1Layout->addWidget(providerLabel);
    llmRow1Layout->addWidget(llmProvidersCombo, 1);

    QHBoxLayout* llmRow2Layout = new QHBoxLayout();
    llmRow2Layout->addStretch();
    llmRow2Layout->addWidget(addProviderButton);

    // provider details group
    providerDetailsGroup = new QGroupBox(tr("Selected LLM details"), this);

    QLabel* typeLabel = new QLabel(tr("Provider type:"), this);
    providerTypeValue = new QLabel("", this);

    QLabel* mLabel = new QLabel(tr("Model:"), this);
    modelValue = new QLabel("", this);

    QLabel* sLabel = new QLabel(tr("Status:"), this);
    statusValue = new QLabel("", this);

    testButton = new QPushButton(tr("Test Connection"), this);
    removeButton = new QPushButton(tr("Remove"), this);

    QGridLayout* detailsLayout = new QGridLayout();
    detailsLayout->addWidget(typeLabel, 0, 0);
    detailsLayout->addWidget(providerTypeValue, 0, 1);
    detailsLayout->addWidget(mLabel, 1, 0);
    detailsLayout->addWidget(modelValue, 1, 1);
    detailsLayout->addWidget(sLabel, 2, 0);
    detailsLayout->addWidget(statusValue, 2, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(testButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();

    QVBoxLayout* groupLayout = new QVBoxLayout();
    groupLayout->addLayout(detailsLayout);
    groupLayout->addLayout(buttonLayout);

    providerDetailsGroup->setLayout(groupLayout);
    providerDetailsGroup->setVisible(false);

    // main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(helpLabel);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(llmRow1Layout);
    mainLayout->addLayout(llmRow2Layout);
    mainLayout->addWidget(providerDetailsGroup);
    mainLayout->addStretch();

    setLayout(mainLayout);

    // signals
    QObject::connect(
        addProviderButton, &QPushButton::clicked,
        this, &WingmanTab::handleAddProvider);
    QObject::connect(
        removeButton, &QPushButton::clicked,
        this, &WingmanTab::handleRemoveProvider);
    QObject::connect(
        testButton, &QPushButton::clicked,
        this, &WingmanTab::handleTestConnection);
    QObject::connect(
        llmProvidersCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(handleProviderSelectionChanged(int)));
}

ConfigurationDialog::WingmanTab::~WingmanTab()
{
}

void ConfigurationDialog::WingmanTab::refresh()
{
    // populate providers combo
    llmProvidersCombo->clear();

    vector<LlmProviderConfig>& providers = config.getLlmProviders();
    if (providers.empty()) {
        providerDetailsGroup->setVisible(false);
        return;
    }

    for (const auto& provider : providers) {
        llmProvidersCombo->addItem(
            QString::fromStdString(provider.displayName),
            QString::fromStdString(provider.id));
    }

    // select active provider
    LlmProviderConfig* activeProvider = config.getActiveLlmProvider();
    if (activeProvider) {
        int index = llmProvidersCombo->findData(
            QString::fromStdString(activeProvider->id));
        if (index >= 0) {
            llmProvidersCombo->setCurrentIndex(index);
        }
    }

    handleProviderSelectionChanged(llmProvidersCombo->currentIndex());
}

void ConfigurationDialog::WingmanTab::save()
{
    // save active provider selection
    if (llmProvidersCombo->count() > 0) {
        QString providerId = llmProvidersCombo->itemData(
            llmProvidersCombo->currentIndex()).toString();
        config.setActiveLlmProvider(providerId.toStdString());
    }
}

void ConfigurationDialog::WingmanTab::handleAddProvider()
{
    AddLlmProviderDialog addDialog(this);
    if (addDialog.exec() == QDialog::Accepted) {
        WingmanLlmProviders providerType = addDialog.getSelectedProviderType();

        if (providerType == WINGMAN_PROVIDER_OPENAI) {
            OpenAiConfigDialog configDialog(this);
            if (configDialog.exec() == QDialog::Accepted) {
                config.addLlmProvider(configDialog.getProviderConfig());
                refresh();
            }
        } else if (providerType == WINGMAN_PROVIDER_OLLAMA) {
            OllamaConfigDialog configDialog(this);
            if (configDialog.exec() == QDialog::Accepted) {
                config.addLlmProvider(configDialog.getProviderConfig());
                refresh();
            }
        } else if (providerType == WINGMAN_PROVIDER_OPENROUTER) {
            OpenRouterConfigDialog configDialog(this);
            if (configDialog.exec() == QDialog::Accepted) {
                config.addLlmProvider(configDialog.getProviderConfig());
                refresh();
            }
        }
    }
}

void ConfigurationDialog::WingmanTab::handleEditProvider()
{
    if (llmProvidersCombo->count() == 0) {
        return;
    }

    QString providerId = llmProvidersCombo->itemData(
        llmProvidersCombo->currentIndex()).toString();
    LlmProviderConfig* provider = config.getLlmProviderById(providerId.toStdString());

    if (!provider) {
        return;
    }

    // TODO: implement edit functionality
    QMessageBox::information(
        this,
        tr("Edit Provider"),
        tr("Edit functionality is not yet implemented."));
}

void ConfigurationDialog::WingmanTab::handleRemoveProvider()
{
    if (llmProvidersCombo->count() == 0) {
        return;
    }

    QString providerId = llmProvidersCombo->itemData(
        llmProvidersCombo->currentIndex()).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Remove Provider"),
        tr("Are you sure you want to remove this LLM provider configuration?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        config.removeLlmProvider(providerId.toStdString());
        refresh();
    }
}

void ConfigurationDialog::WingmanTab::handleTestConnection()
{
    if (llmProvidersCombo->count() == 0) {
        return;
    }

    QString providerId = llmProvidersCombo->itemData(
        llmProvidersCombo->currentIndex()).toString();
    LlmProviderConfig* provider = config.getLlmProviderById(providerId.toStdString());

    if (!provider) {
        return;
    }

    string errorMessage;
    bool success = false;

    if (provider->providerType == WINGMAN_PROVIDER_OPENAI) {
        success = config.probeOpenAiProvider(
            provider->apiKey, provider->llmModel, errorMessage);
    } else if (provider->providerType == WINGMAN_PROVIDER_OLLAMA) {
        success = config.probeOllamaProvider(
            provider->url, provider->llmModel, errorMessage);
    } else if (provider->providerType == WINGMAN_PROVIDER_OPENROUTER) {
        success = config.probeOpenRouterProvider(
            provider->apiKey, provider->llmModel, errorMessage);
    }

    if (success) {
        QMessageBox::information(
            this,
            tr("Connection Test"),
            tr("Provider configuration is valid."));
    } else {
        QMessageBox::critical(
            this,
            tr("Connection Test"),
            tr("Provider configuration test failed: %1")
            .arg(QString::fromStdString(errorMessage)));
    }
}

void ConfigurationDialog::WingmanTab::handleProviderSelectionChanged(int index)
{
    if (index < 0 || llmProvidersCombo->count() == 0) {
        providerDetailsGroup->setVisible(false);
        return;
    }

    QString providerId = llmProvidersCombo->itemData(index).toString();
    LlmProviderConfig* provider = config.getLlmProviderById(providerId.toStdString());

    if (!provider) {
        providerDetailsGroup->setVisible(false);
        return;
    }

    // update details
    if (provider->providerType == WINGMAN_PROVIDER_OPENAI) {
        providerTypeValue->setText("OpenAI");
    } else if (provider->providerType == WINGMAN_PROVIDER_OLLAMA) {
        providerTypeValue->setText("ollama");
    } else if (provider->providerType == WINGMAN_PROVIDER_OPENROUTER) {
        providerTypeValue->setText("OpenRouter");
    } else {
        providerTypeValue->setText("Unknown");
    }

    modelValue->setText(QString::fromStdString(provider->llmModel));

    if (provider->isValid) {
        statusValue->setText(tr("Configured ✓"));
        statusValue->setStyleSheet("QLabel { color: green; }");
    } else {
        statusValue->setText(tr("Not validated"));
        statusValue->setStyleSheet("QLabel { color: orange; }");
    }

    providerDetailsGroup->setVisible(true);
}

} // m8r namespace

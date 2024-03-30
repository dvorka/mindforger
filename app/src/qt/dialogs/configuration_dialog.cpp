/*
 configuration_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
 * Wingman Open AI tab
 */

ConfigurationDialog::WingmanOpenAiTab::WingmanOpenAiTab(QWidget* parent, QComboBox* parentLlmProvidersCombo)
    : QWidget(parent),
      parentLlmProvidersCombo{parentLlmProvidersCombo},
      config(Configuration::getInstance())
{
    helpLabel = new QLabel(
        tr(
            "<html><a href='https://openai.com'>OpenAI</a> LLM provider configuration:\n"
            "<ul>"
            "<li>Generate new OpenAI API key at <a href='https://platform.openai.com/api-keys'>openai.com</a>.</li>"
            "<li>a) either set the <b>%1</b> environment variable<br/>"
                "with the API key<br/>"
                "b) or paste the API key below to save it <font color='#ff0000'>unencrypted</font> to<br/>"
                "<b>.mindforger.md</b> file in your home directory.</li>"
            "</ul>"
        ).arg(ENV_VAR_OPENAI_API_KEY));
    helpLabel->setVisible(!config.canWingmanOpenAiFromEnv());
    apiKeyLabel = new QLabel(tr("<br>API key:"));
    apiKeyLabel->setVisible(helpLabel->isVisible());
    apiKeyEdit = new QLineEdit(this);
    apiKeyEdit->setVisible(helpLabel->isVisible());
    // enabled on valid config > add ollama to drop down > choose it in drop down
    setOpenAiButton = new QPushButton(tr("Set OpenAI as LLM Provider"), this);
    setOpenAiButton->setToolTip(
        tr("Add OpenAI to the dropdown with LLM providers (if not there) and set it for use with Wingman")
    );
    setOpenAiButton->setVisible(helpLabel->isVisible());
    clearApiKeyButton = new QPushButton(tr("Clear API Key"), this);
    clearApiKeyButton->setVisible(helpLabel->isVisible());
    llmModelsLabel = new QLabel(tr("LLM model:"));
    llmModelsCombo = new QComboBox();
    llmModelsCombo->addItem(LLM_MODEL_NONE);
    llmModelsCombo->addItem(LLM_MODEL_GPT35_TURBO);
    llmModelsCombo->addItem(LLM_MODEL_GPT4);

    configuredLabel = new QLabel(
        tr("The OpenAI API key is configured using the environment variable."), this);
    configuredLabel->setVisible(!helpLabel->isVisible());

    QVBoxLayout* llmProvidersLayout = new QVBoxLayout();
    llmProvidersLayout->addWidget(helpLabel);
    llmProvidersLayout->addWidget(apiKeyLabel);
    llmProvidersLayout->addWidget(apiKeyEdit);
    llmProvidersLayout->addWidget(configuredLabel);
    llmProvidersLayout->addWidget(llmModelsLabel);
    llmProvidersLayout->addWidget(llmModelsCombo);
    QHBoxLayout* buttonsLayout = new QHBoxLayout{};
    buttonsLayout->addWidget(setOpenAiButton);
    buttonsLayout->addWidget(clearApiKeyButton);
    llmProvidersLayout->addLayout(buttonsLayout);
    llmProvidersLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(llmProvidersLayout);
    layout->addStretch();
    setLayout(layout);

    QObject::connect(
        setOpenAiButton, SIGNAL(clicked()),
        this, SLOT(setOpenAiSlot()));
    QObject::connect(
        clearApiKeyButton, SIGNAL(clicked()),
        this, SLOT(clearApiKeySlot()));
}

ConfigurationDialog::WingmanOpenAiTab::~WingmanOpenAiTab()
{
    delete helpLabel;
    delete configuredLabel;
    delete apiKeyLabel;
    delete apiKeyEdit;
    delete clearApiKeyButton;
    delete llmModelsLabel;
    delete llmModelsCombo;
}

void refreshWingmanLlmProvidersComboBox(
    QComboBox* parentLlmProvidersCombo,
    Configuration& config
)
{
    parentLlmProvidersCombo->clear();
    parentLlmProvidersCombo->addItem(WINGMAN_NONE_COMBO_LABEL); // NO LLM provider - NO Wingman
#ifdef MF_WIP
    if(config.canWingmanMock()) {
        parentLlmProvidersCombo->addItem(
            QString::fromStdString(WINGMAN_MOCK_COMBO_LABEL),
            WingmanLlmProviders::WINGMAN_PROVIDER_MOCK);
    }
#endif
    if(config.canWingmanOpenAi()) {
        parentLlmProvidersCombo->addItem(
            QString::fromStdString(WINGMAN_OPENAI_COMBO_LABEL),
            WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI);
    }
    if(config.canWingmanOllama()) {
        parentLlmProvidersCombo->addItem(
            QString::fromStdString(WINGMAN_OLLAMA_COMBO_LABEL),
            WingmanLlmProviders::WINGMAN_PROVIDER_OLLAMA);
    }

    // select configure provider in the combo box
    parentLlmProvidersCombo->setCurrentIndex(
        parentLlmProvidersCombo->findData(config.getWingmanLlmProvider()));

}

void ConfigurationDialog::WingmanOpenAiTab::setOpenAiSlot()
{
    MF_DEBUG("Signal SLOT: set OpenAI" << endl);

    if(apiKeyEdit->text().size()==0 && !config.canWingmanOpenAiFromEnv()) {
        QMessageBox::critical(
            this,
            tr("LLM Provider Config Error"),
            tr(
                "Unable to set OpenAI as LLM provider as neither API key is set in the configuration, "
                "nor it is defined and environment variable")
        );
        config.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_NONE);
        refreshWingmanLlmProvidersComboBox(parentLlmProvidersCombo, config);
    }

    save();
    config.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI);
    refreshWingmanLlmProvidersComboBox(parentLlmProvidersCombo, config);
}

void ConfigurationDialog::WingmanOpenAiTab::clearApiKeySlot()
{
    apiKeyEdit->clear();

    save();
    if(config.getWingmanLlmProvider() == WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI) {
        config.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_NONE);
    }
    refreshWingmanLlmProvidersComboBox(parentLlmProvidersCombo, config);
    QMessageBox::information(
        this,
        tr("OpenAI API Key Cleared"),
        tr(
            "API key has been cleared from the configuration and "
            "OpenAI is no longer the LLM provider.")
    );
}

void ConfigurationDialog::WingmanOpenAiTab::refresh()
{
    apiKeyEdit->setText(QString::fromStdString(config.getWingmanOpenAiApiKey()));
    llmModelsCombo->setCurrentText(
            QString::fromStdString(config.getWingmanOpenAiLlm()));
}

void ConfigurationDialog::WingmanOpenAiTab::save()
{
    config.setWingmanOpenAiApiKey(apiKeyEdit->text().toStdString());
    config.setWingmanOpenAiLlm(
        llmModelsCombo->itemText(llmModelsCombo->currentIndex()).toStdString());
}


/*
 * Wingman ollama
 */

ConfigurationDialog::WingmanOllamaTab::WingmanOllamaTab(QWidget* parent, QComboBox* parentLlmProvidersCombo)
    : QWidget(parent),
      parentLlmProvidersCombo{parentLlmProvidersCombo},
      config(Configuration::getInstance())
{
    helpLabel = new QLabel(
        tr(
            "<html><a href='https://ollama.com'>ollama</a> LLM provider configuration:\n"
            "<ul>"
            "<li>Set your ollama server URL - default is <a href='http://localhost:11434'>http://localhost:11434</a></li>"
            "</ul>"
        ).arg(ENV_VAR_OPENAI_API_KEY));
    helpLabel->setVisible(!config.canWingmanOllama());
    urlLabel = new QLabel(tr("<br>ollama server URL:"));
    urlEdit = new QLineEdit(this);
    // enabled on valid config > add ollama to drop down > choose it in drop down
    setOllamaButton = new QPushButton(tr("Set ollama as LLM Provider"), this);
    setOllamaButton->setToolTip(
        tr("Add ollama to the dropdown with LLM providers (if not there) and set it for use with Wingman")
    );
    clearUrlButton = new QPushButton(tr("Clear URL"), this);
    llmModelsLabel = new QLabel(tr("LLM model:"));
    llmModelsCombo = new QComboBox();
    llmModelsCombo->addItem(LLM_MODEL_NONE);

    QVBoxLayout* llmProvidersLayout = new QVBoxLayout();
    llmProvidersLayout->addWidget(helpLabel);
    llmProvidersLayout->addWidget(urlLabel);
    llmProvidersLayout->addWidget(urlEdit);
    llmProvidersLayout->addWidget(setOllamaButton);
    llmProvidersLayout->addWidget(clearUrlButton);
    llmProvidersLayout->addWidget(llmModelsLabel);
    llmProvidersLayout->addWidget(llmModelsCombo);
    llmProvidersLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(llmProvidersLayout);
    layout->addStretch();
    setLayout(layout);

    QObject::connect(
        setOllamaButton, SIGNAL(clicked()),
        this, SLOT(setOllamaSlot()));
    QObject::connect(
        clearUrlButton, SIGNAL(clicked()),
        this, SLOT(clearUrlSlot()));
}

ConfigurationDialog::WingmanOllamaTab::~WingmanOllamaTab()
{
    delete helpLabel;
    delete urlLabel;
    delete urlEdit;
    delete clearUrlButton;
    delete llmModelsLabel;
    delete llmModelsCombo;
}

void ConfigurationDialog::WingmanOllamaTab::setOllamaSlot()
{
    MF_DEBUG("Signal SLOT: set ollama" << endl);

    if(urlEdit->text().size()==0) {
        QMessageBox::critical(
            this,
            tr("LLM Provider Config Error"),
            tr(
                "Unable to set ollama as LLM provider as neither API key is set in the configuration, "
                "nor it is defined and environment variable")
        );
        config.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_NONE);
        refreshWingmanLlmProvidersComboBox(parentLlmProvidersCombo, config);
    }

    save();
    config.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_OLLAMA);
    refreshWingmanLlmProvidersComboBox(parentLlmProvidersCombo, config);
}

void ConfigurationDialog::WingmanOllamaTab::clearUrlSlot()
{
    urlEdit->clear();

    save();
    if(config.getWingmanLlmProvider() == WingmanLlmProviders::WINGMAN_PROVIDER_OLLAMA) {
        config.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_NONE);
    }
    refreshWingmanLlmProvidersComboBox(parentLlmProvidersCombo, config);
    QMessageBox::information(
        this,
        tr("ollama Server URL Cleared"),
        tr(
            "ollama server URL has been cleared from the configuration and "
            "ollama is no longer the LLM provider."));
}

void ConfigurationDialog::WingmanOllamaTab::refresh()
{
    urlEdit->setText(QString::fromStdString(config.getWingmanOllamaUrl()));
}

void ConfigurationDialog::WingmanOllamaTab::save()
{
    config.setWingmanOllamaUrl(urlEdit->text().toStdString());
}

/*
 * TODO: Wingman Open AI API tab
 */

/*
 * Wingman tab
 */

ConfigurationDialog::WingmanTab::WingmanTab(QWidget* parent)
    : QWidget(parent),
      config(Configuration::getInstance())
{
    llmProvidersLabel = new QLabel(tr("LLM provider:"), this);
    llmProvidersCombo = new QComboBox{this};
    QObject::connect(
        llmProvidersCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(handleComboBoxChanged(int))
    );

    wingmanTabWidget = new QTabWidget;
    wingmanOpenAiTab = new WingmanOpenAiTab{this, llmProvidersCombo};
    wingmanOllamaTab = new WingmanOllamaTab{this, llmProvidersCombo};
    wingmanTabWidget->addTab(wingmanOpenAiTab, tr("OpenAI"));
    wingmanTabWidget->addTab(wingmanOllamaTab, tr("ollama"));

    // assembly
    QVBoxLayout* nLayout = new QVBoxLayout{this};
    nLayout->addWidget(llmProvidersLabel);
    nLayout->addWidget(llmProvidersCombo);

    nLayout->addWidget(wingmanTabWidget);

    QGroupBox* nGroup = new QGroupBox{tr("Large language model (LLM) providers"), this};
    nGroup->setLayout(nLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(nGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}


void ConfigurationDialog::WingmanTab::handleComboBoxChanged(int index) {
    string comboItemLabel{llmProvidersCombo->itemText(index).toStdString()};
    MF_DEBUG("WingmanTab::handleComboBoxChange: '" << comboItemLabel << "'" << endl);
    if(this->isVisible() && comboItemLabel == WINGMAN_OPENAI_COMBO_LABEL) {
        QMessageBox::warning(
            this,
            tr("Data Privacy Warning"),
            tr(
                "You have chosen OpenAI as your Wingman LLM provider. "
                "Therefore, your data will be sent to OpenAI servers "
                "for GPT processing when you use Wingman."));
    }
}

ConfigurationDialog::WingmanTab::~WingmanTab()
{
    delete llmProvidersLabel;
    delete llmProvidersCombo;
}

void ConfigurationDialog::WingmanTab::refresh()
{
    // refresh LLM providers combo
    refreshWingmanLlmProvidersComboBox(llmProvidersCombo, config);

    wingmanOpenAiTab->refresh();
    wingmanOllamaTab->refresh();
}

void ConfigurationDialog::WingmanTab::save()
{
    // get LLM provider enum value from llmProvidersCombo
    WingmanLlmProviders llmProvider = static_cast<WingmanLlmProviders>(
        llmProvidersCombo->itemData(llmProvidersCombo->currentIndex()).toInt());
    config.setWingmanLlmProvider(llmProvider);

    // OpenAI tab
    wingmanOpenAiTab->save();
    // ollama tab
    wingmanOllamaTab->save();
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

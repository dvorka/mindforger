/*
 markdown_configuration_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "markdown_configuration_representation.h"

using namespace std;
using namespace m8r::filesystem;

namespace m8r {

constexpr const auto CONFIG_SECTION_APP = "Application";
constexpr const auto CONFIG_SECTION_MIND = "Mind";
constexpr const auto CONFIG_SECTION_REPOSITORIES= "Repositories";

// mind
constexpr const auto CONFIG_SETTING_MIND_STATE = "* Mind state: ";
constexpr const auto CONFIG_SETTING_MIND_TIME_SCOPE_LABEL = "* Time scope: ";
constexpr const auto CONFIG_SETTING_MIND_TAGS_SCOPE_LABEL = "* Tags scope: ";
constexpr const auto CONFIG_SETTING_MIND_DISTRIBUTOR_INTERVAL = "* Async refresh interval (ms): ";
constexpr const auto CONFIG_SETTING_MIND_AUTOLINKING = "* Autolinking: ";
constexpr const auto CONFIG_SETTING_MIND_WINGMAN_PROVIDER = "* Wingman LLM provider: ";
constexpr const auto CONFIG_SETTING_MIND_OPENAI_KEY = "* Wingman's OpenAI API key: ";

// application
constexpr const auto CONFIG_SETTING_STARTUP_VIEW_LABEL = "* Startup view: ";
constexpr const auto CONFIG_SETTING_UI_THEME_LABEL = "* Theme: ";
constexpr const auto CONFIG_SETTING_UI_APP_FONT_SIZE = "* Application font size: ";
constexpr const auto CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL = "* Markdown CSS theme: ";
constexpr const auto CONFIG_SETTING_UI_HTML_ZOOM_LABEL = "* Markdown HTML zoom: ";
constexpr const auto CONFIG_SETTING_UI_SHOW_TOOLBAR_LABEL =  "* Show toolbar: ";
constexpr const auto CONFIG_SETTING_UI_EXPERT_MODE_LABEL =  "* Hide expendable buttons: ";
constexpr const auto CONFIG_SETTING_UI_LIVE_NOTE_PREVIEW_LABEL =  "* Live note preview: ";
constexpr const auto CONFIG_SETTING_UI_OS_TABLE_SORT_COL_LABEL =  "* Outlines table sort column: ";
constexpr const auto CONFIG_SETTING_UI_OS_TABLE_SORT_ORDER_LABEL =  "* Outlines table sort order: ";
constexpr const auto CONFIG_SETTING_UI_CLICK_NOTE_VIEW_TO_EDIT_LABEL =  "* Enable double click note view to edit: ";
constexpr const auto CONFIG_SETTING_UI_NERD_MENU=  "* Nerd menu: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_KEY_BINDING_LABEL =  "* Editor key binding: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_FONT_LABEL =  "* Editor font: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_SYNTAX_HIGHLIGHT_LABEL =  "* Editor syntax highlighting: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_LIVE_SPELLCHECK_LABEL =  "* Live editor spell check: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_SPELLCHECK_LANG_LABEL =  "* Spell check language: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_AUTOCOMPLETE_LABEL =  "* Editor autocomplete: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_SMART_EDITOR_LABEL =  "* Smart editor: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_SPACE_SECTION_ESCAPING_LABEL =  "* Space-based # in section escaping: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_TAB_WIDTH_LABEL =  "* Editor TAB width: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_TABS_AS_SPACES_LABEL =  "* Editor insert SPACEs for TAB: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_AUTOSAVE_LABEL =  "* Editor autosave on close: ";
constexpr const auto CONFIG_SETTING_EXTERNAL_EDITOR_CMD_LABEL =  "* External editor command: ";
constexpr const auto CONFIG_SETTING_UI_FULL_O_PREVIEW_LABEL =  "* Full Outline preview: ";
constexpr const auto CONFIG_SETTING_NAVIGATOR_MAX_GRAPH_NODES_LABEL = "* Navigator max knowledge graph nodes: ";
constexpr const auto CONFIG_SETTING_MD_HIGHLIGHT_LABEL = "* Enable source code syntax highlighting support in Markdown: ";
constexpr const auto CONFIG_SETTING_MD_MATH_LABEL = "* Enable math support in Markdown: ";
constexpr const auto CONFIG_SETTING_MD_DIAGRAM_LABEL = "* Enable diagram support in Markdown: ";
constexpr const auto CONFIG_SETTING_SAVE_READS_METADATA_LABEL = "* Save reads metadata: ";

// repositories
constexpr const auto CONFIG_SETTING_ACTIVE_REPOSITORY_LABEL = "* Active repository: ";
constexpr const auto CONFIG_SETTING_REPOSITORY_LABEL = "* Repository: ";

MarkdownConfigurationRepresentation::MarkdownConfigurationRepresentation()
    : mdRepositoryCfgRepresentation{}
{
}

MarkdownConfigurationRepresentation::~MarkdownConfigurationRepresentation()
{
}

/*
 * Parse Configuration represented as AST and delete AST. Aim to be
 * as ROBUST as possible ~ handle eventual user typos and incorrect
 * formatting.
 */
void MarkdownConfigurationRepresentation::configuration(vector<MarkdownAstNodeSection*>* ast, Configuration& c)
{
    c.clear();

    if(ast) {
        size_t off = 0;
        if(ast->size()) {
            // skip configuration's preamble (if present)
            if(ast->at(off)->isPreambleSection()) {
                if(ast->size()>1) {
                    ++off;
                }
            }
            // 1st section contains just description (post declared/trailing hashes are ignored as they are not needed on save)
        }

        // parse remaining sections as configuration
        if(ast->size() > off+1) {
            off++;
            for(size_t i = off; i < ast->size(); i++) {
                vector<string*>* sectionBody;
                sectionBody = ast->at(i)->moveBody();
                configurationSection(ast->at(i)->getText(), sectionBody, c);

                for(string* l:*sectionBody) {
                    delete l;
                }
                delete sectionBody;
            }
        }

        // delete AST
        for(MarkdownAstNodeSection* node:*ast) {
            if(node!=nullptr) {
                delete node;
            }
        }
        delete ast;
    }
}

/*
 * Parse a section of the Configuration.
 */
void MarkdownConfigurationRepresentation::configurationSection(
    string* title,
    vector<string*>* body,
    Configuration& c
) {
    if(title && body && title->size() && body->size()) {
        if(!title->compare(CONFIG_SECTION_APP)) {
            MF_DEBUG("PARSING configuration section: Application" << endl);
            for(string* line: *body) {
                if(line && line->size() && line->at(0)=='*') {
                    if(line->find(CONFIG_SETTING_SAVE_READS_METADATA_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setSaveReadsMetadata(true);
                        } else {
                            c.setSaveReadsMetadata(false);
                        }
                    } else if(line->find(CONFIG_SETTING_STARTUP_VIEW_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_STARTUP_VIEW_LABEL));
                        // NOTE: startup view is NOT validated
                        if(t.size()) {
                            c.setStartupView(t);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_THEME_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_THEME_LABEL));
                        // NOTE: theme name is NOT validated
                        if(t.size()) {
                            c.setUiThemeName(t);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_APP_FONT_SIZE) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_APP_FONT_SIZE));
                        std::string::size_type st;
                        int i;
                        try {
                          i = std::stoi (t,&st);
                        }
                        catch(...) {
                          i = Configuration::DEFAULT_UI_APP_FONT_SIZE;
                        }
                        if(i<0) {
                            i=Configuration::DEFAULT_UI_APP_FONT_SIZE;
                        }
                        c.setUiAppFontSize(i);
                    } else if(line->find(CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL));
                        if(t.size()) {
                            // TODO: IMPORTANT - this is potential SECURITY threat - theme name is NOT validated
                            c.setUiHtmlCssPath(t);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_HTML_ZOOM_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_HTML_ZOOM_LABEL));
                        std::string::size_type st;
                        int i;
                        try {
                          i = std::stoi (t,&st);
                        }
                        catch(...) {
                          i = Configuration::DEFAULT_UI_HTML_ZOOM;
                        }
                        if(i<25 || i>500) {
                            i=Configuration::DEFAULT_UI_HTML_ZOOM;
                        }
                        c.setUiHtmlZoom(i);
                    } else if(line->find(CONFIG_SETTING_UI_SHOW_TOOLBAR_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiShowToolbar(true);
                        } else {
                            c.setUiShowToolbar(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EXPERT_MODE_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiExpertMode(true);
                        } else {
                            c.setUiExpertMode(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_LIVE_NOTE_PREVIEW_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiLiveNotePreview(true);
                        } else {
                            c.setUiLiveNotePreview(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_OS_TABLE_SORT_COL_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_OS_TABLE_SORT_COL_LABEL));
                        std::string::size_type st;
                        int i;
                        try {
                          i = std::stoi (t,&st);
                          if(i<0 || i>Configuration::DEFAULT_OS_TABLE_SORT_COLUMN) {
                              i=Configuration::DEFAULT_OS_TABLE_SORT_COLUMN;
                          }
                        }
                        catch(...) {
                          i = Configuration::DEFAULT_OS_TABLE_SORT_COLUMN;
                        }
                        c.setUiOsTableSortColumn(i);
                    } else if(line->find(CONFIG_SETTING_UI_OS_TABLE_SORT_ORDER_LABEL) != std::string::npos) {
                        if(line->find(UI_OS_TABLE_SORT_ORDER_ASC) != std::string::npos) {
                            c.setUiOsTableSortOrder(true);
                        } else {
                            c.setUiOsTableSortOrder(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_NERD_MENU) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiNerdTargetAudience(true);
                        } else {
                            c.setUiNerdTargetAudience(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_TABS_AS_SPACES_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorTabsAsSpaces(true);
                        } else {
                            c.setUiEditorTabsAsSpaces(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_AUTOSAVE_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorAutosave(true);
                        } else {
                            c.setUiEditorAutosave(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_CLICK_NOTE_VIEW_TO_EDIT_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiDoubleClickNoteViewToEdit(true);
                        } else {
                            c.setUiDoubleClickNoteViewToEdit(false);
                        }
                    } else if(line->find(CONFIG_SETTING_EXTERNAL_EDITOR_CMD_LABEL) != std::string::npos) {
                        string p = line->substr(strlen(CONFIG_SETTING_EXTERNAL_EDITOR_CMD_LABEL));
                        c.setExternalEditorCmd(p);
                    } else if(line->find(CONFIG_SETTING_UI_FULL_O_PREVIEW_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiFullOPreview(true);
                        } else {
                            c.setUiFullOPreview(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_KEY_BINDING_LABEL) != std::string::npos) {
                        if(line->find(UI_EDITOR_KEY_BINDING_EMACS) != std::string::npos) {
                            c.setEditorKeyBinding(Configuration::EditorKeyBindingMode::EMACS);
                        } else {
                            c.setEditorKeyBinding(Configuration::EditorKeyBindingMode::WINDOWS);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_FONT_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_EDITOR_FONT_LABEL));
                        if(t.size()) {
                            c.setEditorFont(t);
                        }
                    } else if(line->find(CONFIG_SETTING_MD_MATH_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEnableMathInMd(true);
                        } else {
                            c.setUiEnableMathInMd(false);
                        }
                    } else if(line->find(CONFIG_SETTING_MD_DIAGRAM_LABEL) != std::string::npos) {
                        if(line->find(UI_JS_LIB_ONLINE) != std::string::npos) {
                            c.setUiEnableDiagramsInMd(Configuration::JavaScriptLibSupport::ONLINE);
                        } else if(line->find(UI_JS_LIB_OFFLINE) != std::string::npos) {
                            c.setUiEnableDiagramsInMd(Configuration::JavaScriptLibSupport::OFFLINE);
                        } else {
                            c.setUiEnableDiagramsInMd(Configuration::JavaScriptLibSupport::NO);
                        }
                    } else if(line->find(CONFIG_SETTING_MD_HIGHLIGHT_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEnableSrcHighlightInMd(true);
                        } else {
                            c.setUiEnableSrcHighlightInMd(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_SYNTAX_HIGHLIGHT_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorEnableSyntaxHighlighting(true);
                        } else {
                            c.setUiEditorEnableSyntaxHighlighting(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_LIVE_SPELLCHECK_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorLiveSpellCheck(true);
                        } else {
                            c.setUiEditorLiveSpellCheck(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_SPELLCHECK_LANG_LABEL) != std::string::npos) {
                        string p = line->substr(strlen(CONFIG_SETTING_UI_EDITOR_SPELLCHECK_LANG_LABEL));
                        c.setUiEditorSpellCheckDefaultLanguage(p);
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_AUTOCOMPLETE_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorEnableAutocomplete(true);
                        } else {
                            c.setUiEditorEnableAutocomplete(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_SMART_EDITOR_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorEnableSmartEditor(true);
                        } else {
                            c.setUiEditorEnableSmartEditor(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_SPACE_SECTION_ESCAPING_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorSpaceSectionEscaping(true);
                        } else {
                            c.setUiEditorSpaceSectionEscaping(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_TAB_WIDTH_LABEL) != std::string::npos) {
                        if(line->find("8") != std::string::npos) {
                            c.setUiEditorTabWidth(8);
                        } else {
                            c.setUiEditorTabWidth(4);
                        }
                    } else if(line->find(CONFIG_SETTING_NAVIGATOR_MAX_GRAPH_NODES_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_NAVIGATOR_MAX_GRAPH_NODES_LABEL));
                        std::string::size_type st;
                        int i;
                        try {
                          i = std::stoi(t,&st);
                        }
                        catch(...) {
                          i = Configuration::DEFAULT_NAVIGATOR_MAX_GRAPH_NODES;
                        }
                        if(i<0) {
                            i=Configuration::DEFAULT_NAVIGATOR_MAX_GRAPH_NODES;
                        }
                        c.setNavigatorMaxNodes(i);
                    }
                }
            }
        } else if(!title->compare(CONFIG_SECTION_MIND)) {
            MF_DEBUG("PARSING configuration section: Mind" << endl);
            for(string* line: *body) {
                if(line && line->size() && line->at(0)=='*') {
                    if(line->find(CONFIG_SETTING_MIND_TIME_SCOPE_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_MIND_TIME_SCOPE_LABEL));
                        if(t.size()) {
                            TimeScope ts;
                            if(TimeScope::fromString(t, ts)) {
                                c.setTimeScope(ts);
                            }
                        }
                    } else if(line->find(CONFIG_SETTING_MIND_TAGS_SCOPE_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_MIND_TAGS_SCOPE_LABEL));
                        c.getTagsScope().clear();
                        if(t.size()) {
                            char **r = stringSplit(t.c_str(), ' ');
                            int i=0;
                            while(r[i]) {
                                c.getTagsScope().push_back(r[i]);
                                delete[] r[i++];
                            };
                            delete[] r;
                        }
                    } else if(line->find(CONFIG_SETTING_MIND_STATE) != std::string::npos) {
                        if(line->find("think") != std::string::npos) {
                            c.setDesiredMindState(Configuration::MindState::THINKING);
                        } else {
                            c.setDesiredMindState(Configuration::MindState::SLEEPING);
                        }
                    } else if(line->find(CONFIG_SETTING_MIND_DISTRIBUTOR_INTERVAL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_MIND_DISTRIBUTOR_INTERVAL));
                        std::string::size_type st;
                        int i;
                        try {
                          i = std::stoi (t,&st);
                        }
                        catch(...) {
                          i = Configuration::DEFAULT_EDITOR_TAB_WIDTH;
                        }
                        if(i<0) {
                            i=Configuration::DEFAULT_EDITOR_TAB_WIDTH;
                        }
                        i %= 10000;
                        c.setDistributorSleepInterval(i);
                    } else if(line->find(CONFIG_SETTING_MIND_AUTOLINKING) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setAutolinking(true);
                        } else {
                            c.setAutolinking(false);
                        }
                    } else if(line->find(CONFIG_SETTING_MIND_WINGMAN_PROVIDER) != std::string::npos) {
                        if(line->find(
                            c.getWingmanLlmProviderAsString(
                                WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI)) != std::string::npos
                        ) {
                            c.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI);
                        } else {
                            c.setWingmanLlmProvider(WingmanLlmProviders::WINGMAN_PROVIDER_NONE);
                        }
                    } else if(line->find(CONFIG_SETTING_MIND_OPENAI_KEY) != std::string::npos) {
                        string k = line->substr(strlen(CONFIG_SETTING_MIND_OPENAI_KEY));
                        c.setWingmanOpenAiApiKey(k);
                    }
                }
            }
        } else if(!title->compare(CONFIG_SECTION_REPOSITORIES)) {
            MF_DEBUG("PARSING configuration section: Repositories" << endl);
            for(string* line: *body) {
                if(line && line->size() && line->at(0)=='*') {
                    if(line->find(CONFIG_SETTING_ACTIVE_REPOSITORY_LABEL) != std::string::npos) {
                        string p = line->substr(strlen(CONFIG_SETTING_ACTIVE_REPOSITORY_LABEL));
                        if(p.size()) {
                            Repository* r = RepositoryIndexer::getRepositoryForPath(p);
                            if(r) {
                                MF_DEBUG("  Setting ACTIVE repository: " << r->getPath() << endl);
                                c.setActiveRepository(
                                    c.addRepository(r), this->mdRepositoryCfgRepresentation
                                );
                            } else {
                                cerr << "Unable to construct active repository for non-existent configured path: '" << p << "'" << endl;
                            }
                        } else {
                            cerr << "Unable to construct configured active repository as path is empty" << endl;
                        }
                    } else if(line->find(CONFIG_SETTING_REPOSITORY_LABEL) != std::string::npos) {
                        string p = line->substr(strlen(CONFIG_SETTING_REPOSITORY_LABEL));
                        if(p.size()) {
                            Repository* r = RepositoryIndexer::getRepositoryForPath(p);
                            if(r) {
                                c.addRepository(r);
                            } else {
                                cerr << "Unable to construct repository for non-existent configured path: '" << p << "'" << endl;
                            }
                        } else {
                            cerr << "Unable to construct configured repository as path is empty" << endl;
                        }
                    }
                }
            }
        }
    }
}

string* MarkdownConfigurationRepresentation::to(Configuration& c)
{
    string* md = new string{};
    to(&c, *md);
    return md;
}

string& MarkdownConfigurationRepresentation::to(Configuration* c, string& md)
{
    stringstream s{};
    string timeScopeAsString{}, tagsScopeAsString{}, mindStateAsString{"sleep"};
    if(c) {
        // time
        c->getTimeScope().toString(timeScopeAsString);
        // tags
        if(c->getTagsScope().size()) {
            for(string& t:c->getTagsScope()) {
                tagsScopeAsString += t;
                tagsScopeAsString += ",";
            }
            tagsScopeAsString.resize(tagsScopeAsString.size()-1);
        }
        // mind state
        if(c->getDesiredMindState()==Configuration::MindState::THINKING) mindStateAsString= "think";
    } else {
        timeScopeAsString.assign(Configuration::DEFAULT_TIME_SCOPE);
    }

    // IMPROVE build more in compile time and less in runtime
    s <<
         "# MindForger Configuration" << endl <<
         endl <<
         "This is MindForger configuration file (Markdown hosted DSL)." << endl <<
         "See documentation for configuration options details." << endl <<
         endl <<

         "# " << CONFIG_SECTION_MIND << endl <<
         "Mind-related settings:" << endl <<
         endl <<
         CONFIG_SETTING_MIND_STATE << mindStateAsString << endl <<
         "    * Examples: sleep, think" << endl <<
         CONFIG_SETTING_MIND_TIME_SCOPE_LABEL << timeScopeAsString << endl <<
         "    * Examples: 2y0m0d0h0m (recent 2 years), 0y3m15d0h0m (recent 3 months and 15 days)" << endl <<
         CONFIG_SETTING_MIND_TAGS_SCOPE_LABEL << tagsScopeAsString << endl <<
         "    * Examples: important (shown Notebooks must be tagged with 'important'); if no tag is specified, then tags scope is disabled" << endl <<
         CONFIG_SETTING_MIND_DISTRIBUTOR_INTERVAL << (c?c->getDistributorSleepInterval():Configuration::DEFAULT_DISTRIBUTOR_SLEEP_INTERVAL+1) << endl <<
         "    * Sleep interval (miliseconds) between asynchronous mind-related evaluations (associations, ...)" << endl <<
         "    * Examples: 500, 1000, 3000, 5000" << endl <<
         CONFIG_SETTING_MIND_AUTOLINKING << (c?(c->isAutolinking()?"yes":"no"):(Configuration::DEFAULT_AUTOLINKING?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_MIND_WINGMAN_PROVIDER << Configuration::getWingmanLlmProviderAsString(c?c->getWingmanLlmProvider():Configuration::DEFAULT_WINGMAN_LLM_PROVIDER) << endl <<
         "    * Examples: none, openai" << endl <<
         CONFIG_SETTING_MIND_OPENAI_KEY << (c?c->getWingmanOpenAiApiKey():"") << endl <<
         "    * OpenAI API key generated at https://platform.openai.com/api-keys to be used by Wingman as LLM provider" << endl <<
         endl <<

         "# " << CONFIG_SECTION_APP << endl <<
         "Application settings:" << endl <<
         endl <<
         CONFIG_SETTING_STARTUP_VIEW_LABEL << (c?c->getStartupView():Configuration::DEFAULT_STARTUP_VIEW_NAME) << endl <<
         "    * Examples: outlines, tags, recent, home" << endl <<
         CONFIG_SETTING_UI_THEME_LABEL << (c?c->getUiThemeName():Configuration::DEFAULT_UI_THEME_NAME) << endl <<
         "    * Examples: dark, light, native" << endl <<
         CONFIG_SETTING_UI_APP_FONT_SIZE << (c?c->getUiAppFontSize():Configuration::DEFAULT_UI_APP_FONT_SIZE) << endl <<
         "    * Examples: 0 (default - auto), 10, 42" << endl <<
         CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL << (c?c->getUiHtmlCssPath():Configuration::DEFAULT_UI_HTML_CSS_THEME) << endl <<
         "    * Normal themes (dark, light) style HTML generated from Markdown," << endl <<
         "      while raw theme shows syntax-highlighted Markdown only. You can" << endl <<
         // ... well I know this is potential security hole, but I believe MF users have good intentions
         "      also specify path to any CSS file to be used." << endl <<
         "    * Examples: qrc:/html-css/light.css, qrc:/html-css/dark.css, raw, /home/user/my-custom-mf-style.css" << endl <<
         CONFIG_SETTING_UI_HTML_ZOOM_LABEL << (c?c->getUiHtmlZoom():Configuration::DEFAULT_UI_HTML_ZOOM) << endl <<
         "    * Zoom factor of HTML view generated for Markdown. Zoom value can be between 25 and 500" << endl <<
         "      Where 25 is 25%, 100 is 100% and 500 is 500% zoom." << endl <<
         "    * Examples: 25, 100, 300" << endl <<
         CONFIG_SETTING_UI_EXPERT_MODE_LABEL << (c?(c->isUiExpertMode()?"yes":"no"):(Configuration::DEFAULT_UI_EXPERT_MODE?"yes":"no")) << endl <<
         "    * Hide expendable buttons - experts use keyboard shortcuts as they are faster." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_LIVE_NOTE_PREVIEW_LABEL << (c?(c->isUiLiveNotePreview()?"yes":"no"):(Configuration::DEFAULT_UI_LIVE_NOTE_PREVIEW?"yes":"no")) << endl <<
         "    * Show notebook/note HTML preview as you write Markdown." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_OS_TABLE_SORT_COL_LABEL << (c?c->getUiOsTableSortColumn():Configuration::DEFAULT_OS_TABLE_SORT_COLUMN) << endl <<
         "    * Column (index) to be used for Notebooks table sorting - value between 0 and " << Configuration::DEFAULT_OS_TABLE_SORT_COLUMN << "." << endl <<
         "    * Examples: 0, 5, 7" << endl <<
         CONFIG_SETTING_UI_OS_TABLE_SORT_ORDER_LABEL << (c?(c->isUiOsTableSortOrder()?"ascending":"descending"):(Configuration::DEFAULT_OS_TABLE_SORT_ORDER?"ascending":"descending")) << endl <<
         "    * Order of Notebooks table sorting." << endl <<
         "    * Examples: ascending, descending" << endl <<
         CONFIG_SETTING_UI_SHOW_TOOLBAR_LABEL<< (c?(c->isUiShowToolbar()?"yes":"no"):(Configuration::DEFAULT_UI_SHOW_TOOLBAR?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_NERD_MENU << (c?(c->isUiNerdTargetAudience()?"yes":"no"):(Configuration::DEFAULT_UI_NERD_MENU?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_TABS_AS_SPACES_LABEL << (c?(c->isUiEditorTabsAsSpaces()?"yes":"no"):(Configuration::DEFAULT_EDITOR_TABS_AS_SPACES?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_AUTOSAVE_LABEL << (c?(c->isUiEditorAutosave()?"yes":"no"):(Configuration::DEFAULT_EDITOR_AUTOSAVE?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_CLICK_NOTE_VIEW_TO_EDIT_LABEL << (c?(c->isUiDoubleClickNoteViewToEdit()?"yes":"no"):(Configuration::DEFAULT_CLICK_NOTE_VIEW_TO_EDIT?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_EXTERNAL_EDITOR_CMD_LABEL << (c?c->getExternalEditorCmd():"") << endl <<
         "    * Command to run external Markdown editor. Filename with Markdown is appended at the end." << endl <<
         "    * Example: emacs -nw" << endl <<
         "    * Example: vi" << endl <<
         CONFIG_SETTING_UI_EDITOR_KEY_BINDING_LABEL << (c?c->getEditorKeyBindingAsString():Configuration::editorKeyBindingToString(Configuration::EditorKeyBindingMode::WINDOWS)) << endl <<
         "    * Examples: emacs, vim, windows" << endl <<
         CONFIG_SETTING_UI_EDITOR_FONT_LABEL << (c?c->getEditorFont():Configuration::DEFAULT_EDITOR_FONT) << endl <<
         "    * Examples: " << Configuration::DEFAULT_EDITOR_FONT << endl <<
         CONFIG_SETTING_UI_EDITOR_LIVE_SPELLCHECK_LABEL << (c?(c->isUiEditorLiveSpellCheck()?"yes":"no"):(Configuration::DEFAULT_SPELLCHECK_LIVE?"yes":"no")) << endl <<
         "    * Enable live spell check in Notebook and Note description editor." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_SPELLCHECK_LANG_LABEL<< (c?c->getUiEditorSpellCheckDefaultLanguage():"") << endl <<
         "    * Set prefered spell check language in Notebook and Note description editor." << endl <<
         "    * Examples: en_US" << endl <<
         CONFIG_SETTING_UI_EDITOR_SYNTAX_HIGHLIGHT_LABEL << (c?(c->isUiEditorEnableSyntaxHighlighting()?"yes":"no"):(Configuration::DEFAULT_EDITOR_SYNTAX_HIGHLIGHT?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_AUTOCOMPLETE_LABEL << (c?(c->isUiEditorEnableAutocomplete()?"yes":"no"):(Configuration::DEFAULT_EDITOR_AUTOCOMPLETE?"yes":"no")) << endl <<
         "    * Enable text autocomplete." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_SMART_EDITOR_LABEL << (c?(c->isUiEditorEnableSmartEditor()?"yes":"no"):(Configuration::DEFAULT_EDITOR_SMART_EDITOR?"yes":"no")) << endl <<
         "    * Enable numbered and buletted list, blocks and [({`_ autocomplete." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_SPACE_SECTION_ESCAPING_LABEL << (c?(c->isUiEditorSpaceSectionEscaping()?"yes":"no"):(Configuration::DEFAULT_EDITOR_SPACE_SECTION_ESCAPING?"yes":"no")) << endl <<
         "    * Enable SPACE-based # in section espacing (HTML entity used when disabled)." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_TAB_WIDTH_LABEL << (c?c->getUiEditorTabWidth():Configuration::DEFAULT_EDITOR_TAB_WIDTH) << endl <<
         "    * Examples: 4, 8" << endl <<
         CONFIG_SETTING_SAVE_READS_METADATA_LABEL << (c?(c->isSaveReadsMetadata()?"yes":"no"):(Configuration::DEFAULT_SAVE_READS_METADATA?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_MD_HIGHLIGHT_LABEL << (c?(c->isUiEnableSrcHighlightInMd()?"yes":"no"):(Configuration::DEFAULT_MD_HIGHLIGHT?"yes":"no")) << endl <<
         "    * Enable offline Highlight JavaScript library to show source code with syntax highlighting in HTML generated from Markdown." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_MD_MATH_LABEL << (c?(c->isUiEnableMathInMd()?"yes":"no"):(Configuration::DEFAULT_MD_MATH?"yes":"no")) << endl <<
         "    * Enable online MathJax JavaScript library to show math expressions in HTML generated from Markdown." << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_MD_DIAGRAM_LABEL << (c?c->getJsLibSupportAsString(c->getUiEnableDiagramsInMd()):UI_JS_LIB_NO) << endl <<
         "    * Enable online Mermaid JavaScript library to show diagrams in HTML generated from Markdown." << endl <<
         "    * Examples: online, no" << endl <<
         CONFIG_SETTING_NAVIGATOR_MAX_GRAPH_NODES_LABEL << (c?c->getNavigatorMaxNodes():Configuration::DEFAULT_NAVIGATOR_MAX_GRAPH_NODES) << endl <<
         "    * Maximum number of knowledge graph navigator nodes (performance vs. readability trade-off)." << endl <<
         "    * Examples: 150" << endl <<
         endl <<

         "# " << CONFIG_SECTION_REPOSITORIES << endl <<
         "If MindForger detects MindForger repository structure, then the directory is" << endl <<
         "threated as knowledge base, else it's used as a Markdown directory (e.g. metadata"  << endl <<
         "are not stored to Markdown files)." << endl <<
         endl <<
         CONFIG_SETTING_ACTIVE_REPOSITORY_LABEL << (c&&c->getActiveRepository()?c->getActiveRepository()->getPath():Configuration::DEFAULT_ACTIVE_REPOSITORY_PATH) <<
         endl;
    if(c) {
        for(auto& r:c->getRepositories()) {
            s << CONFIG_SETTING_REPOSITORY_LABEL << r.first << endl;
        }
    } else {
        s << CONFIG_SETTING_REPOSITORY_LABEL << Configuration::DEFAULT_ACTIVE_REPOSITORY_PATH << endl;
    }
    s << endl;

    md.assign(s.str());

    return md;
}

bool MarkdownConfigurationRepresentation::load(Configuration& c)
{
    MF_DEBUG("Loading configuration from " << c.getConfigFilePath() << endl);
    string file{c.getConfigFilePath().c_str()};
    if(isFile(file.c_str())) {
        MarkdownDocument md{&file};
        md.from();
        vector<MarkdownAstNodeSection*>* ast = md.moveAst();
        configuration(ast, c);

        return true;
    } else {
        return false;
    }
}

void MarkdownConfigurationRepresentation::save(const File* file, Configuration* c)
{
#ifdef DO_MF_DEBUG
    if(c) {
        MF_DEBUG(
            "Saving configuration as MD:" << endl <<
            "  path: '" << c->getConfigFilePath() << "'" << endl <<
            "  repo: " << boolalpha << c->hasRepositoryConfiguration() << endl <<
            "  repo path: " << c->getRepositoryConfigFilePath() << "'" << endl
        );
        if(c->hasRepositoryConfiguration()) {
            MF_DEBUG(
                "  os: " << c->getRepositoryConfiguration().getOrganizers().size() << endl
            );
        }
    }
#endif

    string md{};
    to(c,md);

    if(c) {
        MF_DEBUG("Saving configuration to file " << c->getConfigFilePath() << endl);
        std::ofstream out(c->getConfigFilePath());
        out << md;
        out.close();

        // repository configuration path is available only if MF in repository mode
        if(c->hasRepositoryConfiguration()) {
            MF_DEBUG("Saving repository configuration to " << c->getRepositoryConfigFilePath() << endl);
            MarkdownRepositoryConfigurationRepresentation repositorCfgMd{};
            repositorCfgMd.save(*c);
        }
    } else if(file) {
        MF_DEBUG("Saving configuration to FILE " << file->getName() << endl);
        std::ofstream out(file->getName());
        out << md;
        out.close();
    } else {
        MF_DEBUG("WARNING: configuration NOT saved - either configuration instance and/ro file name is not available" << endl);
    }
}

} // m8r namespace

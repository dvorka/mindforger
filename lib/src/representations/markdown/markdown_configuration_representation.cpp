/*
 markdown_configuration_representation.cpp     MindForger thinking notebook

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

#include "markdown_configuration_representation.h"

namespace m8r {

constexpr const auto CONFIG_SECTION_APP = "Application";
constexpr const auto CONFIG_SECTION_MIND = "Mind";
constexpr const auto CONFIG_SECTION_REPOSITORIES= "Repositories";

// application
constexpr const auto CONFIG_SETTING_UI_THEME_LABEL = "* Theme: ";
constexpr const auto CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL = "* Markdown CSS theme: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_KEY_BINDING_LABEL =  "* Editor key binding: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_FONT_LABEL =  "* Editor font: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_SYNTAX_HIGHLIGHT_LABEL =  "* Editor syntax highlighting: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_AUTOCOMPLETE_LABEL =  "* Editor autocomplete: ";
constexpr const auto CONFIG_SETTING_UI_EDITOR_TAB_WIDTH_LABEL =  "* Editor TAB width: ";
constexpr const auto CONFIG_SETTING_MD_HIGHLIGHT_LABEL = "* Enable source code syntax highlighting support in Markdown: ";
constexpr const auto CONFIG_SETTING_MD_MATH_LABEL = "* Enable math support in Markdown: ";
constexpr const auto CONFIG_SETTING_MD_DIAGRAM_LABEL = "* Enable diagram support in Markdown: ";
constexpr const auto CONFIG_SETTING_SAVE_READS_METADATA_LABEL = "* Save reads metadata: ";

// mind
constexpr const auto CONFIG_SETTING_MIND_STATE = "* Mind state: ";
constexpr const auto CONFIG_SETTING_MIND_TIME_SCOPE_LABEL = "* Time scope: ";
constexpr const auto CONFIG_SETTING_MIND_TAGS_SCOPE_LABEL = "* Tags scope: ";
constexpr const auto CONFIG_SETTING_MIND_DISTRIBUTOR_INTERVAL = "* Async refresh interval (ms): ";

// repositories
constexpr const auto CONFIG_SETTING_ACTIVE_REPOSITORY_LABEL = "* Active repository: ";
constexpr const auto CONFIG_SETTING_REPOSITORY_LABEL = "* Repository: ";

using namespace std;

MarkdownConfigurationRepresentation::MarkdownConfigurationRepresentation()
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
            MarkdownAstNodeSection* astNode = ast->at(off);

            // skip configuration's preamble (if present)
            if(astNode->isPreambleSection()) {
                if(ast->size()>1) {
                    astNode = ast->at(++off);
                } else {
                    astNode = nullptr;
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
                configuration(ast->at(i)->getText(), sectionBody, c);

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
void MarkdownConfigurationRepresentation::configuration(string* title, vector<string*>* body, Configuration& c)
{
    if(title && body && title->size() && body->size()) {
        if(!title->compare(CONFIG_SECTION_APP)) {
            MF_DEBUG("PARSING configuration section App" << endl);
            for(string* line: *body) {
                if(line && line->size() && line->at(0)=='*') {
                    if(line->find(CONFIG_SETTING_SAVE_READS_METADATA_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setSaveReadsMetadata(true);
                        } else {
                            c.setSaveReadsMetadata(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_THEME_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_THEME_LABEL));
                        // NOTE: theme name is NOT validated
                        if(t.size()) {
                            c.setUiThemeName(t);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL) != std::string::npos) {
                        string t = line->substr(strlen(CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL));
                        if(t.size()) {
                            // TODO: IMPORTANT - this is potential SECURITY threat - theme name is NOT validated
                            c.setUiHtmlCssPath(t);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_KEY_BINDING_LABEL) != std::string::npos) {
                        if(line->find(UI_EDITOR_KEY_BINDING_EMACS) != std::string::npos) {
                            c.setEditorKeyBinding(Configuration::EditorKeyBindingMode::EMACS);
                        } else if(line->find(UI_EDITOR_KEY_BINDING_WIN) != std::string::npos) {
                            c.setEditorKeyBinding(Configuration::EditorKeyBindingMode::WINDOWS);
                        } else {
                            c.setEditorKeyBinding(Configuration::EditorKeyBindingMode::VIM);
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
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_AUTOCOMPLETE_LABEL) != std::string::npos) {
                        if(line->find("yes") != std::string::npos) {
                            c.setUiEditorEnableAutocomplete(true);
                        } else {
                            c.setUiEditorEnableAutocomplete(false);
                        }
                    } else if(line->find(CONFIG_SETTING_UI_EDITOR_TAB_WIDTH_LABEL) != std::string::npos) {
                        if(line->find("8") != std::string::npos) {
                            c.setUiEditorTabWidth(8);
                        } else {
                            c.setUiEditorTabWidth(4);
                        }
                    }
                }
            }
        } else if(!title->compare(CONFIG_SECTION_MIND)) {
            MF_DEBUG("PARSING configuration section Mind" << endl);
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
                        i %=10000;
                        c.setDistributorSleepInterval(i);
                    }
                }
            }
        } else if(!title->compare(CONFIG_SECTION_REPOSITORIES)) {
            MF_DEBUG("PARSING configuration section Repositories" << endl);
            for(string* line: *body) {
                if(line && line->size() && line->at(0)=='*') {
                    if(line->find(CONFIG_SETTING_ACTIVE_REPOSITORY_LABEL) != std::string::npos) {
                        string p = line->substr(strlen(CONFIG_SETTING_ACTIVE_REPOSITORY_LABEL));
                        if(p.size()) {
                            Repository* r = RepositoryIndexer::getRepositoryForPath(p);
                            if(r) {
                                c.setActiveRepository(c.addRepository(r));
                            } else {
                                cerr << "Unable to construct active repository for non-existing configured path: '" << p << "'" << endl;
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
                                cerr << "Unable to construct repository for non-existing configured path: '" << p << "'" << endl;
                            }
                        } else {
                            cerr << "Unable to construct a configured repository as path is empty" << endl;
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
         "    * Examples: important (shown Notebooks must be tagged with 'important;); if no tag is specified, then tags scope is disabled" << endl <<
         CONFIG_SETTING_MIND_DISTRIBUTOR_INTERVAL << (c?c->getDistributorSleepInterval():Configuration::DEFAULT_DISTRIBUTOR_SLEEP_INTERVAL+1) << endl <<
         "    * Sleep interval (miliseconds) between asynchronous mind-related evaluations (associations, ...)" << endl <<
         "    * Examples: 3000, 5000, 10000" << endl <<
         endl <<

         "# " << CONFIG_SECTION_APP << endl <<
         "Application settings:" << endl <<
         endl <<
         CONFIG_SETTING_UI_THEME_LABEL << (c?c->getUiThemeName():Configuration::DEFAULT_UI_THEME_NAME) << endl <<
         "    * Examples: dark, light" << endl <<
         CONFIG_SETTING_UI_HTML_CSS_THEME_LABEL << (c?c->getUiHtmlCssPath():Configuration::DEFAULT_UI_HTML_CSS_THEME) << endl <<
         "    * Normal themes (dark, light) style HTML generated from Markdown," << endl <<
         "      while raw theme shows syntax-highlighted Markdown only. You can" << endl <<
         // ... well I know this is potential security hole, but I believe MF users have good intentions
         "      also specify path to any CSS file to be used." << endl <<
         "    * Examples: qrc:/html-css/light.css, qrc:/html-css/dark.css, raw, /home/user/my-custom-mf-style.css" << endl <<
         CONFIG_SETTING_UI_EDITOR_KEY_BINDING_LABEL << (c?c->getEditorKeyBindingAsString():Configuration::DEFAULT_EDITOR_KEY_BINDING) << endl <<
         "    * Examples: emacs, vim, windows" << endl <<
         CONFIG_SETTING_UI_EDITOR_FONT_LABEL << (c?c->getEditorFont():Configuration::DEFAULT_EDITOR_FONT) << endl <<
         "    * Examples: Courier New,12" << endl <<
         CONFIG_SETTING_UI_EDITOR_SYNTAX_HIGHLIGHT_LABEL << (c?(c->isUiEditorEnableSyntaxHighlighting()?"yes":"no"):(Configuration::DEFAULT_EDITOR_SYNTAX_HIGHLIGHT?"yes":"no")) << endl <<
         "    * Examples: yes, no" << endl <<
         CONFIG_SETTING_UI_EDITOR_AUTOCOMPLETE_LABEL << (c?(c->isUiEditorEnableAutocomplete()?"yes":"no"):(Configuration::DEFAULT_EDITOR_AUTOCOMPLETE?"yes":"no")) << endl <<
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
         "    * Enable online or offline Mermaid JavaScript library to show diagrams in HTML generated from Markdown." << endl <<
         "    * Examples: online, offline, no" << endl <<
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
    string md{};
    to(c,md);

    if(c) {
        MF_DEBUG("Saving configuration to file " << c->getConfigFilePath() << endl);
        std::ofstream out(c->getConfigFilePath());
        out << md;
        out.close();
    } else {
        MF_DEBUG("Saving configuration to File " << file->getName() << endl);
        std::ofstream out(file->getName());
        out << md;
        out.close();
    }
}

} // m8r namespace

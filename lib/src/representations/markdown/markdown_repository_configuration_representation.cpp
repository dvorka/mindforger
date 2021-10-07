/*
 markdown_configuration_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "markdown_repository_configuration_representation.h"

namespace m8r {

constexpr const auto CONFIG_SECTION_ORGANIZERS = "Organizers";

// organizers
constexpr const auto CONFIG_SETTING_ORG_NAME = "Organizer name: ";
constexpr const auto CONFIG_SETTING_ORG_KEY= "* Key: ";
constexpr const auto CONFIG_SETTING_ORG_TAG_UR = "* Upper right tag: ";
constexpr const auto CONFIG_SETTING_ORG_TAG_LR = "* Lower right tag: ";
constexpr const auto CONFIG_SETTING_ORG_TAG_LL = "* Lower left tag: ";
constexpr const auto CONFIG_SETTING_ORG_TAG_UL = "* Upper left tag: ";
constexpr const auto CONFIG_SETTING_ORG_FILTER_BY = "* Filter by: ";
constexpr const auto CONFIG_SETTING_ORG_SORT_BY = "* Sort by: ";
constexpr const auto CONFIG_SETTING_ORG_SCOPE = "* Outline scope: ";

using namespace std;

MarkdownRepositoryConfigurationRepresentation::MarkdownRepositoryConfigurationRepresentation()
{    
}

MarkdownRepositoryConfigurationRepresentation::~MarkdownRepositoryConfigurationRepresentation()
{
}

/*
 * Parse Configuration represented as AST and delete AST. Aim to be
 * as ROBUST as possible ~ handle eventual user typos and incorrect
 * formatting.
 */
void MarkdownRepositoryConfigurationRepresentation::repositoryConfiguration(vector<MarkdownAstNodeSection*>* ast, Configuration& c)
{
    c.getRepositoryConfiguration().clear();

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
                repositoryConfigurationSection(ast->at(i)->getText(), sectionBody, c);

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
 * Parse a section of the configuration.
 */
void MarkdownRepositoryConfigurationRepresentation::repositoryConfigurationSection(
    string* title,
    vector<string*>* body,
    Configuration& c
) {
    if(title && body && title->size() && body->size()) {
        if(!title->compare(CONFIG_SECTION_ORGANIZERS)) {
            MF_DEBUG("PARSING configuration section: Organizers" << endl);
            repositoryConfigurationSectionOrganizers(body, c);
        }
    }
}

/**
 * @brief Parse organizer(s) from MD section.
 *
 * MD section syntax is designed so that it can be included either
 * in .mindforger.md or in another configuration file.
 *
 * @example
 * # Organizers
 * Organizer name: MY GLOBAL ORGANIZER
 * * Upper right tag: UR TAG
 * * Lower right tag: LR TAG
 * * Lower left tag: LL TAG
 * * Upper left tag: UL TAG
 * * Sort by: importance
 * * Filter by: outlines and notes
 * * Outline scope:
 * ...
 * Organizer name: MY O ORGANIZER
 * * Upper right tag: UR TAG
 * * Lower right tag: LR TAG
 * * Lower left tag: LL TAG
 * * Upper left tag: UL TAG
 * * Sort by: urgency
 * * Filter by: notes
 * * Outline scope: /home/dvorka/mf/memory/target-outline.md
 *
 * MD section is split using organizer name row(s).
 */
void MarkdownRepositoryConfigurationRepresentation::repositoryConfigurationSectionOrganizers(
    vector<string*>* body, Configuration& c
) {
    set<string> keys{};
    if(body) {
        Organizer* o = nullptr;
        string tags{};
        for(string* line:*body) {
            if(line) {
                if(line && line->find(CONFIG_SETTING_ORG_NAME) != std::string::npos) {
                    // add PREVIOUS Organizer (if available) so that it's not rewritten
                    o = repositoryConfigurationSectionOrganizerAdd(o, keys, c);

                    // new organizer
                    string name{line->substr(strlen(CONFIG_SETTING_ORG_NAME))};
                    if(name.length()) {
                        o = new Organizer(name);
                    } else {
                        o = new Organizer("Custom Organizer");
                    }
                } else if(o && line->find(CONFIG_SETTING_ORG_KEY) != std::string::npos) {
                    o->setKey(line->substr(strlen(CONFIG_SETTING_ORG_KEY)));
                } else if(o && line->find(CONFIG_SETTING_ORG_TAG_UR) != std::string::npos) {
                    tags = line->substr(strlen(CONFIG_SETTING_ORG_TAG_UR));
                    o->tagsUrQuadrant = Organizer::tagsFromString(tags);
                } else if(o && line->find(CONFIG_SETTING_ORG_TAG_LR) != std::string::npos) {
                    tags = line->substr(strlen(CONFIG_SETTING_ORG_TAG_LR));
                    o->tagsLrQuadrant = Organizer::tagsFromString(tags);
                } else if(o && line->find(CONFIG_SETTING_ORG_TAG_LL) != std::string::npos) {
                    tags = line->substr(strlen(CONFIG_SETTING_ORG_TAG_LL));
                    o->tagsLlQuadrant = Organizer::tagsFromString(tags);
                } else if(o && line->find(CONFIG_SETTING_ORG_TAG_UL) != std::string::npos) {
                    tags = line->substr(strlen(CONFIG_SETTING_ORG_TAG_UL));
                    o->tagsUlQuadrant = Organizer::tagsFromString(tags);
                } else if(o && line->find(CONFIG_SETTING_ORG_SORT_BY) != std::string::npos) {
                    string sortBy{line->substr(strlen(CONFIG_SETTING_ORG_SORT_BY))};
                    if(Organizer::CONFIG_VALUE_SORT_BY_I == sortBy) {
                        o->sortBy = Organizer::SortBy::IMPORTANCE;
                    } else if(Organizer::CONFIG_VALUE_SORT_BY_U == sortBy) {
                        o->sortBy = Organizer::SortBy::URGENCY;
                    } else {
                        o->sortBy = Organizer::SortBy::IMPORTANCE;
                    }
                } else if(o && line->find(CONFIG_SETTING_ORG_FILTER_BY) != std::string::npos) {
                    string filterBy{line->substr(strlen(CONFIG_SETTING_ORG_FILTER_BY))};
                    if(Organizer::CONFIG_VALUE_FILTER_BY_O == filterBy) {
                        o->filterBy = Organizer::FilterBy::OUTLINES;
                    } else if(Organizer::CONFIG_VALUE_FILTER_BY_N == filterBy) {
                        o->filterBy = Organizer::FilterBy::NOTES;
                    } else if(Organizer::CONFIG_VALUE_FILTER_BY_O_N == filterBy) {
                        o->filterBy = Organizer::FilterBy::OUTLINES_NOTES;
                    } else {
                        o->filterBy = Organizer::FilterBy::OUTLINES_NOTES;
                    }
                } else if(o && line->find(CONFIG_SETTING_ORG_SCOPE) != std::string::npos) {
                    // validity of O ID will be checked (and fixed) on organizer load
                    o->scopeOutlineId = line->substr(strlen(CONFIG_SETTING_ORG_SCOPE));
                }
            }
        }

        // add (valid) organizer
        o = repositoryConfigurationSectionOrganizerAdd(o, keys, c);
    }
}

Organizer* MarkdownRepositoryConfigurationRepresentation::repositoryConfigurationSectionOrganizerAdd(
    Organizer* o,
    set<string>& keys,
    Configuration& c
) {
    if(o) {
        // validate organizer integrity
        if(o->getKey().empty()) {
            o->setKey(
                Organizer::createOrganizerKey(
                    keys,
                    c.getMemoryPath(),
                    Thing::getNextKey(),
                    FILE_PATH_SEPARATOR
                )
            );
        }
        set<string>::iterator it = keys.find(o->getKey());
        if(it != keys.end()) {
            cerr << "Error: skipping '" << o->getName() << "' organizer as another organizer "
                 << "with key '" << o->getKey() << "' is already defined" << endl;
            delete o;
            return nullptr;
        } // else OK - key not defined yet

        if(
            !o->tagsUrQuadrant.size()
            || !o->tagsLrQuadrant.size()
            || !o->tagsLlQuadrant.size()
            || !o->tagsUlQuadrant.size()
        ) {
            cerr << "Error: skipping '" << o->getName() << "' organizer as it does not define tags for all quandrants" << endl;
            delete o;
            return nullptr;
        }

        // persist
        if(o && c.hasRepositoryConfiguration()) {
            c.getRepositoryConfiguration().addOrganizer(o);
            keys.insert(o->getKey());
        }
    }

    return o;
}

string* MarkdownRepositoryConfigurationRepresentation::to(Configuration& c)
{
    string* md = new string{};
    to(&c, *md);
    return md;
}

string& MarkdownRepositoryConfigurationRepresentation::to(Configuration* c, string& md)
{
    stringstream s{};
    string os{};
    string timeScopeAsString{}, tagsScopeAsString{}, mindStateAsString{"sleep"};
    if(c) {
        // organizers
        stringstream oss{};
        if(c->hasRepositoryConfiguration() && c->getRepositoryConfiguration().getOrganizers().size()) {
            for(Organizer* o:c->getRepositoryConfiguration().getOrganizers()) {
                oss
                << CONFIG_SETTING_ORG_NAME << o->getName() << endl
                << CONFIG_SETTING_ORG_KEY << o->getKey() << endl
                << CONFIG_SETTING_ORG_TAG_UR << Organizer::tagsToString(o->getUpperRightTags()) << endl
                << CONFIG_SETTING_ORG_TAG_LR << Organizer::tagsToString(o->getLowerRightTags()) << endl
                << CONFIG_SETTING_ORG_TAG_LL << Organizer::tagsToString(o->getLowerLeftTags()) << endl
                << CONFIG_SETTING_ORG_TAG_UL << Organizer::tagsToString(o->getUpperLeftTags()) << endl
                << CONFIG_SETTING_ORG_SORT_BY << o->getSortByAsStr() << endl
                << CONFIG_SETTING_ORG_FILTER_BY << o->getFilterByAsStr() << endl
                << CONFIG_SETTING_ORG_SCOPE << o->getOutlineScope() << endl
                << endl;
            }
        } else {
            oss << endl;
        }
        os=oss.str();
    }

    // IMPROVE build more in compile time and less in runtime
    s <<
         "# MindForger Repository Configuration" << endl <<
         endl <<
         "This is MindForger **repository** configuration file (Markdown hosted DSL)." << endl <<
         "See documentation for configuration options details." << endl <<
         endl <<

         "# " << CONFIG_SECTION_ORGANIZERS << endl <<
         os

         ;

    s << endl;

    md.assign(s.str());

    return md;
}

bool MarkdownRepositoryConfigurationRepresentation::load(Configuration& c)
{
    MF_DEBUG("Loading repository configuration from: '" << c.getRepositoryConfigFilePath() << "'" << endl);
    string file{c.getRepositoryConfigFilePath().c_str()};
    if(isFile(file.c_str())) {
        MarkdownDocument md{&file};
        md.from();
        vector<MarkdownAstNodeSection*>* ast = md.moveAst();
        repositoryConfiguration(ast, c);
        MF_DEBUG("  Loaded " << c.getRepositoryConfiguration().getOrganizers().size() << " Organizer(s)" << endl);
        return true;
    } else {
        return false;
    }
}

void MarkdownRepositoryConfigurationRepresentation::save(const File* file, Configuration* c)
{
    string md{};
    to(c,md);

    if(c) {
        MF_DEBUG("Saving repository configuration to file " << c->getRepositoryConfigFilePath() << endl);
        std::ofstream out(c->getRepositoryConfigFilePath());
        out << md;
        out.close();
    } else {
        MF_DEBUG("Saving repository configuration to File " << file->getName() << endl);
        std::ofstream out(file->getName());
        out << md;
        out.close();
    }
}

} // m8r namespace

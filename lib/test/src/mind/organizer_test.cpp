/*
 organizer_test.cpp     MindForger test

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

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../test_gear.h"
#include "../../../src/model/organizer.h"
#include "../../../src/representations/markdown/markdown_configuration_representation.h"

using namespace std;

TEST(OrganizerTestCase, ParseSaveAndLoad)
{
    // GIVEN
    string const outlineScope{"/home/mindforger/mf/memory/outline.md"};
    string cfgFile{m8r::platformSpecificPath("/tmp/.mindforger-organizer.md")};
    string repositoryDir{ m8r::platformSpecificPath("/tmp")};
    string repositoryFilename{"custom-repository-organizer-single-file.md"};
    string repositoryPath{repositoryDir};
    repositoryPath += FILE_PATH_SEPARATOR;
    repositoryPath += repositoryFilename;
    std::ofstream out(repositoryPath);
    out << "# Just a Test" << endl;
    out.close();
    string timeScopeAsString{};
    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();

    // WHEN: save configuration file WITH organizers
    c.setConfigFilePath(cfgFile);
    m8r::Repository* r = new m8r::Repository{
        repositoryDir,
        m8r::Repository::RepositoryType::MARKDOWN,
        m8r::Repository::RepositoryMode::FILE,
       repositoryFilename
    };
    c.setActiveRepository(c.addRepository(r));
    // organizers
    m8r::Organizer* o = new m8r::Organizer("My GLOBAL organizer on IMPORTANCE and NOTES");
    o->setUpperRightTag("ur1");
    o->setLowerRightTag("lr1");
    o->setLowerLeftTag("ll1");
    o->setUpperLeftTag("ul1");
    o->sortBy = m8r::Organizer::SortBy::IMPORTANCE;
    o->filterBy = m8r::Organizer::FilterBy::NOTES;
    o->scopeOutlineId.clear(); // global scope
    c.addOrganizer(o);
    o = new m8r::Organizer("My organizer on URGENCY and OUTLINES+NOTES");
    o->setUpperRightTag("ur2");
    o->setLowerRightTag("lr2");
    o->setLowerLeftTag("ll2");
    o->setUpperLeftTag("ul2");
    o->sortBy = m8r::Organizer::SortBy::URGENCY;
    o->filterBy = m8r::Organizer::FilterBy::OUTLINES_NOTES;
    o->scopeOutlineId = outlineScope;
    c.addOrganizer(o);

    configRepresentation.save(c);

    // THEN: assert serialized configuration
    string* asString = m8r::fileToString(c.getConfigFilePath());
    EXPECT_NE(std::string::npos, asString->find("Upper right tag: ur1"));
    EXPECT_NE(std::string::npos, asString->find("Lower right tag: lr1"));
    EXPECT_NE(std::string::npos, asString->find("Lower left tag: ll1"));
    EXPECT_NE(std::string::npos, asString->find("Upper left tag: ul1"));
    EXPECT_NE(std::string::npos, asString->find("Sort by: importance"));
    EXPECT_NE(std::string::npos, asString->find("Filter by: notes"));
    EXPECT_NE(std::string::npos, asString->find("Outline scope: "));
    delete asString;

    // GIVEN load previously saved configuration
    c.setConfigFilePath(cfgFile);

    // WHEN: load previously saved configuration
    bool loaded = configRepresentation.load(c);

    // THEN: assert configuration from loaded file
    ASSERT_TRUE(loaded);
    EXPECT_EQ(2, c.getOrganizers().size());
    EXPECT_EQ("ur1", c.getOrganizers()[0]->getUpperRightTag());
    EXPECT_EQ("lr1", c.getOrganizers()[0]->getLowerRightTag());
    EXPECT_EQ("ll1", c.getOrganizers()[0]->getLowerLeftTag());
    EXPECT_EQ("ul1", c.getOrganizers()[0]->getUpperLeftTag());
    EXPECT_EQ(m8r::Organizer::FilterBy::NOTES, c.getOrganizers()[0]->filterBy);
    EXPECT_EQ(m8r::Organizer::SortBy::IMPORTANCE, c.getOrganizers()[0]->sortBy);
    EXPECT_EQ("", c.getOrganizers()[0]->scopeOutlineId);

    EXPECT_EQ(m8r::Organizer::FilterBy::OUTLINES_NOTES, c.getOrganizers()[1]->filterBy);
    EXPECT_EQ(m8r::Organizer::SortBy::URGENCY, c.getOrganizers()[1]->sortBy);
    EXPECT_EQ(outlineScope, c.getOrganizers()[1]->scopeOutlineId);
}

TEST(OrganizerTestCase, NoOrganizerParseSaveAndLoad)
{
    // GIVEN
    string const outlineScope{"/home/mindforger/mf/memory/outline.md"};
    string cfgFile{m8r::platformSpecificPath("/tmp/.mindforger-no-organizer.md")};
    string repositoryDir{ m8r::platformSpecificPath("/tmp")};
    string repositoryFilename{"custom-repository-no-organizer-single-file.md"};
    string repositoryPath{repositoryDir};
    repositoryPath += FILE_PATH_SEPARATOR;
    repositoryPath += repositoryFilename;
    std::ofstream out(repositoryPath);
    out << "# Just a Test" << endl;
    out.close();
    string timeScopeAsString{};
    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();

    // WHEN: save configuration file WITH organizers
    c.setConfigFilePath(cfgFile);
    m8r::Repository* r = new m8r::Repository{
        repositoryDir,
        m8r::Repository::RepositoryType::MARKDOWN,
        m8r::Repository::RepositoryMode::FILE,
       repositoryFilename
    };
    c.setActiveRepository(c.addRepository(r));
    // NO organizers
    c.clearOrganizers();

    configRepresentation.save(c);

    // THEN: assert serialized configuration
    string* asString = m8r::fileToString(c.getConfigFilePath());
    EXPECT_EQ(std::string::npos, asString->find("Upper right tag: ur"));
    EXPECT_EQ(std::string::npos, asString->find("Lower right tag: lr"));
    EXPECT_EQ(std::string::npos, asString->find("Lower left tag: ll"));
    EXPECT_EQ(std::string::npos, asString->find("Upper left tag: ul"));
    EXPECT_EQ(std::string::npos, asString->find("Sort by: importance"));
    EXPECT_EQ(std::string::npos, asString->find("Filter by: notes"));
    EXPECT_EQ(std::string::npos, asString->find("Outline scope: "));
    delete asString;

    // GIVEN load previously saved configuration
    c.setConfigFilePath(cfgFile);

    // WHEN: load previously saved configuration
    bool loaded = configRepresentation.load(c);

    // THEN: assert configuration from loaded file
    ASSERT_TRUE(loaded);
    EXPECT_EQ(0, c.getOrganizers().size());
}

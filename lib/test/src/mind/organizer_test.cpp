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

#include "../test_utils.h"
#include "../../../src/model/organizer.h"
#include "../../../src/representations/markdown/markdown_configuration_representation.h"

using namespace std;

TEST(OrganizerTestCase, SerializeAndSplitTags)
{
    // GIVEN
    string given_string_tags{"aaa,,bbb,,ccc"};
    m8r::Organizer o{"Test organizer"};

    // WHEN
    std::set<std::string> tags_as_vector = m8r::Organizer::tagsFromString(given_string_tags);
    cout << endl << "Tags vector[" << tags_as_vector.size() << "]";
    for(auto t:tags_as_vector) {
        cout << endl << "  '" << t << "'";
    }
    string tags_as_string = m8r::Organizer::tagsToString(tags_as_vector);
    cout << endl << "Tags serialization: '" << tags_as_string << "'" << endl;

    // THEN
    EXPECT_GT(tags_as_vector.size(), 0);
    EXPECT_EQ(given_string_tags, tags_as_string);

}

TEST(OrganizerTestCase, ParseSaveAndLoad)
{
    // GIVEN
    string const outlineScope{"/home/mindforger/mf/memory/outline.md"};
    // organizers available in MARKDOWN REPOSITORY mode only
    m8r::TestSandbox box{"", true};
    string mdFilename{"custom-repository-organizer-single-file.md"};
    string mdFilePath{box.addMdFile(mdFilename)};
    cout <<
    "Test directories:" << endl <<
    "  home   :" << box.testHomePath << endl <<
    "  MF cfg :" << box.configPath << endl <<
    "  MD file:" << mdFilePath << endl
    ;

    string timeScopeAsString{};
    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();

    // WHEN: save configuration file WITH organizers
    c.setConfigFilePath(box.configPath);
    m8r::Repository* r = new m8r::Repository{
        box.repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        mdFilename
    };
    c.setActiveRepository(c.addRepository(r), repositoryConfigRepresentation);
    // organizers
    m8r::Organizer* o = new m8r::Organizer("My GLOBAL organizer on IMPORTANCE and NOTES");
    o->setUpperRightTag("ur1");
    o->setLowerRightTag("lr1");
    o->setLowerLeftTag("ll1");
    o->setUpperLeftTag("ul1");
    o->sortBy = m8r::Organizer::SortBy::IMPORTANCE;
    o->filterBy = m8r::Organizer::FilterBy::NOTES;
    o->scopeOutlineId.clear(); // global scope
    c.getRepositoryConfiguration().addOrganizer(o);
    o = new m8r::Organizer("My organizer on URGENCY and OUTLINES+NOTES");
    o->setUpperRightTag("ur2");
    o->setLowerRightTag("lr2");
    o->setLowerLeftTag("ll2");
    o->setUpperLeftTag("ul2");
    o->sortBy = m8r::Organizer::SortBy::URGENCY;
    o->filterBy = m8r::Organizer::FilterBy::OUTLINES_NOTES;
    o->scopeOutlineId = outlineScope;
    c.getRepositoryConfiguration().addOrganizer(o);

    cout << "Test step: SAVE CONFIGURATION" << endl;
    configRepresentation.save(c);

    // THEN: assert serialized configuration
    cout << "Test step: LOADING REPOSITORY CONFIGURATION from: '"
         << c.getRepositoryConfigFilePath()
         << "'"
         << endl;
    ASSERT_TRUE(c.getRepositoryConfigFilePath().size());
    string* asString = m8r::fileToString(c.getRepositoryConfigFilePath());
    EXPECT_NE(std::string::npos, asString->find("Upper right tag: ur1"));
    EXPECT_NE(std::string::npos, asString->find("Lower right tag: lr1"));
    EXPECT_NE(std::string::npos, asString->find("Lower left tag: ll1"));
    EXPECT_NE(std::string::npos, asString->find("Upper left tag: ul1"));
    EXPECT_NE(std::string::npos, asString->find("Sort by: importance"));
    EXPECT_NE(std::string::npos, asString->find("Filter by: notes"));
    EXPECT_NE(std::string::npos, asString->find("Outline scope: "));
    delete asString;

    // GIVEN load previously saved configuration
    c.setConfigFilePath(box.configPath);
    cout << "Test step: loading configuration from:" << endl <<
            "  '" << c.getConfigFilePath() << "' (config)" << endl <<
            "  '" << box.configPath << "' (expected)" << endl
    ;

    // WHEN: load previously saved configuration
    bool loaded = configRepresentation.load(c);

    // THEN: assert configuration from loaded file
    ASSERT_TRUE(loaded);
    EXPECT_EQ(2+1, c.getRepositoryConfiguration().getOrganizers().size());  // 2x custom + 1x default
    cout << "Organizer[0]: " << c.getRepositoryConfiguration().getOrganizers()[0]->getName() << endl;
    cout << "Organizer[1]: " << c.getRepositoryConfiguration().getOrganizers()[1]->getName() << endl;
    cout << "Organizer[2]: " << c.getRepositoryConfiguration().getOrganizers()[2]->getName() << endl;
    EXPECT_EQ("ur1", *c.getRepositoryConfiguration().getOrganizers()[1]->getUpperRightTags().begin());
    EXPECT_EQ("lr1", *c.getRepositoryConfiguration().getOrganizers()[1]->getLowerRightTags().begin());
    EXPECT_EQ("ll1", *c.getRepositoryConfiguration().getOrganizers()[1]->getLowerLeftTags().begin());
    EXPECT_EQ("ul1", *c.getRepositoryConfiguration().getOrganizers()[1]->getUpperLeftTags().begin());
    EXPECT_EQ(m8r::Organizer::FilterBy::NOTES, c.getRepositoryConfiguration().getOrganizers()[1]->filterBy);
    EXPECT_EQ(m8r::Organizer::SortBy::IMPORTANCE, c.getRepositoryConfiguration().getOrganizers()[1]->sortBy);
    EXPECT_EQ("", c.getRepositoryConfiguration().getOrganizers()[1]->scopeOutlineId);

    EXPECT_EQ(m8r::Organizer::FilterBy::OUTLINES_NOTES, c.getRepositoryConfiguration().getOrganizers()[2]->filterBy);
    EXPECT_EQ(m8r::Organizer::SortBy::URGENCY, c.getRepositoryConfiguration().getOrganizers()[2]->sortBy);
    EXPECT_EQ(outlineScope, c.getRepositoryConfiguration().getOrganizers()[2]->scopeOutlineId);
}

TEST(OrganizerTestCase, NoMindForgerRepositoryNoOrganizer)
{
    // GIVEN
    m8r::TestSandbox box{".mindforger-no-organizer.md"};
    string mdFilename{"custom-repository-no-organizer-single-file.md"};
    string mdFilePath{box.addMdFile(mdFilename)};
    cout <<
    "Test directories:" << endl <<
    "  home   :" << box.testHomePath << endl <<
    "  MF cfg :" << box.configPath << endl <<
    "  MD file:" << mdFilePath << endl
    ;

    string timeScopeAsString{};
    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();

    // WHEN: save configuration file WITH organizers AND there is NO repository (config)
    c.setConfigFilePath(box.configPath);
    m8r::Repository* r = new m8r::Repository{
        box.repositoryPath,
        m8r::Repository::RepositoryType::MARKDOWN,
        m8r::Repository::RepositoryMode::FILE,
        box.configFileName
    };
    c.setActiveRepository(c.addRepository(r), repositoryConfigRepresentation);
    // NO organizers
    c.getRepositoryConfiguration().clearOrganizers();

    configRepresentation.save(c);

    // THEN: assert serialized configuration
    string* asString = m8r::fileToString(c.getConfigFilePath());
    EXPECT_EQ(std::string::npos, asString->find("Upper right tag: ur"));
    EXPECT_EQ(std::string::npos, asString->find("Sort by: importance"));
    EXPECT_EQ(std::string::npos, asString->find("Filter by: notes"));
    delete asString;

    // GIVEN load previously saved configuration
    c.setConfigFilePath(box.configPath);

    // WHEN: load previously saved configuration
    bool loaded = configRepresentation.load(c);

    // THEN: assert configuration from loaded file
    ASSERT_TRUE(loaded);
    EXPECT_FALSE(c.hasRepositoryConfiguration());
    ASSERT_EQ(0, c.getRepositoryConfiguration().getOrganizers().size());
}


TEST(OrganizerTestCase, DefaultOrganizerParseSaveAndLoad)
{
    // GIVEN
    m8r::TestSandbox box{".mindforger-default-organizer.md", false};
    string mdFilename{"custom-repository-no-organizer-single-file-repo.md"};
    string mdFilePath{box.addMdFile(mdFilename)};
    cout <<
    "Test directories:" << endl <<
    "  home   :" << box.testHomePath << endl <<
    "  MF cfg :" << box.configPath << endl <<
    "  MD file:" << mdFilePath << endl
    ;

    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();

    // WHEN: save configuration file WITH organizers
    c.setConfigFilePath(box.configPath);
    m8r::Repository* r = new m8r::Repository{
        box.repositoryPath,
        m8r::Repository::RepositoryType::MARKDOWN,
        m8r::Repository::RepositoryMode::REPOSITORY,
        box.configFileName
    };
    c.setActiveRepository(c.addRepository(r), repositoryConfigRepresentation);
    // NO organizers (even Eisenhower Matrix organizer not present)
    c.getRepositoryConfiguration().clearOrganizers();

    configRepresentation.save(c);

    // THEN: assert serialized configuration
    cout <<
    "Configuration saved:" << endl <<
    "  Config           : '" << c.getConfigFilePath() << "'" << endl <<
    "  Repository config: '" << c.getRepositoryConfigFilePath() << "'" << endl
    ;
    // THEN: ensure organizer configuration no longer presents in global configuration
    string* asString = m8r::fileToString(c.getConfigFilePath());
    EXPECT_EQ(std::string::npos, asString->find("Upper right tag: ur"));
    EXPECT_EQ(std::string::npos, asString->find("Lower right tag: lr"));
    EXPECT_EQ(std::string::npos, asString->find("Lower left tag: ll"));
    EXPECT_EQ(std::string::npos, asString->find("Upper left tag: ul"));
    EXPECT_EQ(std::string::npos, asString->find("Sort by: importance"));
    EXPECT_EQ(std::string::npos, asString->find("Filter by: notes"));
    EXPECT_EQ(std::string::npos, asString->find("Outline scope: "));
    delete asString;
    ASSERT_FALSE(c.hasRepositoryConfiguration());
    ASSERT_FALSE(c.hasRepositoryConfiguration());
    ASSERT_EQ(0, c.getRepositoryConfiguration().getOrganizers().size());

    cout << "-------------------------------------------------" << endl;

    // GIVEN load previously saved configuration
    c.setConfigFilePath(box.configPath);

    // WHEN: load previously saved configuration
    bool loaded = configRepresentation.load(c);

    // THEN: assert configuration from loaded file
    ASSERT_TRUE(loaded);
    // MARKDOWN/REPOSITORY does NOT have repository configuration (ony MINDFORGER/REPOSITORY has)
    ASSERT_FALSE(c.hasRepositoryConfiguration());
    ASSERT_EQ(0, c.getRepositoryConfiguration().getOrganizers().size());
}

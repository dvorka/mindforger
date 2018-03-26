/*
 configuration_test.cpp     MindForger configuration test

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "../../../src/representations/markdown/markdown_configuration_representation.h"

#include <gtest/gtest.h>

extern char* getMindforgerGitHomePath();

using namespace std;

TEST(ConfigurationTestCase, FromConstructor)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));

    cout << endl << "Active repository:" << endl << "  " << config.getActiveRepository()->getDir();
    cout << endl << "Repositories[" << config.getRepositories().size() << "]:";
    for(auto& r:config.getRepositories()) {
        cout << endl << "  " << r.first;
    }
    cout << endl;

    EXPECT_GE(config.getRepositories().size(), 1);
}

TEST(ConfigurationTestCase, FromEnvironment)
{
    // set MINDFORGER_REPOSITORY environment variable
    const char* envVarName = "MINDFORGER_REPOSITORY";
    const char* relativeRepositoryPath = "/lib/test/resources/basic-repository";
    char* envVar = new char[strlen(envVarName)+1+strlen(getMindforgerGitHomePath())+strlen(relativeRepositoryPath)+1];
    envVar[0] = 0;
    strcat(envVar, envVarName);
    strcat(envVar, "=");
    strcat(envVar, getMindforgerGitHomePath());
    strcat(envVar, relativeRepositoryPath);
    putenv(envVar);
    cout << "Setting env:" << endl << "  " << envVar;

    m8r::Configuration& config = m8r::Configuration::getInstance();

    // TODO use code from Configuration constructor to get repo from environment

    if(config.getActiveRepository()) {
        cout << endl << "Active repository:" << endl << "  " << config.getActiveRepository()->getDir();
        cout << endl << "Repositories[" << config.getRepositories().size() << "]:";
        for(auto& r:config.getRepositories()) {
            cout << endl << "  " << r.first;
        }
        EXPECT_GE(config.getRepositories().size(), 1);
    } else {
        FAIL() << "Repository environment variable is NOT set!" << endl;
    }
    cout << endl;

    // unset environment variable
    unsetenv(m8r::ENV_VAR_M8R_REPOSITORY);
    delete[] envVar;
}

TEST(ConfigurationTestCase, SaveDefaultConfig)
{
    m8r::File file{"/tmp/.mindforger.md.default"};
    m8r::MarkdownConfigurationRepresentation configRepresentation{};

    configRepresentation.save(file);

    // asserts
    string* asString = m8r::fileToString(file.getName());
    EXPECT_NE(asString->find("Theme: light"), std::string::npos);
    EXPECT_NE(asString->find("Time scope: 0y0m0d0h0m"), std::string::npos);
    EXPECT_NE(asString->find("Show Notebook edit button: yes"), std::string::npos);
    EXPECT_NE(asString->find("Save reads metadata: yes"), std::string::npos);
    EXPECT_NE(asString->find("Active repository: ~/mindforger-repository"), std::string::npos);
    EXPECT_NE(asString->find("Repository: ~/mindforger-repository"), std::string::npos);
    delete asString;
}

TEST(ConfigurationTestCase, SaveAndLoad)
{
    string file{"/tmp/.mindforger.md"};
    string repositoryDir{"/tmp"};
    string repositoryFilename{"custom-repository-single-file.md"};
    string repositoryPath{repositoryDir};
    repositoryPath+="/"; repositoryPath+=repositoryFilename;
    std::ofstream out(repositoryPath);
    out << "# Just a Test" << endl;
    out.close();
    string timeScopeAsString{};
    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();

    string backupFile = c.getConfigFilePath();
    string backupTheme = c.getUiThemeName();
    m8r::TimeScope backupTimeScope = c.getTimeScope();
    bool backupReadsMetadata = c.isSaveReadsMetadata();
    bool backupNotebookButton = c.isUiShowNotebookEditButton();
    m8r::Repository* backupActiveRepository;
    if(c.isActiveRepository()) {
        backupActiveRepository = c.getActiveRepository();
    } else {
        backupActiveRepository = nullptr;
    }

    /*
     * SAVE
     */

    c.setConfigFilePath(file);
    c.setUiThemeName("CRAZYCOLORS");
    m8r::TimeScope ts{1,2,33,4,55};
    c.setTimeScope(ts);
    c.setSaveReadsMetadata(false);
    c.setUiShowNotebookEditButton(false);
    m8r::Repository* r = new m8r::Repository{
        repositoryDir,
        m8r::Repository::RepositoryType::MARKDOWN,
        m8r::Repository::RepositoryMode::FILE,
       repositoryFilename
    };
    c.setActiveRepository(c.addRepository(r));

    configRepresentation.save(c);

    // asserts
    string* asString = m8r::fileToString(c.getConfigFilePath());
    EXPECT_NE(asString->find("Theme: CRAZYCOLORS"), std::string::npos);
    EXPECT_NE(asString->find("Time scope: 1y2m33d4h55m"), std::string::npos);
    EXPECT_NE(asString->find("Show Notebook edit button: no"), std::string::npos);
    EXPECT_NE(asString->find("Save reads metadata: no"), std::string::npos);
    EXPECT_NE(asString->find("Active repository: /tmp/custom-repository-single-file.md"), std::string::npos);
    EXPECT_NE(asString->find("Repository: /tmp/custom-repository-single-file.md"), std::string::npos);
    delete asString;
    // r deleted by configuration destructor

    /*
     * LOAD
     */

    c.setConfigFilePath(file);

    bool loaded = configRepresentation.load(c);

    // asserts
    ASSERT_TRUE(loaded);
    EXPECT_EQ(c.getUiThemeName(), "CRAZYCOLORS");
    c.getTimeScope().toString(timeScopeAsString);
    EXPECT_EQ(timeScopeAsString, "1y2m33d4h55m");
    EXPECT_FALSE(c.isSaveReadsMetadata());
    EXPECT_FALSE(c.isUiShowNotebookEditButton());

    EXPECT_GE(c.getRepositories().size(), 1);
    EXPECT_NE(c.getRepositories().find(repositoryPath), c.getRepositories().end());
    c.getRepositories().find(repositoryPath)->second->print();
    EXPECT_EQ(c.getRepositories().find(repositoryPath)->second->getPATH(), repositoryPath);
    EXPECT_EQ(c.getRepositories().find(repositoryPath)->second->getDir(), repositoryDir);
    EXPECT_EQ(c.getRepositories().find(repositoryPath)->second->getFile(), repositoryFilename);
    EXPECT_EQ(c.getRepositories().find(repositoryPath)->second->getMode(), m8r::Repository::RepositoryMode::FILE);

    EXPECT_NE(c.getActiveRepository(),nullptr);
    c.getActiveRepository()->print();
    EXPECT_EQ(c.getActiveRepository()->getPATH(), repositoryPath);
    EXPECT_EQ(c.getActiveRepository()->getDir(), repositoryDir);
    EXPECT_EQ(c.getActiveRepository()->getFile(), repositoryFilename);
    EXPECT_EQ(c.getActiveRepository()->getMode(), m8r::Repository::RepositoryMode::FILE);
    // r deleted by configuration destructor

    // configuration cleanup
    c.setConfigFilePath(backupFile);
    c.setUiThemeName(backupTheme);
    c.setTimeScope(backupTimeScope);
    c.setSaveReadsMetadata(backupReadsMetadata);
    c.setUiShowNotebookEditButton(backupNotebookButton);
    if(backupActiveRepository) {
        c.setActiveRepository(c.addRepository(backupActiveRepository));
    } else {
        c.setActiveRepository(nullptr);
    }
}

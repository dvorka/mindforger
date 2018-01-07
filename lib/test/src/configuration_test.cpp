/*
 configuration_test.h     MindForger configuration test

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

#include "../../src/config/configuration.h"

#include <gtest/gtest.h>

extern char* getMindforgerGitHomePath(void);

using namespace std;

TEST(ConfigurationTestCase, FromConstructor)
{
    string repository{"/lib/test/resources/basic-repository"};
    repository.insert(0, getMindforgerGitHomePath());

    m8r::Configuration configuration{repository};

    cout << endl << "Active repository:" << endl << "  " << *configuration.getActiveRepository();
    cout << endl << "Repositories[" << configuration.getRepositories().size() << "]:";
    for(const string* r:configuration.getRepositories()) {
        cout << endl << "  " << *r;
    }
    cout << endl;

    EXPECT_EQ(configuration.getRepositories().size(), 1);
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

    m8r::Configuration configuration{};

    if(configuration.getActiveRepository()) {
        cout << endl << "Active repository:" << endl << "  " << *configuration.getActiveRepository();
        cout << endl << "Repositories[" << configuration.getRepositories().size() << "]:";
        for(const string* r:configuration.getRepositories()) {
            cout << endl << "  " << *r;
        }
        EXPECT_EQ(configuration.getRepositories().size(), 1);
    } else {
        FAIL() << "Repository environment variable is NOT set!" << endl;
    }
    cout << endl;

    // unset environment variable
    unsetenv(m8r::ENV_VAR_M8R_REPOSITORY);
    delete[] envVar;
}

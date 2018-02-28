/*
 mindforger_lib_unit_tests.cpp     MindForger library unit tests

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

#include <iostream>
#include <gtest/gtest.h>

using namespace std;

/*
 * GTest Unit testing.
 *
 * https://github.com/google/googletest/blob/master/googletest/docs/Primer.md
 *
 * Remarks:
 *   - use DISABLED_ prefix before test name to temporarily disable it
 *   - specific test runs:
 *     ./mf_unit                                    has no flag, and thus runs all its tests.
 *     ./mf_unit --gtest_list_tests
 *     ./mf_unit --gtest_filter=CppTestCase.Vector  tun CppTestCase.Vector only
 *     ./mf_unit --gtest_filter=*                   also runs everything, due to the single match-everything * value.
 *     ./mf_unit --gtest_filter=FooTest.*           runs everything in test case FooTest.
 */

/**
 * @brief Verify that MINDFORGER_GIT_HOME environment variable is set.
 *
 * This variable is used to access test data, like MF repositories, stored
 * in Git repository.
 */
char* getMindforgerGitHomePath()
{
    return getenv("M8R_GIT_PATH");
}

int main(int argc, char **argv)
{
    if(!getMindforgerGitHomePath()) {
        cout << "M8R_GIT_PATH environment variable must be set to run unit tests!" << endl;
        return 1;
    } else {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
}

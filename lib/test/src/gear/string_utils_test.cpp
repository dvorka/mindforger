/*
 string_utils_test.cpp     MindForger application test

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

#include "../../../src/gear/string_utils.h"

using namespace std;
using namespace m8r;

TEST(StringGearTestCase, StringToNcName)
{
    string s("123 text 456");
    string r = normalizeToNcName(s, '-');
    cout << s << " => " << r << endl;
    ASSERT_EQ(r, "123-text-456");

    s.assign("čeština už je tu!");
    r = normalizeToNcName(s, '-');
    cout << s << " => " << r << endl;
    ASSERT_EQ(r, "---e--tina-u---je-tu-");
}

TEST(StringGearTestCase, Split)
{
    char *s, **r;
    int i;

    s = (char*)"a;bb;ccc;";
    r = stringSplit(s, ';');
    cout << "split('" << s << "') = split[]" << endl;
    i = 0;
    while(r[i]) {
        cout << "  '" << r[i++] << "'" << endl;
    }

    EXPECT_STREQ(r[0], "a");
    EXPECT_STREQ(r[1], "bb");
    EXPECT_STREQ(r[2], "ccc");

    EXPECT_EQ(i, 3);
    i=0; while(r[i]) { delete[] r[i++]; }; delete[] r;
return;
    s = (char*)"type: OUTLINE; created: 2016-05-02 21:30:27; reads: 0; read: 2016-05-02 21:30:27; writes: 1; written: 2016-05-02 21:30:27; importance: 0; urgency: 0; id: urn:mindforger.com:outline:agttaW5kLWZvcmdlcnIWCxIOR2FlT3V0bGluZUJlYW4Y7Ls1DA;";
    r = stringSplit(s, ';');
    cout << "split('" << s << "') = split[]" << endl;
    i = 0;
    while(r[i]) {
        cout << "  '" << r[i++] << "'" << endl;
    }
    EXPECT_EQ(i, 9);
    i=0; while(r[i]) { delete r[i++]; }; delete r;
}

TEST(StringGearTestCase, Trim)
{
    char *r = stringTrim("abc");
    cout << "trim('abc') = '" << r << "'" << endl;
    EXPECT_EQ(0, strcmp("abc", r));
    delete[] r;

    r = stringTrim(" abc");
    cout << "trim(' abc') = '" << r << "'" << endl;
    EXPECT_EQ(0, strcmp("abc", r));
    delete[] r;

    r = stringTrim("abc ");
    cout << "trim('abc ') = '" << r << "'" << endl;
    EXPECT_EQ(0, strcmp("abc", r));
    delete[] r;

    r = stringTrim(" abc ");
    cout << "trim(' abc ') = '" << r << "'" << endl;
    EXPECT_EQ(0, strcmp("abc", r));
    delete[] r;

    r = stringTrim(" ");
    cout << "trim(' ') = '" << r << "'" << endl;
    EXPECT_EQ(0, strcmp("", r));
    delete[] r;

    r = stringTrim("");
    cout << "trim('') = '" << r << "'" << endl;
    EXPECT_EQ(0, strcmp("", r));
    delete[] r;
}

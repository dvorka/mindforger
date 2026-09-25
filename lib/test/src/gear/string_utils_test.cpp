/*
 string_utils_test.cpp     MindForger application test

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <sstream>

#include <gtest/gtest.h>

#include "../../../src/gear/string_utils.h"

using namespace std;
using namespace m8r;

TEST(StringGearTestCase, StringToNcName)
{
    // GIVEN
    string s("123 text 456");
    // WHEN
    string r = normalizeToNcName(s, '-');
    // THEN
    cout << s << " => " << r << endl;
    ASSERT_EQ("123-text-456", r);

    // GIVEN a name with non-ASCII (UTF-8) letters
    // WHEN normalized to a NCName-like identifier
    // THEN non-ASCII letters are kept as-is (see XML NCName spec, which
    // permits Unicode letters) instead of being destroyed to '-' runs -
    // https://github.com/dvorka/mindforger/issues/1501
    s.assign("čeština už je tu!");
    r = normalizeToNcName(s, '-');
    cout << s << " => " << r << endl;
    ASSERT_EQ("čeština-už-je-tu-", r);

    // GIVEN
    s.assign("Compensation Letter 05012021 - Doe, John (Clifton, Tony).pdf");
    // WHEN
    r = normalizeToNcName(s, '-');
    // THEN
    cout << s << " => " << r << endl;
    ASSERT_EQ("Compensation-Letter-05012021---Doe--John--Clifton--Tony--pdf", r);

    // GIVEN a name composed entirely of non-ASCII (Korean) letters
    // WHEN normalized
    // THEN it must NOT collapse to an empty string (issue #1501)
    s.assign("기타");
    r = normalizeToNcName(s, '-');
    cout << s << " => " << r << endl;
    ASSERT_EQ("기타", r);
    ASSERT_FALSE(r.empty());

    // GIVEN a name composed entirely of non-ASCII (Chinese) letters
    // WHEN normalized
    // THEN it must NOT collapse to an empty string (issue #1501)
    s.assign("普通话");
    r = normalizeToNcName(s, '-');
    cout << s << " => " << r << endl;
    ASSERT_EQ("普通话", r);
    ASSERT_FALSE(r.empty());
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

    EXPECT_STREQ("a", r[0]);
    EXPECT_STREQ("bb", r[1]);
    EXPECT_STREQ("ccc", r[2]);

    EXPECT_EQ(3, i);
    i=0; while(r[i]) { delete[] r[i++]; }; delete[] r;
return;
    s = (char*)"type: OUTLINE; created: 2016-05-02 21:30:27; reads: 0; read: 2016-05-02 21:30:27; writes: 1; written: 2016-05-02 21:30:27; importance: 0; urgency: 0; id: urn:mindforger.com:outline:agttaW5kLWZvcmdlcnIWCxIOR2FlT3V0bGluZUJlYW4Y7Ls1DA;";
    r = stringSplit(s, ';');
    cout << "split('" << s << "') = split[]" << endl;
    i = 0;
    while(r[i]) {
        cout << "  '" << r[i++] << "'" << endl;
    }
    EXPECT_EQ(9, i);
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

TEST(StringGearTestCase, VectorToString)
{
    vector<string*> v{};
    string s{};

    toString(v, s);

    ASSERT_EQ(0, s.size());
}

TEST(StringGearTestCase, StringTweaks)
{
    string s{"12345"};

    s[0] = 'a';

    ASSERT_STREQ("a2345", s.c_str());
}

TEST(StringGearTestCase, MarkdownParagraphBoundaryLine)
{
    // GIVEN
    vector<string> boundaryLines{
        "",
        "   ",
        "# Heading",
        "###### Heading level 6",
        "- bullet item",
        "* bullet item",
        "1. numbered item",
        "> blockquote",
        "```",
        "~~~",
        "    indented code (4 spaces)",
        "\tindented code (tab)",
        "---",
        "***",
        "___",
        "| a | table row |",
    };
    // WHEN / THEN
    for(const string& line: boundaryLines) {
        cout << "boundary? '" << line << "'" << endl;
        ASSERT_TRUE(isMarkdownParagraphBoundaryLine(line));
    }

    // GIVEN
    vector<string> proseLines{
        "This is a plain paragraph line.",
        "Another sentence, with a comma and 42 numbers.",
        "word",
    };
    // WHEN / THEN
    for(const string& line: proseLines) {
        cout << "prose? '" << line << "'" << endl;
        ASSERT_FALSE(isMarkdownParagraphBoundaryLine(line));
    }
}

TEST(StringGearTestCase, RewrapParagraphLinesToWidth)
{
    // GIVEN
    vector<string> lines{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod",
        "tempor incididunt ut labore et dolore magna aliqua.",
    };

    // WHEN
    vector<string> rewrapped = rewrapParagraphLines(lines, 20);

    // THEN
    for(const string& line: rewrapped) {
        cout << "rewrapped[" << line.size() << "] '" << line << "'" << endl;
        ASSERT_LE(line.size(), 20u);
    }
    // no word was lost/reordered by rewrap
    string joinedOriginal{}, joinedRewrapped{};
    for(const string& l: lines) { joinedOriginal += l + " "; }
    for(const string& l: rewrapped) { joinedRewrapped += l + " "; }
    istringstream origWords{joinedOriginal}, wrappedWords{joinedRewrapped};
    string ow, ww;
    while(origWords >> ow) {
        ASSERT_TRUE(static_cast<bool>(wrappedWords >> ww));
        ASSERT_EQ(ow, ww);
    }
    ASSERT_FALSE(static_cast<bool>(wrappedWords >> ww));
}

TEST(StringGearTestCase, RewrapParagraphLinesKeepsOverlongWordWhole)
{
    // GIVEN a single word longer than the requested width
    vector<string> lines{"https://www.mindforger.com/a/very/long/url/that/does/not/fit"};

    // WHEN
    vector<string> rewrapped = rewrapParagraphLines(lines, 10);

    // THEN it is kept whole on its own line rather than broken
    ASSERT_EQ(1u, rewrapped.size());
    ASSERT_EQ(lines[0], rewrapped[0]);
}

TEST(StringGearTestCase, RewrapParagraphLinesEmpty)
{
    // GIVEN
    vector<string> lines{};

    // WHEN
    vector<string> rewrapped = rewrapParagraphLines(lines, 80);

    // THEN
    ASSERT_TRUE(rewrapped.empty());
}

TEST(StringGearTestCase, RewrapParagraphLinesUsesCodepointWidthNotByteWidth)
{
    // GIVEN a Czech word whose UTF-8 byte length (9) is longer than its
    // Unicode codepoint length (7) - byte-based width counting would wrap
    // this line too early
    vector<string> lines{"čeština dnes"};

    // WHEN wrapped to a width that fits both words by codepoint count
    // (7 + 1 + 4 = 12) but not by UTF-8 byte count (9 + 1 + 4 = 14)
    vector<string> rewrapped = rewrapParagraphLines(lines, 12);

    // THEN both words stay on the same output line
    for(const string& line: rewrapped) {
        cout << "rewrapped[" << line.size() << "] '" << line << "'" << endl;
    }
    ASSERT_EQ(1u, rewrapped.size());
    ASSERT_EQ("čeština dnes", rewrapped[0]);
}

TEST(StringGearTestCase, RewrapParagraphLinesPreservesHardBreakWithTrailingSpaces)
{
    // GIVEN a paragraph where the 1st line ends with a markdown hard line
    // break (two trailing spaces) - short enough that a naive rewrap would
    // merge it with the 2nd line into a single wrapped line
    vector<string> lines{
        "Line one.  ",
        "Line two.",
    };

    // WHEN
    vector<string> rewrapped = rewrapParagraphLines(lines, 80);

    // THEN the hard break still splits the output into 2 lines and the
    // 2-space marker is preserved so the rendered document is not affected
    for(const string& line: rewrapped) {
        cout << "rewrapped[" << line.size() << "] '" << line << "'" << endl;
    }
    ASSERT_EQ(2u, rewrapped.size());
    ASSERT_EQ("Line one.  ", rewrapped[0]);
    ASSERT_EQ("Line two.", rewrapped[1]);
}

TEST(StringGearTestCase, RewrapParagraphLinesPreservesHardBreakWithTrailingBackslash)
{
    // GIVEN a paragraph where the 1st line ends with a markdown hard line
    // break expressed as a trailing backslash
    vector<string> lines{
        "Line one.\\",
        "Line two.",
    };

    // WHEN
    vector<string> rewrapped = rewrapParagraphLines(lines, 80);

    // THEN the hard break still splits the output into 2 lines and the
    // backslash marker is preserved
    for(const string& line: rewrapped) {
        cout << "rewrapped[" << line.size() << "] '" << line << "'" << endl;
    }
    ASSERT_EQ(2u, rewrapped.size());
    ASSERT_EQ("Line one.\\", rewrapped[0]);
    ASSERT_EQ("Line two.", rewrapped[1]);
}

TEST(StringGearTestCase, RewrapParagraphLinesHardBreakInLongParagraphStillWraps)
{
    // GIVEN a long line without a hard break, followed by a short line
    // with a hard break, followed by more text - words before the hard
    // break must still be wrapped to width, and the hard break must force
    // a new output line regardless of the remaining width budget
    vector<string> lines{
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod",
        "tempor.  ",
        "Ut enim ad minim veniam.",
    };

    // WHEN
    vector<string> rewrapped = rewrapParagraphLines(lines, 20);

    // THEN
    for(const string& line: rewrapped) {
        cout << "rewrapped[" << line.size() << "] '" << line << "'" << endl;
    }
    ASSERT_FALSE(rewrapped.empty());
    // the hard break marker must survive and terminate its own output line
    bool foundHardBreakLine{false};
    for(const string& line: rewrapped) {
        if(line.size()>=2 && line.substr(line.size()-2)=="  ") {
            foundHardBreakLine = true;
            break;
        }
    }
    ASSERT_TRUE(foundHardBreakLine);
    // and the words after it must not appear on the same output line
    ASSERT_EQ(string::npos, rewrapped.back().find("tempor"));
}

TEST(StringGearTestCase, SortLinesAlphabetically)
{
    // GIVEN
    vector<string> lines{
        "banana",
        "Apple",
        "cherry",
        "apple",
    };

    // WHEN
    vector<string> sorted = sortLinesAlphabetically(lines);

    // THEN
    for(const string& line: sorted) {
        cout << "sorted '" << line << "'" << endl;
    }
    ASSERT_EQ(4u, sorted.size());
    // case insensitive ordering w/ case sensitive tie break
    ASSERT_EQ("Apple", sorted[0]);
    ASSERT_EQ("apple", sorted[1]);
    ASSERT_EQ("banana", sorted[2]);
    ASSERT_EQ("cherry", sorted[3]);
}

TEST(StringGearTestCase, SortLinesAlphabeticallyKeepsLinesIntact)
{
    // GIVEN Markdown list items w/ indentation and duplicates
    vector<string> lines{
        "- zebra",
        "- ant",
        "- ant",
        "  - beetle",
    };

    // WHEN
    vector<string> sorted = sortLinesAlphabetically(lines);

    // THEN
    for(const string& line: sorted) {
        cout << "sorted '" << line << "'" << endl;
    }
    ASSERT_EQ(4u, sorted.size());
    // indentation is a part of the compared line - indented items sort first
    ASSERT_EQ("  - beetle", sorted[0]);
    ASSERT_EQ("- ant", sorted[1]);
    ASSERT_EQ("- ant", sorted[2]);
    ASSERT_EQ("- zebra", sorted[3]);
}

TEST(StringGearTestCase, SortLinesAlphabeticallyIsDeterministic)
{
    // GIVEN lines which differ in the case only i.e. lines whose lower case keys are equal
    vector<string> lines{
        "mind",
        "MIND",
        "Mind",
    };

    // WHEN sorted twice - the 2nd time from a different initial order
    vector<string> sorted = sortLinesAlphabetically(lines);
    vector<string> reversed{lines.rbegin(), lines.rend()};
    vector<string> sortedReversed = sortLinesAlphabetically(reversed);

    // THEN both runs give the same result
    for(const string& line: sorted) {
        cout << "sorted '" << line << "'" << endl;
    }
    ASSERT_EQ(sorted, sortedReversed);
    ASSERT_EQ("MIND", sorted[0]);
    ASSERT_EQ("Mind", sorted[1]);
    ASSERT_EQ("mind", sorted[2]);
}

TEST(StringGearTestCase, SortLinesAlphabeticallyOrdersNonAsciiByUtf8Bytes)
{
    // GIVEN lines w/ non-ASCII letters - the A-Z only case folding orders them
    // by their UTF-8 bytes i.e. deterministically, but not by Czech collation
    vector<string> lines{
        "Cukr",
        "Čaj",
        "Ananas",
        "cukr",
    };

    // WHEN
    vector<string> sorted = sortLinesAlphabetically(lines);

    // THEN
    for(const string& line: sorted) {
        cout << "sorted '" << line << "'" << endl;
    }
    ASSERT_EQ(4u, sorted.size());
    ASSERT_EQ("Ananas", sorted[0]);
    ASSERT_EQ("Cukr", sorted[1]);
    ASSERT_EQ("cukr", sorted[2]);
    // non-ASCII letter is NOT folded, so it sorts past the ASCII ones - and it
    // must survive the sort byte by byte
    ASSERT_EQ("Čaj", sorted[3]);
}

TEST(StringGearTestCase, SortLinesAlphabeticallyEmpty)
{
    // GIVEN
    vector<string> lines{};

    // WHEN
    vector<string> sorted = sortLinesAlphabetically(lines);

    // THEN
    cout << "sorted[" << sorted.size() << "]" << endl;
    ASSERT_TRUE(sorted.empty());
}

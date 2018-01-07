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
#include <cassert>
#include <iostream>
#include <cstdio>

#include <gtest/gtest.h>

#include "../../../src/gear/datetime_utils.h"

using namespace std;
using namespace m8r;

/**
 * @brief Test data/time immutability.
 *
 * It's expected that serialization/deserialization doesn't changes the "string" representation
 * of time (DTS is ignored), it's expected that user's local time is always used. Cross time-zone
 * sharing is not considered (for now).
 */
TEST(DateTimeGearTestCase, Immutability)
{
    string in[] = {
            // DTS (summer months)
            "2016-05-02 21:30:28",
            "2016-5-2 21:30:28",
            "2016-5-28 18:45:00",
            "2016-05-28 18:45:33",
            // normal time (winter months)
            "2017-1-1 0:00:00",
            "2004-03-21 12:45:33",
            "2016-12-21 12:45:33",
            };
    string out[] = {
            // DTS (summer months)
            "2016-05-02 21:30:28",
            "2016-05-02 21:30:28",
            "2016-05-28 18:45:00",
            "2016-05-28 18:45:33",
            // normal time (winter months)
            "2017-01-01 00:00:00",
            "2004-03-21 12:45:33",
            "2016-12-21 12:45:33",
            };
    tm from{0,0,0,0,0,0,0,0,0,0,0};
    char to[50];
    time_t timet;
    cout << "Original (char*) -> Parsed (tm) -> time_t -> Roundtrip (char*)" << endl;
    string roundtrip;

    for(size_t i=0; i<sizeof(in)/sizeof(string); i++) {
        // C-style initialization as GCC doesn't like {}
        memset(&from, 0, sizeof from);

        datetimeFrom(in[i].c_str(), &from);
        // does time zone conversion
        datetimeTo(&from, to);
        timet = datetimeSeconds(&from);
        roundtrip = datetimeToString(timet);
        cout << "#" << i << " " << in[i] << " -> " << to << " -> " << roundtrip << " # DTS " << from.tm_isdst << endl;

        ASSERT_FALSE(strcmp(out[i].c_str(), to));
        ASSERT_FALSE(out[i].compare(roundtrip));
    }
}

TEST(DateTimeGearTestCase, PrettyTimestamp)
{
    const char *in;
    time_t ts, ts2;
    struct tm datetime;
    // C-style initialization as GCC doesn't like {}
    memset(&datetime, 0, sizeof datetime);
    string pretty;

    // calibration
    in = "2016-5-28 18:45:00";
    datetimeFrom(in, &datetime);
    ts = datetimeSeconds(&datetime);

    in = "2016-5-28 18:46:00";
    datetimeFrom(in, &datetime );
    ts2 = datetimeSeconds(&datetime);
    cout << endl << "CALIBRATION 1 min: " << ts2-ts;
    cout << endl << "------------------------------";

    // long time ago
    in = "2004-03-21 12:45:33";
    datetimeFrom(in, &datetime);
    ts = datetimeSeconds(&datetime);
    pretty = datetimeToPrettyHtml(&ts);
    cout << endl << "LONG  " << in << " -> " << pretty;

    // this year
    in = "2016-05-28 18:45:33";
    datetimeFrom(in, &datetime);
    ts = datetimeSeconds(&datetime);
    pretty = datetimeToPrettyHtml(&ts);
    cout << endl << "YEAR  "<< in << " -> " << pretty;

    // this week
    in = "2016-12-25 18:45:33";
    datetimeFrom(in, &datetime);
    ts = datetimeSeconds(&datetime);
    pretty = datetimeToPrettyHtml(&ts);
    cout << endl << "WEEK  "<< in << " -> " << pretty;

    // today
    in = "2016-12-28 18:45:33";
    datetimeFrom(in, &datetime);
    ts = datetimeSeconds(&datetime);
    pretty = datetimeToPrettyHtml(&ts);
    cout << endl << "TODAY "<< in << " -> " << pretty;
}

TEST(DateTimeGearTestCase, MdDateTimeParsing)
{
    struct tm datetime;
    // C-style initialization as GCC doesn't like {}
    memset(&datetime, 0, sizeof datetime);

    datetimeFrom("2004-03-21 12:45:33", &datetime);
    cout << "year+1900: " << datetime.tm_year << endl; // year =  tm_year+1900
    cout << "month+1: " << datetime.tm_mon << endl; // month = tm_mon+1
    cout << "day: " << datetime.tm_mday << endl;
    cout << "hour: " << datetime.tm_hour << endl;
    cout << "min: " << datetime.tm_min << endl;
    cout << "sec: " << datetime.tm_sec << endl;
    cout << endl;
    EXPECT_EQ(datetime.tm_year, 104);

    datetimeFrom("1976-11-12 18:31:01", &datetime);
    cout << "year+1900: " << datetime.tm_year << endl; // year =  tm_year+1900
    cout << "month+1: " << datetime.tm_mon << endl; // month = tm_mon+1
    cout << "day: " << datetime.tm_mday << endl;
    cout << "hour: " << datetime.tm_hour << endl;
    cout << "min: " << datetime.tm_min << endl;
    cout << "sec: " << datetime.tm_sec << endl;
    cout << endl;
    EXPECT_EQ(datetime.tm_year, 76);

    datetimeFrom("2016-12-30 5:31:01", &datetime);
    cout << "year+1900: " << datetime.tm_year << endl; // year =  tm_year+1900
    cout << "month+1: " << datetime.tm_mon << endl; // month = tm_mon+1
    cout << "day: " << datetime.tm_mday << endl;
    cout << "hour: " << datetime.tm_hour << endl;
    cout << "min: " << datetime.tm_min << endl;
    cout << "sec: " << datetime.tm_sec << endl;
    cout << endl;
    EXPECT_EQ(datetime.tm_year, 116);
}

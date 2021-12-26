/*
 debug.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_DEBUG_H_
#define M8R_DEBUG_H_

// DO_MF_DEBUG to be enabled in qmake (either .pro or CLI or QtCreator build command):
//   - configuration: DEFINES = DO_MF_DEBUG
//   - command line: CONFIG+=mfunits

#ifdef DO_MF_DEBUG
    #include <chrono>
    #include <iostream>
    #ifndef _WIN32
        #include <execinfo.h>
    #endif //_WIN32
    #define MF_DEBUG(x) do { std::cerr << x; } while (0)

    // enable/disable verbose debug of particular components
    //#define MF_DEBUG_HTML
    //#define MF_DEBUG_ASYNC_TASKS
    //#define MF_DEBUG_QRC
    //#define MF_DEBUG_L10N
    //#define MF_DEBUG_AUTOLINKING

    // show WIP features
    #define MF_WIP

    // future timestamps check
    #define MF_ASSERT_WHERE " (" << __FILE__ << ":" << __LINE__ << ")"
    #define MF_ASSERT_FUTURE_CREATE(TIMESTAMP, KEY, NAME) \
        if(TIMESTAMP > datetimeNow()) { std::cerr << "ERROR: attempt to set future CREATION timestamp: " << KEY << " ~ " << NAME << MF_ASSERT_WHERE << endl; abort(); }
    #define MF_ASSERT_FUTURE_READ(TIMESTAMP, KEY, NAME) \
        if(TIMESTAMP > datetimeNow()) { std::cerr << "ERROR: attempt to set future READ timestamp: " << KEY << " ~ " << NAME << MF_ASSERT_WHERE << endl; abort(); }
    #define MF_ASSERT_FUTURE_MODIFICATION(TIMESTAMP, KEY, NAME) \
        if(TIMESTAMP > datetimeNow()) { std::cerr << "ERROR: attempt to set future MODIFICATION timestamp: " << KEY << " ~ " << NAME << MF_ASSERT_WHERE << endl; abort(); }
    #define MF_ASSERT_FUTURE_TIMESTAMPS(C_TS, R_TS, M_TS, KEY, NAME) \
        MF_ASSERT_FUTURE_CREATE(C_TS, KEY, NAME); \
        MF_ASSERT_FUTURE_READ(R_TS, KEY, NAME); \
        MF_ASSERT_FUTURE_MODIFICATION(M_TS, KEY, NAME);
#ifndef _WIN32
    #define MF_PRINT_STACKTRACE() \
        void *array[50]; size_t size = backtrace(array, 50); backtrace_symbols_fd(array, size, STDERR_FILENO)
#endif // _WIN32
#else // DO_MF_DEBUG
    #define MF_DEBUG(x) do {;} while (0)
    #define MF_ASSERT_FUTURE_TIMESTAMPS(x1,x2,x3,x4,x5) do {;} while (0)
#endif // DO_MF_DEBUG

#endif /* M8R_DEBUG_H_ */

#!/bin/bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

export SCRIPT_DIR=`pwd`
export BUILD_DIR=${SCRIPT_DIR}/..




# TODO rewrite this script according to non-valgrind unit test script






# Hints:
#  - use -g GCC option to get line information
#  - use --track-origins=yes Valgrind option to better track root cause of problems like "Conditional jump using uninitialized values"

#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind -v --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests

#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --track-origins=yes --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=NoteTestCase.AddNewStencilNoteToOutline
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --track-origins=yes --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=OutlineTestCase.NewOutlineFromStencil
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --track-origins=yes --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=OutlineTestCase.NewAndDeleteOutline
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind -v --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=MindTestCase.FTS
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=RepositoryIndexerTestCase.MindForgerRepository
cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full --track-origins=yes ./mindforger-lib-unit-tests --gtest_filter=ConfigurationTestCase.FromConstructor
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=ConfigurationTestCase.FromEnvironment
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind -v --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=MarkdownParserTestCase.MarkdownParserSections
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=DateTimeGearTestCase.*
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=DateTimeGearTestCase.MdDateTimeParsing
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=DateTimeGearTestCase.Immutability
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind -v --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=DateTimeGearTestCase.PrettyTimestamp
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && valgrind -v --track-origins=yes --tool=memcheck --leak-check=full ./mindforger-lib-unit-tests --gtest_filter=StringGearTestCase.Split

cd ${BUILD_DIR} && make clean

# eof

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

# run all tests
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && gdb ./mindforger-lib-unit-tests

# run selected test(s)
# --gtest-filter=XYZ.* ... filter tests
# --gtest_repeat=1000  ... performance/load tests
#export TEST_NAME="MarkdownParserTestCase.MarkdownLexerSectionsNoMetadata"
export TEST_NAME="MarkdownParserTestCase.*"
#export TEST_NAME="MarkdownParserTestCase.MarkdownParserSections"
#export TEST_NAME="MarkdownParserTestCase.MarkdownParserSectionsNoMetadata"
#export TEST_NAME="RepositoryIndexerTestCase.*"
#export TEST_NAME="RepositoryIndexerTestCase.MarkdownRepository"
#export TEST_NAME="RepositoryIndexerTestCase.MindForgerRepository"
#export TEST_NAME="RepositoryIndexerTestCase.MindForgerFile"
#export TEST_NAME="RepositoryIndexerTestCase.MarkdownFile"
#export TEST_NAME="NoteTestCase.*"
#export TEST_NAME="MindTestCase.LearnAmnesiaLearn"
#export TEST_NAME="DateTimeGearTestCase.Immutability"
#export TEST_NAME="ConfigurationTestCase.*"
#export TEST_NAME="ConfigurationTestCase.FromConstructor"
#export TEST_NAME="ConfigurationTestCase.Environment"
#export TEST_NAME="MarkdownParserTestCase.Bug37Notrailing"
#export TEST_NAME="MarkdownParserBugsTestCase.*"
#export TEST_NAME="NoteTestCase.PromoteDemoteUpDownNote"
#export TEST_NAME="NoteTestCase.RefactorNote"
#export TEST_NAME="OutlineTestCase.CloneOutline"
#export TEST_NAME="StringGearTestCase.Split"

cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=${TEST_NAME}
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && gdb --args ./mindforger-lib-unit-tests --gtest_filter=${TEST_NAME}

# clean all to avoid side effects next time
#cd ${BUILD_DIR} && make clean

# eof

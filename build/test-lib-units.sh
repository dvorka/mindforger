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

cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && gdb ./mindforger-lib-unit-tests

#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=StringGearTestCase.Split
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=NoteTestCase.*
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=MindTestCase.Mind
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=DateTimeGearTestCase.Immutability
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=ConfigurationTestCase.*
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=ConfigurationTestCase.FromConstructor
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=ConfigurationTestCase.Environment
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=MarkdownParserTestCase.FileSystemPersistence
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=MarkdownParserTestCase.Bug37Notrailing
#cd ${BUILD_DIR} && qmake mindforger-lib-unit-tests.pro && make clean && make && ./mindforger-lib-unit-tests --gtest_filter=MarkdownParserBugsTestCase.*
#cd ${BUILD_DIR} && make clean

# eof

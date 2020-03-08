#!/bin/bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>
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

export OPTION_RUN_ALL_TESTS=yes # comment this line to run OPTION_TEST only
export OPTION_RECOMPILE=yes # recompile before running test(s) (comment this line to disable)
export OPTION_RUN_VALGRIND=yes # run test(s) w/ Valgrind (comment this line to disable)
#export OPTION_RUN_GDB=yes # run test(s) w/ GDB (comment this line to disable)

# benchmarks
#export OPTION_TEST="AiBenchmark.AaMatrix"
#export OPTION_TEST="TrieBenchmark.TrieVsMap"
#export OPTION_TEST="HtmlBenchmark.Outline"
#export OPTION_TEST="MarkdownParserBenchmark.ParserMeta"

# tests
#export OPTION_TEST="AutolinkingCmarkTestCase.Monster"
#export OPTION_TEST="AutolinkingCmarkTestCase.*"
#export OPTION_TEST="AutolinkingCmarkTestCase.CmarkAstRowWalker"
#export OPTION_TEST="AutolinkingCmarkTestCase.CmarkAstBlockTransformer"
#export OPTION_TEST="AutolinkingCmarkTestCase.NanoRepo"
#export OPTION_TEST="AutolinkingCmarkTestCase.MicroRepo"
#export OPTION_TEST="AutolinkingTestCase.*"
#export OPTION_TEST="AutolinkingTestCase.CrashAndBurn"
#export OPTION_TEST="FtsTestCase.*"
#export OPTION_TEST="TrieTestCase.*"
#export OPTION_TEST="TrieTestCase.AddAndRemove"
#export OPTION_TEST="FileGearTestCase.DeepCopy"
#export OPTION_TEST="FileGearTestCase.DeepProductionCopy"
#export OPTION_TEST="FileGearTestCase.DeepCopyToExisting"
#export OPTION_TEST="AiNlpTestCase.*"
#export OPTION_TEST="AiNlpTestCase.AaUniverseBow"
#export OPTION_TEST="AiNlpTestCase.AaRepositoryBow"
#export OPTION_TEST="AiNlpTestCase.Outline"
#export OPTION_TEST="AiNlpTestCase.Lexicon"
#export OPTION_TEST="AiNlpTestCase.Tokenizer"
#export OPTION_TEST="AiNlpTestCase.Stemmer"
#export OPTION_TEST="HtmlTestCase.*"
#export OPTION_TEST="HtmlTestCase.Outline"
#export OPTION_TEST="HtmlTestCase.TaskList"
#export OPTION_TEST="HtmlTestCase.NoteLinks"
#export OPTION_TEST="MarkdownParserTestCase.*"
#export OPTION_TEST="MarkdownParserTestCase.Bug622Loop64kLinesOverflow"
#export OPTION_TEST="MarkdownParserTestCase.Links"
#export OPTION_TEST="MarkdownParserTestCase.TimeScope"
#export OPTION_TEST="MarkdownParserTestCase.Deadline"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownLexerSections"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownLexerSectionsPreamble"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownLexerSectionsNoMetadata"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownLexerSectionsPostDeclaredHeaders"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownLexerSectionsPostDeclaredHeaders2"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownLexerTimeScope"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownLexerLinks"
#export OPTION_TEST="MarkdownParserTestCase.*"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownParserSections"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownParserSectionsPreamble"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownParserSectionsEmptyFirstLine"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownParserSectionsNoMetadata"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownParserSections"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownRepresentationPostDeclaredSection"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownRepresentationTrailingHashesSection"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownRepresentationEmptyFirstLine"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownRepresentationPreamble"
#export OPTION_TEST="MarkdownParserTestCase.MarkdownRepresentationSectionTrailingHashes"
#export OPTION_TEST="RepositoryIndexerTestCase.*"
#export OPTION_TEST="RepositoryIndexerTestCase.RepositoryTypeDetection"
#export OPTION_TEST="RepositoryIndexerTestCase.MarkdownRepository"
#export OPTION_TEST="RepositoryIndexerTestCase.MindForgerRepository"
#export OPTION_TEST="RepositoryIndexerTestCase.MindForgerFile"
#export OPTION_TEST="RepositoryIndexerTestCase.MarkdownFile"
#export OPTION_TEST="RepositoryIndexerTestCase.MakePathRelative"
#export OPTION_TEST="NoteTestCase.*"
#export OPTION_TEST="NoteTestCase.MangleNoteName"
#export OPTION_TEST="NoteTestCase.DeepUpDownFirstLastClone"
#export OPTION_TEST="NoteTestCase.PromoteDemoteUpDownNote"
#export OPTION_TEST="NoteTestCase.RefactorNote"
#export OPTION_TEST="NoteTestCase.DirectNoteChildren"
#export OPTION_TEST="MindTestCase.LearnAndRememberMindForgerRepository"
#export OPTION_TEST="MindTestCase.LearnAmnesiaLearn"
#export OPTION_TEST="MindTestCase.CommonWordsBlacklist"
#export OPTION_TEST="DateTimeGearTestCase.*"
#export OPTION_TEST="DateTimeGearTestCase.Immutability"
#export OPTION_TEST="ConfigurationTestCase.*"
export OPTION_TEST="ConfigurationTestCase.SaveDefaultConfig"
#export OPTION_TEST="ConfigurationTestCase.SaveAndLoad"
#export OPTION_TEST="ConfigurationTestCase.FromConstructor"
#export OPTION_TEST="ConfigurationTestCase.FromEnvironment"
#export OPTION_TEST="ConfigurationTestCase.Save*"
#export OPTION_TEST="MarkdownParserTestCase.Bug37Notrailing"
#export OPTION_TEST="MarkdownParserBugsTestCase.*"
#export OPTION_TEST="OutlineTestCase.CloneOutline"
#export OPTION_TEST="StringGearTestCase.Split"
#export OPTION_TEST="OutlineTestCase.DirectOutlineNoteChildren"

# environment - to be specified in .bashrc or elsewhere:
#   export M8R_CPU_CORES=7
#   export M8R_GIT_PATH=/home/dvorka/p/mindforger/git/mindforger

if [ -z ${M8R_CPU_CORES} ]
then
    echo "Set M8R_CPU_CORES env var to specify number of CPU cores to be used by compiler/make"
    exit 1
fi
if [ -z ${M8R_GIT_PATH} ]
then
    echo "Set M8R_GIT_PATH env var to specify location of MindForger Git repository"
    exit 1
fi

if [ ${OPTION_RUN_VALGRIND} ] 
then
    if [ ${OPTION_RUN_GDB} ] 
    then
	export M8R_VALGRIND="valgrind --vgdb=yes --vgdb-error=0 --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all"
    else
	#export M8R_VALGRIND="valgrind --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all"
	export M8R_VALGRIND="valgrind -v --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all"
    fi
else
    export M8R_VALGRIND=    
    if [ ${OPTION_RUN_GDB} ] 
    then
	export M8R_GDB="gdb --args"
    fi
fi

export SCRIPT_DIR=`pwd`
export BUILD_DIR=${SCRIPT_DIR}/../lib/test


# Compile source w/ debug code enabled and various test libs linked to get test-ready binary
#  - use -g GCC option to get line information
# Valgrind
#  - use --track-origins=yes Valgrind option to better track root cause of problems
#    like "Conditional jump using uninitialized values"
#  - use --leak-check=full
#  - use --show-leak-kinds=all
#  - use -v only if you want a lot of info (might be too much)
if [ ${OPTION_RECOMPILE} ]
then
    # cleanup
    cd ${BUILD_DIR} && cd ../../ && make clean && rm *.a
    cd ${BUILD_DIR} && cd ./src && make clean
    # recursive qmake and recompilation - IMPORTANT: mfunits ensures MF_DEBUG is enabled in library src
    cd ${BUILD_DIR} && make clean && qmake -r mindforger-lib-unit-tests.pro CONFIG+=mfunits && make -j${M8R_CPU_CORES}
    if [ ${?} -ne 0 ]
    then
	exit 1
    fi
fi

# logs
export TEST_LOG_FILE="${M8R_GIT_PATH}/TEST_LOG_LIB.txt"
rm -vf ${TEST_LOG_FILE}

# run test(s)
if [ ${OPTION_RUN_ALL_TESTS} ]
then
    echo "Running: ${M8R_GDB} ${M8R_VALGRIND} ./mindforger-lib-unit-tests"
    cd ${BUILD_DIR} && ${M8R_GDB} ${M8R_VALGRIND} ./src/mindforger-lib-unit-tests #> ${TEST_LOG_FILE} 2>&1
else
    # run selected test(s)
    # --gtest-filter=XYZ.* ... filter tests
    # --gtest_repeat=1000  ... performance/load tests

    cd ${BUILD_DIR} && ${M8R_GDB} ${M8R_VALGRIND} ./src/mindforger-lib-unit-tests --gtest_filter=${OPTION_TEST} #> ${TEST_LOG_FILE} 2>&1
fi

# eof

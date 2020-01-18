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

# options
#export OPTION_EDIT_TEST=yes # edit the test in SikuliX IDE and exit (comment this line to disable)
#export OPTION_RUN_TRAINER=yes # run MindForger to create test in SikuliX IDE and exit (comment this line to disable)
#export OPTION_RUN_VALGRIND=yes # run test(s) w/ Valgrind (comment this line to disable)
#export OPTION_RECOMPILE=yes # recompile before running test(s) (comment this line to disable)
#export OPTION_RUN_ALL_TESTS=yes # comment this line to disable
export OPTION_TESTS="open-exit.sikuli"
export OPTION_TEST=open-exit.sikuli 

export M8R_TEST_APP_DIR="`pwd`/../app/test/qt"
export M8R_TEST_REPOSITORY="${HOME}/tmp/small-repository"

# environment - to be specified in .bashrc or elsewhere:
#   export M8R_CPU_CORES=7
#   export M8R_GIT_PATH=/home/dvorka/p/mindforger/git/mindforger
#   export M8R_SIKULIX_DIR="/home/dvorka/bin/sikulix"
#   export M8R_SIKULIX_SCRIPTS_DIR=${M8R_GIT_PATH}"/test/sikulix"

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
if [ -z ${M8R_SIKULIX_DIR} ]
then
    echo "Set M8R_SIKULIX_DIR env var to specify directory w/ SikuliX binary"
    exit 1
fi
if [ -z ${M8R_SIKULIX_SCRIPTS_DIR} ]
then
    echo "Set M8R_SIKULIX_SCRIPTS_DIR env var to specify directory w/ MindForger's SikuliX scripts"
    exit 1
fi
if [ ${OPTION_RUN_VALGRIND} ] 
then
    export M8R_VALGRIND="valgrind --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all"
    #export M8R_VALGRIND="valgrind -v --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all"
else
    export M8R_VALGRIND=
fi


# Run SikuliX IDE and exit
if [ ${OPTION_EDIT_TEST} ]
then
    cd ${M8R_SIKULIX_DIR} && ./runsikulix
    exit 0
fi

   
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
    cd ../test/qt && make clean && qmake -qt5 mindforger-gui-tests.pro && make -j${M8R_CPU_CORES}
fi

# Run MindForger to create a SikuliX script and exit
if [ ${OPTION_RUN_TRAINER} ]
then
    cd ${M8R_TEST_APP_DIR} && ./mindforger-gui-tests ${M8R_TEST_REPOSITORY}
    exit 0
fi


# logs
export TEST_LOG_FILE="${M8R_GIT_PATH}/TEST_LOG_GUI.txt"
rm -vf ${TEST_LOG_FILE}

# run UI test(s)
if [ ${OPTION_RUN_ALL_TESTS} ]
then
    for GUI_TEST in open-exit.sikuli
    do
	cd ${M8R_TEST_APP_DIR} && ${M8R_VALGRIND} ./mindforger-gui-tests ${M8R_TEST_REPOSITORY} &
	echo "Waiting for MindForger to start..." && if [ -n ${M8R_VALGRIND} ]; then sleep 25; else sleep 1; fi
        echo "Running SikuliX to test MindForger"
	cd ${M8R_SIKULIX_DIR} && ./runsikulix -r ${M8R_SIKULIX_SCRIPTS_DIR}/${GUI_TEST}
    done > ${TEST_LOG_FILE} 2>&1
else
    cd ${M8R_TEST_APP_DIR} && ${M8R_VALGRIND} ./mindforger-gui-tests ${M8R_TEST_REPOSITORY} &
    echo "Waiting for MindForger to start... (${M8R_VALGRIND})" && if [ ${M8R_VALGRIND} ]; then sleep 25; else sleep 1; fi
    echo "Running SikuliX to test MindForger"
    cd ${M8R_SIKULIX_DIR} && ./runsikulix -r ${M8R_SIKULIX_SCRIPTS_DIR}/${OPTION_TEST}
fi > ${TEST_LOG_FILE} 2>&1

# eof

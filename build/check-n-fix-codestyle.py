#!/usr/bin/env python
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

# help
helpString = """MindForger codestyle check & fix tool.

Usage check-n-fix-codestyle.py [OPTION]

Output:
  -h  --help        help
  -c  --color       use color to highlight output
  -l  --link        print paths as URIs to be clickable in terminal like gnome-terminal
  -g  --gcc         print GCC-like output

MindForger codestyle:

  C++:
    Source file has correct copyright (checked)
    Things to be done are marked with TODO keyword (checked)
    Trailing spaces are not present (checked, can be fixed with --fix-trailing-spaces)
    TABs are not present - SPACEs to be used instead (checked)
    {{}} used w/ constructor having 0/1 parameter, () otherwise
    CamelCase class names (no underscores)
    lower_case_with_unserscores class files names
    .h extension for header files
    .cpp extension for class files

  Python:
    Source file has correct copyright (checked)
    Things to be done are marked with TODO keyword (checked)
    Trailing spaces are not present (checked, can be fixed with --fix-trailing-spaces)

Mail bug reports and suggestions to martin.dvorak@mindforger.com
"""

import os
import re
import sys
import subprocess

__version__ = "1.0.0"
__notes__ = "development version"

# options
OPTION_TEAMCITY = False
OPTION_FILE_AS_URIS = ''
OPTION_GCC = False
OPTION_FIX_TRAILING_SPACES = False

# files to SKIP from validation
BLACKLIST = {
    'mindforger/src/qt/3rdparty/elasticnodes/node.cpp',
    'mindforger/src/qt/3rdparty/elasticnodes/node.h',
    'mindforger/src/qt/3rdparty/elasticnodes/graphwidget.h',
    'mindforger/src/qt/3rdparty/elasticnodes/edge.h',
    'mindforger/src/qt/3rdparty/elasticnodes/edge.cpp',
    'mindforger/src/qt/3rdparty/elasticnodes/graphwidget.cpp',

    'mindforger/lib/src/3rdparty/hoedown/autolink.c',
    'mindforger/lib/src/3rdparty/hoedown/html.h',
    'mindforger/lib/src/3rdparty/hoedown/version.h',
    'mindforger/lib/src/3rdparty/hoedown/autolink.h',
    'mindforger/lib/src/3rdparty/hoedown/html.c',
    'mindforger/lib/src/3rdparty/hoedown/stack.h',
    'mindforger/lib/src/3rdparty/hoedown/escape.h',
    'mindforger/lib/src/3rdparty/hoedown/html_blocks.c',
    'mindforger/lib/src/3rdparty/hoedown/stack.c',
    'mindforger/lib/src/3rdparty/hoedown/document.h',
    'mindforger/lib/src/3rdparty/hoedown/document.c',
    'mindforger/lib/src/3rdparty/hoedown/html_smartypants.c',
    'mindforger/lib/src/3rdparty/hoedown/buffer.c',
    'mindforger/lib/src/3rdparty/hoedown/buffer.h',
    'mindforger/lib/src/3rdparty/hoedown/version.c',
    'mindforger/lib/src/3rdparty/hoedown/escape.c',
    'mindforger/licenses/hoedown-license.txt'
}

COPYRIGHT_CPP = 'Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>'
COPYRIGHT_PYTHON = '# Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>'
COPYRIGHT_XML = '<!-- Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com> -->'

ESC_LIGHT_RED = ''
ESC_LIGHT_GREEN = ''
ESC_YELLOW = ''
ESC_WHITE = ''
ESC_NO_COLOR = ''

class CodestyleEnforcer:
    '''
    Main class - finds all files and validate them
    '''

    def __init__(self, gitRepositoryRootDir):
        print 'MindForger Codestyle Checker'
        self.gitRepositoryRootDir = gitRepositoryRootDir
        self.cppFiles = {}
        self.pythonFiles = {}
        self.report = []

    def loadFiles(self):
        for dirName, subdirList, fileList in os.walk(self.gitRepositoryRootDir):
            for f in fileList:
                    fileName = os.path.join(dirName, f)
                    relativeFileName = fileName[len(self.gitRepositoryRootDir)+1:]
                    if relativeFileName not in BLACKLIST:
                        if f.endswith('.h') or f.endswith('.cpp'):
                            cpp = CppFile(fileName)
                            self.cppFiles[fileName] = cpp
                        elif f.endswith('.py'):
                            p = PythonFile(fileName)
                            self.pythonFiles[fileName] = p

    def validate(self):
        self.validateFiles()
        self.showReport()
                            
    def validateFiles(self):
       sys.stdout.write('Checking H/CPP source files')
       for f in self.cppFiles.keys():
           self.cppFiles[f].validate(self.report)
       print ''
       sys.stdout.write('Checking Python source files')
       for f in self.pythonFiles.keys():
           self.pythonFiles[f].validate(self.report)
       print ''

    # TODO implementation to be finished
    def fixTrailingSpaces(self):
       sys.stdout.write('\nFixing H/CPP source files...')
       for f in self.cppFiles.keys():
           with open(f) as file:
               for line in file:
                   #line = line.rstrip()
                   if line:
                       print line
       #sys.stdout.write('\n  Fixing Python source files...')
       #for f in self.pythonFiles.keys():
       #    self.pythonFiles[f].validate(self.report)
       #print ''
       
    def showReport(self):
       reportEntries = 0 
       if len(self.report) == 0:
           print '\nMindForger Code Style Checker finished:'
           print '{}  [INFO] successfuly checked with no issues{}'.format(ESC_LIGHT_GREEN, ESC_NO_COLOR)
       else:
           print '\nReport:'
           for entry in self.report:
               for problem in entry.problems:
                   reportEntries+=1
                   if problem[0].startswith('  [ERROR]'):
                       if OPTION_GCC:
                           print "{}{}:{}:1 {}{}{}{}".format(
                               OPTION_FILE_AS_URIS,
                               entry.filename,
                               problem[1],
                               ESC_LIGHT_RED,
                               problem[0].strip(),
                               ESC_NO_COLOR)
                       else:
                           print "{}{}:{}".format(OPTION_FILE_AS_URIS,entry.filename,problem[1])
                           print '{}{}{}'.format(ESC_LIGHT_RED, problem[0], ESC_NO_COLOR)
                   elif problem[0].startswith('  [WARNING]'): 
                       if OPTION_GCC:
                           print "{}{}:{}:1 {}{}{}".format(
                               OPTION_FILE_AS_URIS,
                               entry.filename,
                               problem[1],
                               ESC_YELLOW,
                               problem[0].strip(),
                               ESC_NO_COLOR)
                       else:
                           print "{}{}:{}".format(OPTION_FILE_AS_URIS,entry.filename,problem[1])
                           print '{}{}{}'.format(ESC_YELLOW, problem[0], ESC_NO_COLOR)
                   else:
                       if OPTION_GCC:
                           print "{}{}:{}:1 {}".format(
                               OPTION_FILE_AS_URIS,
                               entry.filename,
                               problem[1],
                               problem[0].strip())
                       else:
                           print "{}{}:{}".format(OPTION_FILE_AS_URIS,entry.filename,problem[1])
                           print "{}".format(problem[0])
           print '\nMindForger Code Style Checker finished:'
           print '{}  [ERROR] {} problems in {} files found{}'.format(ESC_LIGHT_RED,reportEntries,len(self.report),ESC_NO_COLOR)

# Report Entry #######################################################################################

class ReportEntry:
    def __init__(self, filename):
        self.filename = filename
        self.problems = []

    def addWarning(self, problem, filename, line):
        sys.stdout.write(ESC_YELLOW)
        sys.stdout.write('.')
        sys.stdout.write(ESC_NO_COLOR); 
        sys.stdout.flush()
        self.addProblem(problem, filename, line);

    def addError(self, problem, filename, line):
        sys.stdout.write(ESC_LIGHT_RED)
        sys.stdout.write('.')
        sys.stdout.write(ESC_NO_COLOR); 
        sys.stdout.flush()
        self.addProblem(problem, filename, line);

    def addProblem(self, problem, filename, line):
        self.problems.append([problem, line, None])

# H/CPP ###########################################################################################

class CppFile:
    def __init__(self, filename):
        self.filename = filename

    def validate(self, report):
        reportEntry = ReportEntry(self.filename)
        self.validateCopyright(reportEntry)
        self.validateContent(reportEntry)
        if len(reportEntry.problems) > 0:
            report.append(reportEntry)

    def validateCopyright(self, reportEntry):
        with open(self.filename, 'r') as f:
            f.readline().strip()
            f.readline().strip()
            f.readline().strip()
            line = f.readline().strip()
        if COPYRIGHT_CPP != line:
            reportEntry.addProblem("  [WARNING] wrong copyright: '{}'".format(line), self.filename, 1)

    def validateContent(self, reportEntry):
        # TABs forbidden
        tab = re.compile("\t")
        # trailing spaces forbidden
        trailingSpace = re.compile("[ \t]+$")
        # no todo > TODO
        noTodo = re.compile(" todo")

        with open(self.filename) as f:
            l = 1
            for line in f:
                result = tab.search(line)
                if result is not None:
                    reportEntry.addProblem("  [WARNING] TABs on line: '{}'".format(line[:-1]), self.filename, l)
                result = trailingSpace.search(line)
                if result is not None:
                    reportEntry.addProblem("  [WARNING] trailing spaces on line: '{}'".format(line[:-1]), self.filename, l)
                result = noTodo.search(line)
                if result is not None:
                    reportEntry.addProblem("  [WARNING] forbidden 'todo' marker - use 'TODO' instead: '{}'".format(line[:-1]), self.filename, l)
                l+=1

# Python  ###########################################################################################

class PythonFile:
    def __init__(self, filename):
        self.filename = filename

    def validate(self, report):
        reportEntry = ReportEntry(self.filename)
        self.validateCopyright(reportEntry)
        self.validateContent(reportEntry)
        if len(reportEntry.problems) > 0:
            report.append(reportEntry)

    def validateCopyright(self, reportEntry):
        with open(self.filename, 'r') as f:
            f.readline().strip()
            f.readline().strip()
            f.readline().strip()
            line = f.readline().strip()
        if COPYRIGHT_PYTHON != line:
            reportEntry.addProblem("  [WARNING] wrong copyright: '{}'".format(line), self.filename, 1)

    def validateContent(self, reportEntry):
        # trailing spaces forbidden
        trailingSpace = re.compile("[ \t]+$")
        # no todo > TODO
        noTodo = re.compile(" todo")

        with open(self.filename) as f:
            l = 1
            for line in f:
                result = trailingSpace.search(line)
                if result is not None:
                    reportEntry.addProblem("  [WARNING] trailing spaces on line: '{}'".format(line[:-1]), self.filename, l)
                result = noTodo.search(line)
                if result is not None:
                    reportEntry.addProblem("  [WARNING] forbidden 'todo' marker - use 'TODO' instead: '{}'".format(line[:-1]), self.filename, l)
                l+=1

#############################################################################################

if len(sys.argv)>1: 
    for a in sys.argv:
        if '--color' == a or '-c' == a:
            ESC_LIGHT_RED="[1;31m"
            ESC_LIGHT_GREEN="[1;32m"
            ESC_YELLOW="[1;33m"
            ESC_WHITE="[1;37m"
            ESC_NO_COLOR="[0m"
        if '--gcc' == a or '-g' == a:
            OPTION_GCC=True
        if '--link' == a or '-l' == a:
            OPTION_FILE_AS_URIS='file://'
        #if '--fix-trailing-spaces' == a:
        #    FIX_TRAILING_SPACES = True
        if '--help' == a or '-h' == a:
            print helpString.format()
            quit()

cwd = os.path.join(os.getcwd(),'../..')
enforcer = CodestyleEnforcer(cwd)
enforcer.loadFiles()
if OPTION_FIX_TRAILING_SPACES:
    enforcer.fixTrailingSpaces()
else:
    enforcer.validate()

# eof

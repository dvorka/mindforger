#!/usr/bin/env python
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

'''
Code style checker
'''

import os
import re
import sys
import subprocess
import xml.etree.ElementTree as ET

__version__ = "1.0.0-dev"
__notes__ = "development version"

# options
OPTION_BLAME = False

# conventions
RULE_CLASS_MAX_LINES = 1500
RULE_FILE_CPP_MAX_LINE_LENGHT = 1500

# files to SKIP from validation
BLACKLIST = {
    'NOTHING'
}

COPYRIGHT_CPP = 'Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>'
COPYRIGHT_PROPERTY = '<!-- Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com> -->'

LIGHT_RED="[1;31m"
LIGHT_GREEN="[1;32m"
YELLOW="[1;33m"
WHITE="[1;37m"
NO_COLOR="[0m"



class CodestyleEnforcer:
    '''
    Main class - finds all files and validate them
    '''

    def __init__(self, mavenRepositoryRootDir):
        print 'MindForger Codestyle Checker'
        self.mavenRepositoryRootDir = mavenRepositoryRootDir
        self.cppFiles = {}
        self.propertyFiles = {}
        self.report = []

    def loadFiles(self):
        for dirName, subdirList, fileList in os.walk(self.mavenRepositoryRootDir):
            for f in fileList:
                    fileName = os.path.join(dirName, f).replace('packaging/build/../../','')
                    relativeFileName = fileName[len(self.mavenRepositoryRootDir)+1:]
                    if relativeFileName not in BLACKLIST:
                        if f.endswith('.h') or f.endswith('.cpp'):
                            cpp = CppFile(fileName)
                            self.cppFiles[fileName] = cpp
                        elif f.endswith('.ts'):
                            p = PropertyFile(fileName)
                            self.propertyFiles[fileName] = p

    def validate(self):
        self.validateApiImpl()
        self.showReport()

    def validateApiImpl(self):
       sys.stdout.write('\n  Checking H/CPP sources')
       for f in self.cppFiles.keys():
           self.cppFiles[f].validate(self.report)
       sys.stdout.write('\n  Checking property files')
       for f in self.propertyFiles.keys():
           self.propertyFiles[f].validate(self.report)
       print ''

    def showReport(self):
       if len(self.report) == 0:
           print '\nMindForger Codestyle Checker finished:'
           print '{}  [INFO] successfuly checked with no issues{}'.format(LIGHT_GREEN, NO_COLOR)
       else:
           print '\nReport:'
           for entry in self.report:
               print entry.filename
               for problem in entry.problems:
                   isError = re.compile("ERROR")
                   isWarning = re.compile("WARNING")
                   isInfo = re.compile("INFO")
                   result = isError.search(problem[0])
                   if result is not None:
                       print "{}{}{}".format(LIGHT_RED, problem[0], NO_COLOR)
                   else:
                       result = isWarning.search(problem[0])
                       if result is not None:
                           print "{}{}{}".format(YELLOW, problem[0], NO_COLOR)
                       else:
                           result = isInfo.search(problem[0])
                           if result is not None:
                               print "{}{}{}".format(LIGHT_GREEN, problem[0], NO_COLOR)
                       
                   if problem[1] != None:
                       print problem[1]
           print '\nMindForger Codestyle Checker finished:'
           print '  [ERROR] {} files w/ problems found'.format(len(self.report))

class ReportEntry:

    def __init__(self, filename):
        self.filename = filename
        self.problems = []

    def addProblem(self, problem, filename, line):
        sys.stdout.write('.')
        sys.stdout.flush()
        if OPTION_BLAME:
            blame = self.addBlame(filename, line)
            self.problems.append([problem, blame])
        else:
            self.problems.append([problem, None])

    def addBlame(self, filename, line):
        lineRange = "-L{},+1".format(line)
        try:
            blame = subprocess.check_output(["git", "blame", lineRange, filename])
            begin = blame.find('(') + 1
            end = blame.find(')') - 1
            blame = blame[begin:end]
            return '    {}'.format(blame)
        except subprocess.CalledProcessError as e:
            pass

# H/CPP ###########################################################################################

class CppFile:
    LIMIT_LINE_LNG = 1500
    LIMIT_MAX_LINES = 1500

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
                if len(line) > self.LIMIT_LINE_LNG:
                    reportEntry.addProblem("  [WARNING] line too long {} > {}".format(len(line), self.LIMIT_LINE_LNG), self.filename, 1)
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
            if l > self.LIMIT_MAX_LINES:
                reportEntry.addProblem("  [WARNING] file has too many lines {} > {}".format(l, self.LIMIT_MAX_LINES), self.filename, 1)

# Property ###########################################################################################

class PropertyFile:
    filename = None

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
            firstLine = f.readline().strip()
        if COPYRIGHT_PROPERTY != firstLine:
            reportEntry.addProblem("  [WARNING] wrong copyright: '{}'".format(firstLine), self.filename, 1)

    def validateContent(self, reportEntry):
        trailingSpace = re.compile("[ \t]+$")
        with open(self.filename) as f:
            for line in f:
                l = 1
                result = trailingSpace.search(line)
                if result is not None:
                    reportEntry.addProblem("  [WARNING] trailing spaces on line: '{}'".format(line[:-1]), self.filename, l)
                l+=1

#############################################################################################

cwd = os.path.join(os.getcwd(),'../..')
enforcer = CodestyleEnforcer(cwd)
enforcer.loadFiles()
enforcer.validate()

# eof

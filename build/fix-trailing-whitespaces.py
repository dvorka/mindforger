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
This script fixes coding style violations in source code files.

Make sure that all your changes are commited and git status
reports no changes so that you can review all the fixes and
easily rollback anything suspicious.

Emacs: run this script with: M-x compile ./fix-trailing-whitespaces.py
'''

import os
import sys
import re

__version__ = "1.0.0-dev"
__notes__ = "development version"

class FixItFelix:
    '''
    Main class
    '''

    def __init__(self, gitRepositoryRootDir):
        print 'Coding style violations detection and fix tool'
        self.gitRepositoryRootDir = gitRepositoryRootDir
        self.cppFiles = {}
        self.pythonFiles = {}
        self.report = []

    def loadFiles(self):
        for dirName, subdirList, fileList in os.walk(self.gitRepositoryRootDir):
            for f in fileList:
                    fileName = os.path.join(dirName, f).replace('packaging/build/../../','')
                    relativeFileName = fileName[len(self.gitRepositoryRootDir)+1:]
                    if f.endswith('.h') or f.endswith('.cpp'):
                        cpp = CppFile(fileName)
                        self.cppFiles[fileName] = cpp
                    elif f.endswith('.py'):
                        py = PythonFile(fileName)
                        self.pythonFiles[fileName] = py

    def validate(self):
       sys.stdout.write('\nChecking H/CPP source files...')
       for f in self.cppFiles.keys():
           self.cppFiles[f].validate(self.report)
       print ''
       sys.stdout.write('\nChecking Python source files...')
       for f in self.pythonFiles.keys():
           self.pythonFiles[f].validate(self.report)
       print ''

    def fix(self):
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

# H/CPP ###########################################################################################

class CppFile:
    def __init__(self, filename):
        self.filename = filename

    def validate(self, reportEntry):
        trailingSpace = re.compile("[ \t]+$")
        with open(self.filename) as f:
            l = 1
            for line in f:
                result = trailingSpace.search(line)
                if result is not None:
                    sys.stdout.write("\n{}:{}:1\n  Trailing spaces: '{}'".format(self.filename, l, line[:-1]))
                l+=1

# Python ###########################################################################################

class PythonFile:
    filename = None

    def __init__(self, filename):
        self.filename = filename

    def validate(self, reportEntry):
        trailingSpace = re.compile("[ \t]+$")
        with open(self.filename) as f:
            l = 1
            for line in f:
                result = trailingSpace.search(line)
                if result is not None:
                    sys.stdout.write("\n{}:{}:1\n  Trailing spaces: '{}'".format(self.filename, l, line[:-1]))
                l+=1

#############################################################################################

cwd = os.path.join(os.getcwd(),'..')
felix = FixItFelix(cwd)
felix.loadFiles()
felix.validate()

# eof

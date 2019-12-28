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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Usage:
#
#   generate.py SubMenuName
#

import sys

TEMPLATE_SUBMENU_DECL='''    actionFormatMath{}= new QAction(tr("&{}"), mainWindow);
    submenuFormatMath->addAction(actionFormatMath{});'''

#####################################################################

if len(sys.argv) == 2:
    arg = sys.argv[1]
    print(TEMPLATE_SUBMENU_DECL.format(arg, arg, arg))
else:
    ms = ('Sum', 'Int','Iiint','Alpha','Beta','Delta','Gama','Text','Bar','Hat','Dot','Overrightarrow','Cup','Cap','Emptyset','In','Notin','Sqrt')
    for i in ms:
        print(TEMPLATE_SUBMENU_DECL.format(i, i, i))

    for i in ms:
        print("    QAction* actionFormatMath{};".format(i))

    for i in ms:
        print("    QObject::connect(view->actionFormatMath{}, SIGNAL(triggered()), mwp, SLOT(doActionFormatMath{}()));".format(i, i))

    for i in ms:
        print("    void doActionFormatMath{}();".format(i))

    for i in ms:
        print("void MainWindowPresenter::doActionFormatMath{}()".format(i))
        print("{")
        print('    insertMarkdownText(".", false, 3);')
        print("}")

# EOF

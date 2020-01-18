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

# Usage:
#
#   generate.py Class_Name_With_Underscores
#

import sys

TEMPLATE_HEADER_FILE='''/*
 {}.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_{}_H
#define M8RUI_{}_H

#include <QtWidgets>

namespace m8r {{

class {} : public QObject
{{
    Q_OBJECT

public:
    explicit {}(QWidget* parent);
    {}(const {}&) = delete;
    {}(const {}&&) = delete;
    {} &operator=(const {}&) = delete;
    {} &operator=(const {}&&) = delete;
    ~{}();
}};

}}
#endif // M8RUI_{}_H
'''

TEMPLATE_CPP_FILE='''/*
 {}.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "{}.h"

namespace m8r {{

{}::{}(QWidget* parent)
{{
}}

{}::~{}()
{{
}}

}} // m8r namespace
'''

#####################################################################

if len(sys.argv) == 2:
    arg = sys.argv[1]
    nameCamel = arg.replace("_","")
    nameDash = arg.lower()
    nameMacro = arg.upper()
    with open(nameDash+".h", "w") as text_file:
        text_file.write(TEMPLATE_HEADER_FILE.format(
            nameDash,
            nameMacro,nameMacro,
            nameCamel,
            nameCamel,nameCamel,nameCamel,nameCamel,nameCamel,nameCamel,nameCamel,nameCamel,nameCamel,nameCamel,
            nameMacro))

    with open(nameDash+".cpp", "w") as text_file:
        text_file.write(TEMPLATE_CPP_FILE.format(
            nameDash,
            nameDash,
            nameCamel,nameCamel,nameCamel,nameCamel))
else:
    print "\nUsage: gen-cpp-file.py Note_Edit\n"



# EOF

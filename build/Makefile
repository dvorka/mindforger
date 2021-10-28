# Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>
#
# MindForger thinking notebook
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
#

default: test-lib-units

help:
	@echo "MindForger builder help:"
	@echo "test-lib                 compile and run lib/ unit tests"
	@echo "test-app                 compile and run app/ integration tests"
	@echo "doc                      generate Doxygen documentation"

test-lib:
	./test-lib-units.sh

doc:
	cd doxygen && doxygen ./mindforger.cfg

# Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>
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

.DEFAULT_GOAL := help

MINDFORGER_VERSION := 1.53.0
MINDFORGER_RELEASE_BASE_DIR := /home/dvorka/p/mindforger/release
MINDFORGER_RELEASE_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/$(MINDFORGER_VERSION)-maker

MF_MAKER_WORKING_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/maker-at-work

# class name like My_Class_Name used by C++ class generators
CLASS_NAME := "New_Class"

help:
	@echo "MindForger maker help:"
	@echo "clean         clean build artifacts"
	@echo "gen-lib-class generate lib C++ class skeleton for CLASS_NAME=My_Class"
	@echo "gen-ui-class  generate UI C++ class skeleton CLASS_NAME=My_Class"
	@echo "build         build MindForger executable"
	@echo "l10n          update and release localization strings"
	@echo "test-lib      compile and run lib/ unit tests"
	@echo "test-app      compile and run app/ integration tests"
	@echo "dist-all      build all distributions"
	@echo "dist-tarball  build tarball distribution"
	@echo "dist-deb      build Debian distribution"
	@echo "statistic     show source code statistic"
	@echo "doc           generate Doxygen documentation"

clean:
	rm -vf ../app/mindforger
	rm -vf ../lib/libmindforger.a
	rm -vf ../lib/test/src/mindforger-lib-unit-tests
	cd .. && make clean
	cd ../lib/test && make clean

gen-lib-class:
	@echo "Generating lib C++ class for name: $(CLASS_NAME)"
	./gen-cpp-class.py $(CLASS_NAME)

gen-ui-class:
	@echo "Generating UI C++ class for name: $(CLASS_NAME)"
	./gen-cpp-ui-class.py $(CLASS_NAME)


build: clean
	cd .. && qmake -r mindforger.pro && make -j 7
	@echo "If build succeeded then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger

install-local-dvorka: build ../app/mindforger
	cp -vf ../app/mindforger ~/bin
	mv -vf ~/bin/mindforger ~/bin/MIND
	~/bin/MIND --version

l10n:
	./l10n-update-strings.sh
	./l10n-edit-and-release.sh

test-lib: clean
	./test-lib-units.sh

dist-work-clean:
	rm -rvf $(MF_MAKER_WORKING_DIR)

$(MF_MAKER_WORKING_DIR):
	mkdir -v $(MF_MAKER_WORKING_DIR)

$(MINDFORGER_RELEASE_DIR):
	mkdir -v $(MINDFORGER_RELEASE_DIR) || echo "$(MINDFORGER_RELEASE_DIR) already exists"

dist-tarball: dist-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR)
	@echo "Building TARBALL distribution..."
	cp -vf ./tarball/tarball-build.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./tarball-build.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION).tgz $(MINDFORGER_RELEASE_DIR)

dist-deb: dist-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR)
	@echo "Building DEB distribution..."
	cp -vf ./debian/debian-make-deb.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./debian-make-deb.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION)-1_amd64.deb $(MINDFORGER_RELEASE_DIR)

dist-rpm:
	@echo "Building RPM distribution..."
	@echo "VIRTUAL MACHINE with RedHat must be running!"
	# TODO to be implemented
	exit 1

dist-all-clean:
	rm -rvf $(MINDFORGER_RELEASE_DIR)

dist-all: dist-all-clean $(MINDFORGER_RELEASE_DIR) dist-tarball dist-deb
	@echo "Building all $(MINDFORGER_VERSION) distributions"

statistic:
	./statistic.sh

doc:
	cd doxygen && doxygen ./mindforger.cfg
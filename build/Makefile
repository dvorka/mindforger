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

MINDFORGER_VERSION := 1.54.0
MINDFORGER_RELEASE_BASE_DIR := /home/dvorka/p/mindforger/release
MINDFORGER_RELEASE_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/$(MINDFORGER_VERSION)-maker

MF_MAKER_WORKING_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/maker-at-work

# class name like My_Class_Name used by C++ class generators
CLASS_NAME := "New_Class"
# l10n language: en, cs
MF_LANG := "en"

help:
	@echo "MindForger maker help:"
	@echo "git-subs-update   update git submodules"
	@echo "gen-lib-class     generate lib C++ class skeleton: CLASS_NAME=My_Class"
	@echo "gen-ui-class      generate UI C++ class skeleton: CLASS_NAME=My_Class"
	@echo "devenv-debian     install development environment on Debian"
	@echo "clean             clean build artifacts"
	@echo "build             build MindForger binary"
	@echo "l10n              update and release localization strings: MF_LANG=en"
	@echo "test-lib          compile and run lib/ unit tests"
	@echo "test-app          compile and run app/ integration tests"
	@echo "dist-all          build all distributions"
	@echo "dist-tarball      build tarball distribution"
	@echo "dist-deb          build Debian distribution"
	@echo "dist-rpm          build .rpm package on Fedora"
	@echo "dist-dmg          build macOS Disk iMaGe .dmg package"
	@echo "dist-debian-ppa   create PPA for all Debian releases"
	@echo "statistic         show source code statistic"
	@echo "doc-to-wiki       mindforger-documentation to mindforger.wiki"
	@echo "api-reference     generate Doxygen documentation"
	@echo "dev-install-local compile and install binary to ~/bin as MIND"

.PHONY: clean
clean:
	rm -vf ../app/mindforger
	rm -vf ../lib/libmindforger.a
	rm -vf ../lib/test/src/mindforger-lib-unit-tests
	cd .. && make clean
	cd ../lib/test && make clean

.PHONY: git-subs-update
git-subs-update:
	cd .. && git submodule update --init --recursive

.PHONY: gen-lib-class
gen-lib-class:
	@echo "Generating lib C++ class for name: $(CLASS_NAME)"
	./make/gen-cpp-class.py $(CLASS_NAME)

.PHONY: gen-ui-class
gen-ui-class:
	@echo "Generating UI C++ class for name: $(CLASS_NAME)"
	./make/gen-cpp-ui-class.py $(CLASS_NAME)

.PHONY: devenv-debian
devenv-debian:
	sudo apt-get install build-essential zlib1g-dev libhunspell-dev libqt5webkit5-dev qttools5-dev-tools ccache cmake debhelper

build: clean
	cd .. && qmake -r mindforger.pro && make -j 7
	@echo "If build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger

build-ci: clean
	@echo "MindForger CI build..."
	cd .. && qmake CONFIG+=mfci -r mindforger.pro && make -j 7
	@echo "If CI build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger

.PHONY: l10n
l10n:
	cd make && ./l10n-update-strings.sh && ./l10n-edit-and-release.sh $(MF_LANG)

test-lib: clean
	cd make && ./test-lib-units.sh

.PHONY: dist-work-clean
dist-work-clean:
	rm -rvf $(MF_MAKER_WORKING_DIR)

$(MF_MAKER_WORKING_DIR):
	mkdir -vp $(MF_MAKER_WORKING_DIR)

$(MINDFORGER_RELEASE_DIR):
	mkdir -v $(MINDFORGER_RELEASE_DIR) || echo "$(MINDFORGER_RELEASE_DIR) already exists"

dist-tarball: dist-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR)
	@echo "Building TARBALL distribution..."
	mkdir -vp $(MF_MAKER_WORKING_DIR)
	cp -vf ./tarball/tarball-build.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./tarball-build.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION)_tarball.tgz $(MINDFORGER_RELEASE_DIR)

dist-deb: dist-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR)
	@echo "Building DEB distribution..."
	mkdir -vp $(MF_MAKER_WORKING_DIR)
	cp -vf ./debian/debian-make-deb.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./debian-make-deb.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION)-1_amd64.deb $(MINDFORGER_RELEASE_DIR)

.PHONY: dist-rpm
dist-rpm:
	@echo "IMPORTANT: this target MUST be run on Fedora!"
	cd fedora && ./fedora-distro-setup.sh
	@echo "Building .rpm package..."
	cp -vf ./fedora-rpm-from-deb.sh ~/alien && cd ~/alien && sudo ./fedora-rpm-from-deb.sh $(MINDFORGER_VERSION)
	@echo "Find .rpm in ~/alien directory"

.PHONY: dist-dmg
dist-dmg:
	@echo "Building .dmg package..."
	cd macos && ./mindforger-build.sh && ./dmg-package-build.sh

#
# Debian
#
# MindForger PPA: https://www.mindforger.com/debian is managed by aptly
#
# - information about MindForger's Debian PPAs can be found in
#   https://www.mindforger.com/debian/index.html
#
# - aptly CANNOT be used to build one PPA for all Debian releases
#   therefore every release has it's own PPA
# 
# - the structure looks like this (from deprecated debian/ to new debian-ppa/):
#
#   https://www.mindforger.com/debian-ppa/
#     stretch/
#       dists/      ... export from ~/.aptly/public
#       pool/
#       index.html  ... created by me w/ how to set it up
#     ...
#     trixie/
#       dists/      ... export from ~/.aptly/public
#       pool/
#       index.html  ... created by me w/ how to set it up
#
# - source Debian packages for every version (which are used by aptly)
#   are stored in 
#
#   ~/p/mindforger/debian/aptly/
#     hstr/
#       9-stretch/
#       ...
#       13-trixie/
#     mindforger/
#       9-stretch/
#         mindforger-*.*.*.deb
#       ...
#       13-trixie/
#         mindforger-*.*.*.deb
#
# - Makefile targets below are used to manage Debian PPAs and generate them
#

# future repos  : trixie-main
# CURRENT repos : bookworm-main, bullseye-main, buster-main, stretch-main
# obsolete repos: jessie-main

.PHONY: dist-debian-ppa
dist-debian-aptly-create-ppa:
	cd ./build/debian && ./debian-ppa-aptly-build.sh

#
# ^ Debian
#

.PHONY: dist-all-clean
dist-all-clean:
	rm -rvf $(MINDFORGER_RELEASE_DIR)

.PHONY: dist-all
dist-all: dist-all-clean $(MINDFORGER_RELEASE_DIR) dist-tarball dist-deb
	@echo "Building all $(MINDFORGER_VERSION) distributions"

.PHONY: statistic
statistic:
	cd make && ./statistic.sh

.PHONY: doc-to-wiki
doc-to-wiki:
	cd doc && ./mf-doc-to-wiki.py

.PHONY: api-reference
api-reference:
	cd doxygen && doxygen ./mindforger.cfg

# private development targets

dev-install-local: build-ci ../app/mindforger
	cp -vf ../app/mindforger ~/bin
	mv -vf ~/bin/mindforger ~/bin/mind
	~/bin/mind --version

# eof

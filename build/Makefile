# Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>
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

#
# variables
#

MINDFORGER_VERSION := 2.1.0
MINDFORGER_RELEASE_BASE_DIR := /home/dvorka/p/mindforger/release
MINDFORGER_RELEASE_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/$(MINDFORGER_VERSION)-maker

MF_MAKER_WORKING_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/maker-at-work

# class name like My_Class_Name used by C++ class generators
CLASS_NAME := "New_Class"
# l10n language: en, cs
MF_LANG := "en"
# Ubuntu distro: trusty xenial bionic focal jammy kinetic
DISTRO := "bionic"
# CPU cores thant can be used to build the project
ifeq ($(shell hostname), skunkworks)
  CPU_CORES := 10
else
  CPU_CORES := 7
endif
# Qt version to be used by MindForger
# MF_QT_VERSION := 5.9.9
MF_QT_VERSION := 5.15.2

#
# OS detection
#

# uname returns the operating system name: Darwin, Linux, etc.
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
  # this sets QTDIR and PATH shell (!) and make environment variables on macOS
  QTDIR := /Users/dvorka/Qt/$(MF_QT_VERSION)/clang_64
  PATH := /Users/dvorka/Qt/$(MF_QT_VERSION)/clang_64/bin:/Library/Developer/CommandLineTools/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:$(PATH)
endif


#
# targets
#


.PHONY: diagnostics
diagnostics:
	@echo "MindForger diagnostics:"
	@echo "  MINDFORGER_VERSION: $(MINDFORGER_VERSION)"
	@echo "  MINDFORGER_RELEASE_DIR: $(MINDFORGER_RELEASE_DIR)"
	@echo "  MF_MAKER_WORKING_DIR: $(MF_MAKER_WORKING_DIR)"
	@echo "  CLASS_NAME: $(CLASS_NAME)"
	@echo "  MF_LANG: $(MF_LANG)"
	@echo "  DISTRO: $(DISTRO)"
	@echo "  CPU_CORES: $(CPU_CORES)"
	@echo "  PATH: ${PATH}"
	@echo "  QTDIR: ${QTDIR}"


.PHONY: help
help: ## make targets help
	@echo "MindForger Swiss knife help:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
	| sort \
	| awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'


.PHONY: clean
clean: ## clean build artifacts
	rm -vf ../app/mindforger ../app/*.o
	rm -vf ../lib/libmindforger.a
	rm -vf ../lib/test/src/mindforger-lib-unit-tests
	cd .. && make clean
	cd ../lib/test && make clean


.PHONY: clean-app
clean-app:
	rm -vf ../app/mindforger


.PHONY: git-subs-update
git-subs-update: ## update Git submodules
	@echo "Initializing sub-modules with cmark-gfm and other dependencies..."
	cd .. && git submodule update --init --recursive


../deps/cmark-gfm/build: git-subs-update
	@echo "Building cmark-gfm and other dependencies..."
	cd ../deps/cmark-gfm \
	&& mkdir -p build && cd build \
	&& cmake -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF .. \
	&& cmake --build . \
	; cd ../../../build


.PHONY: gen-lib-class
gen-lib-class: ## generate lib C++ class skeleton: CLASS_NAME=My_Class
	@echo "Generating lib C++ class for name: $(CLASS_NAME)"
	./make/gen-cpp-class.py $(CLASS_NAME)


.PHONY: gen-ui-class
gen-ui-class: ## generate UI C++ class skeleton: CLASS_NAME=My_Class
	@echo "Generating UI C++ class for name: $(CLASS_NAME)"
	./make/gen-cpp-ui-class.py $(CLASS_NAME)


#
# build
#


../Makefile:
	@echo "Generating Makefile..."
	cd .. && qmake -r mindforger.pro ; cd build


../app/mindforger: ../Makefile
	@echo "Building PRODUCTION MindForger executable..."
	cd .. && make -j $(CPU_CORES) ; cd build
	@echo "If build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger


build: clean-app ../app/mindforger ## build production MindForger application binary
	ls -al ../app/mindforger


.PHONY: build-dev
build-dev: clean-app ## build development MindForger application binary
	@echo "Building DEV MindForger executable..."
	cd .. && qmake -r mindforger.pro DEFINES+=DO_MF_DEBUG && make -j $(CPU_CORES) ; cd ..
ifeq ($(UNAME), Darwin)
	@echo "\nIf build succeeded, then MindForger executable can be found in:\n  app/mindforger.app/Contents/MacOS/mindforger"
	ls -al ../app/mindforger.app/Contents/MacOS/mindforger
else
	@echo "\nIf build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger
endif


.PHONY: build-rc
build-rc: clean-app ## build RC MindForger application binary
	@echo "MindForger RC build..."
	cd .. && qmake CONFIG+=mfrc -r mindforger.pro && make -j $(CPU_CORES)
	@echo "If RC build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger


.PHONY: build-ci
build-ci: clean-app ## build CI MindForger application binary
	@echo "MindForger CI build..."
	cd .. && qmake CONFIG+=mfci -r mindforger.pro && make -j $(CPU_CORES)
	@echo "If CI build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger


#
# run
#


run: ../app/mindforger ## run production MindForger
	# cd ../app && pwd && ./mindforger
	cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/library-trainer
	# cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/computer-hw.md
	# cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/bug-copy-image


run-rc: build-rc ## run MindForger RC build
	cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/library-trainer


run-ci: build-ci ## run MindForger CI build
	cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/library-trainer


run-dev: build-dev ## run MindForger development build
ifeq ($(UNAME), Darwin)
	cd ../app/mindforger.app/Contents/MacOS && ./mindforger /Users/dvorka/mf-devel/mf-copy
else
	cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/library-trainer
	# cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/computer-hw.md
	# cd ../app && pwd && ./mindforger /home/dvorka/mf-devel/bug-copy-image
endif

#
# install
#


.PHONY: devenv-ubuntu
devenv-ubuntu:
	sudo apt-get install build-essential zlib1g-dev libhunspell-dev libcurl4-gnutls-dev libqt5webkit5-dev qttools5-dev-tools qt5-default ccache cmake


devenv-ubuntu-2204:
	sudo apt-get install build-essential zlib1g-dev libhunspell-dev libcurl4-gnutls-dev libqt5webkit5-dev qttools5-dev-tools qtbase5-dev qt5-qmake ccache cmake


.PHONY: devenv-debian
devenv-debian:
	sudo apt-get install build-essential zlib1g-dev libcurl4-gnutls-dev libhunspell-dev libqt5webkit5-dev qttools5-dev-tools ccache cmake debhelper


install-dev-local: ../deps/cmark-gfm/build ../Makefile ../lib/tests/Makefile clean build-rc ## install MindForger RC build to ~/bin as 'mind' executable
	cp -vf ../app/mindforger ~/bin
	mv -vf ~/bin/mindforger ~/bin/mind
	~/bin/mind --version


#
# tools
#


.PHONY: localization
localization: ## update and release localization strings: MF_LANG=en
	cd make && ./l10n-update-strings.sh && ./l10n-edit-and-release.sh $(MF_LANG)


.PHONY: ver-find
ver-find: ## pre-version update finder
	cd .. && git grep -n "1\.55"


../lib/tests/Makefile:
	@echo "Generating lib test Makefile..."
	cd ../lib/test && qmake -r mindforger-lib-unit-tests.pro ; cd ../../build


test-lib: clean ## compile and run lib/ unit tests
	cd make && ./test-lib-units.sh


.PHONY: dist-work-clean
dist-work-clean:
	rm -rvf $(MF_MAKER_WORKING_DIR)


$(MF_MAKER_WORKING_DIR):
	mkdir -vp $(MF_MAKER_WORKING_DIR)


$(MINDFORGER_RELEASE_DIR):
	mkdir -v $(MINDFORGER_RELEASE_DIR) || echo "$(MINDFORGER_RELEASE_DIR) already exists"


dist-tarball: dist-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR) ## build tarball distribution
	@echo "Building TARBALL distribution..."
	mkdir -vp $(MF_MAKER_WORKING_DIR)
	cp -vf ./tarball/tarball-build.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./tarball-build.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION)_tarball.tgz $(MINDFORGER_RELEASE_DIR)


dist-deb: dist-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR) ## build Debian distribution
	@echo "Building DEB distribution..."
	mkdir -vp $(MF_MAKER_WORKING_DIR)
	cp -vf ./debian/debian-make-deb.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./debian-make-deb.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION)-1_amd64.deb $(MINDFORGER_RELEASE_DIR)


.PHONY: dist-rpm
dist-rpm: ## build .rpm package on Fedora
	@echo "IMPORTANT: this target MUST be run on Fedora!"
	cd fedora && ./fedora-distro-setup.sh
	@echo "Building .rpm package..."
	cp -vf ./fedora-rpm-from-deb.sh ~/alien && cd ~/alien && sudo ./fedora-rpm-from-deb.sh $(MINDFORGER_VERSION)
	@echo "Find .rpm in ~/alien directory"


.PHONY: dist-dmg
dist-dmg: ## build macOS Disk iMaGe .dmg package
	@echo "Building .dmg package..."
	cd macos && ./mindforger-build.sh && ./dmg-package-build.sh


#
# Debian
#
# OBSOLETE: MindForger PPA: https://www.mindforger.com/debian is managed by aptly
#
# - OBSOLETE: information about MindForger's Debian PPAs can be found in
#   https://www.mindforger.com/debian/index.html
#
# - aptly CANNOT be used to build one PPA for all Debian releases
#   therefore every release has its own PPA
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
# - Debian distributions are built in VirtualBox VMs on "mind" machine
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
dist-debian-aptly-create-ppa:  ## create Debian PPA for all releases
	cd ./debian && ./debian-ppa-aptly-build.sh

#
# ^ Debian
#


.PHONY: dist-ubuntu-deb
dist-ubuntu-deb: ## locally build .deb for any Ubuntu version: DISTRO=bionic
	@echo "Building Ubuntu $(DISTRO) .deb ..."
	@echo "Copying latest version of .sh script to launchpad/ ..."
	@echo "Running .sh script ..."
	@echo "Copying .deb from build area to the current directory ..."
	false


.PHONY: dist-snap
dist-snap: ## build snap distro for snapcraft.io
	@echo "Building Snap ..."
	cd .. && make clean && cd build
	cp -vf snap/snapcraft.yaml .. && cd .. && snapcraft clean && snapcraft --debug


.PHONY: dist-wip
dist-snap-wip:  # debug Snap build w/o clean up, but w/ incremental build
	@echo "Building Snap ..."
	cp -vf snap/snapcraft.yaml .. && cd .. && snapcraft --debug


.PHONY: dist-all-clean
dist-all-clean:
	rm -rvf $(MINDFORGER_RELEASE_DIR)


dist-all: dist-all-clean $(MINDFORGER_RELEASE_DIR) dist-tarball dist-deb ## build all distributions
	@echo "Building all $(MINDFORGER_VERSION) distributions"


.PHONY: statistic
statistic: ## show source code statistic
	cd make && ./statistic.sh


.PHONY: doc-to-wiki
doc-to-wiki: ## mindforger-documentation to mindforger.wiki
	cd doc && ./mf-doc-to-wiki.py


.PHONY: doc-api-reference
api-reference: ## generate Doxygen source code documentation
	cd doxygen && doxygen ./mindforger.cfg

# eof

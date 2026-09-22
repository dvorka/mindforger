# Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>
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

# lib/src/app_info.h (MINDFORGER_VERSION_STRING) is the authoritative version source
MINDFORGER_VERSION := $(shell sed -n 's/.*MINDFORGER_VERSION_STRING "\(.*\)"/\1/p' ../lib/src/app_info.h)
MINDFORGER_VERSION_MAJOR_MINOR := $(word 1,$(subst ., ,$(MINDFORGER_VERSION))).$(word 2,$(subst ., ,$(MINDFORGER_VERSION)))
MINDFORGER_PREVIOUS_MINOR_VERSION := $(shell echo $(MINDFORGER_VERSION) | awk -F. '{minor=$$2-1; if (minor<0) minor=0; print $$1"."minor".0"}')
MINDFORGER_RELEASE_BASE_DIR := /home/dvorka/p/mindforger/release
MINDFORGER_RELEASE_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/$(MINDFORGER_VERSION)-maker

MF_MAKER_WORKING_DIR := $(MINDFORGER_RELEASE_BASE_DIR)/maker-at-work

# class name like My_Class_Name used by C++ class generators
CLASS_NAME := "New_Class"
# l10n language: cs, en, es, hi, zh_cn
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
	@echo " __  __ _           _ _____                         "
	@echo "|  \\/  (_)_ __   __| |  ___|__  _ __ __ _  ___ _ __ "
	@echo "| |\\/| | | '_ \\ / _\` | |_ / _ \\| '__/ _\` |/ _ \\ '__|"
	@echo "| |  | | | | | | (_| |  _| (_) | | | (_| |  __/ |   "
	@echo "|_|  |_|_|_| |_|\\__,_|_|  \\___/|_|  \\__, |\\___|_|   $(MINDFORGER_VERSION)"
	@echo "                                     |___/            "
	@echo "MindForger Swiss knife:"
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
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
	@echo "If build succeeded, then MindForger executable can be found in:\n  ../app/mindforger"
	@echo ""
	ls -al ../app/mindforger


build: clean-app ../app/mindforger ## build production MindForger application binary
	ls -al ../app/mindforger


.PHONY: build-dev
build-dev: clean-app ## build development MindForger application binary w/ debug info
	@echo "Building DEV MindForger executable..."
	cd .. && qmake -r mindforger.pro CONFIG+=debug DEFINES+=DO_MF_DEBUG && make -j $(CPU_CORES) ; cd ..
ifeq ($(UNAME), Darwin)
	@echo "\nIf build succeeded, then MindForger executable can be found in:\n  app/mindforger.app/Contents/MacOS/mindforger"
	ls -al ../app/mindforger.app/Contents/MacOS/mindforger
else
	@echo "\nIf build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger
endif

.PHONY: build-webkit
build-webkit: clean-app ## build MindForger application binary w/ legacy Qt WebKit instead of default Qt WebEngine (Linux only)
	@echo "Building MindForger executable with legacy Qt WebKit support..."
	@echo "Prerequisite: Qt WebKit must be installed: sudo apt-get install -y libqt5webkit5-dev"
	cd .. && qmake -r mindforger.pro CONFIG+=mfwebkit && make -j $(CPU_CORES) ; cd ..
	@echo "If build succeeded, then MindForger executable can be found in:\n  app/mindforger"
	ls -al ../app/mindforger


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


run-gdb: clean build-dev ## run MindForger using gdb
ifeq ($(UNAME), Darwin)
	cd ../app/mindforger.app/Contents/MacOS && ./mindforger /Users/dvorka/mf-devel/mf-copy
else
	cd ../app && pwd && gdb --args ./mindforger /home/dvorka/mf-devel/library-trainer
endif


run-production: build-rc ## run MindForger on the production knowledge base
	cd ../app && pwd && ./mindforger /home/dvorka/mf


#
# Windows
#
# Targets below wrap the .bat scripts in build/windows/ (env.bat, build-*.bat,
# run-*.bat). They only work when run on Windows (e.g. from Git Bash with GNU
# Make installed via Chocolatey/winget) since they invoke MSVC/qmake/nmake.
#

.PHONY: win-build-cmake
win-build-cmake: ## Windows: build the cmark-gfm dependency (Release+Debug)
	cd windows && ./build-cmake.bat
	@echo "If build succeeded, then cmark-gfm libs can be found in:\n  ../deps/cmark-gfm/build/src/{Release,Debug}\n  ../deps/cmark-gfm/build/extensions/{Release,Debug}"

.PHONY: win-build-app
win-build-app: ## Windows: build MindForger release application (qmake + nmake)
	cd windows && ./build-app.bat
	@echo "If build succeeded, then MindForger executable can be found in:\n  ../app/release/mindforger.exe"
	@echo ""
	ls -al ../app/release/mindforger.exe

.PHONY: win-build-installer
win-build-installer: win-build-app ## Windows: build MindForger installer (windeployqt + Inno Setup)
	cd windows && ./build-installer.bat
	@echo "If build succeeded, then MindForger installer can be found in:\n  ../app/release/installer/mindforger-setup.exe"
	@echo ""
	ls -al ../app/release/installer/mindforger-setup.exe

.PHONY: win-build-unit-tests
win-build-unit-tests: ## Windows: build MindForger lib unit tests
	cd windows && ./build-unit-tests.bat
	@echo "If build succeeded, then unit tests executable can be found in:\n  ../lib/test/src/debug/mindforger-lib-unit-tests.exe"
	@echo ""
	ls -al ../lib/test/src/debug/mindforger-lib-unit-tests.exe

.PHONY: win-run-app
win-run-app: ## Windows: run the already-built MindForger release application
	@echo "Running:\n  ../app/release/mindforger.exe"
	cd windows && ./run-app.bat

.PHONY: win-run-unit-tests
win-run-unit-tests: win-build-unit-tests ## Windows: build and run MindForger lib unit tests
	cd windows && ./run-unit-tests.bat


#
# install
#


# Qt WebEngine is the default HTML rendering backend (see build-webkit target above
# for the legacy Qt WebKit alternative, which needs libqt5webkit5-dev instead)

.PHONY: devenv-ubuntu
devenv-ubuntu:
	sudo apt-get install build-essential zlib1g-dev libhunspell-dev libcurl4-gnutls-dev qtwebengine5-dev qttools5-dev-tools qt5-default ccache cmake


devenv-ubuntu-2204:
	sudo apt-get install build-essential zlib1g-dev libhunspell-dev libcurl4-gnutls-dev qtwebengine5-dev qttools5-dev-tools qtbase5-dev qt5-qmake ccache cmake


.PHONY: devenv-debian
devenv-debian: ## dev env for build/debian/debian-make-deb.sh, which is pinned to CONFIG+=mfwebkit (see build/debian/debian/control)
	sudo apt-get install build-essential zlib1g-dev libcurl4-gnutls-dev libhunspell-dev qtwebengine5-dev libqt5webkit5-dev qttools5-dev-tools ccache cmake debhelper


install-dev-local: ../deps/cmark-gfm/build ../Makefile ../lib/tests/Makefile clean build-rc ## install MindForger RC build to ~/bin as 'mind' executable
	cp -vf ../app/mindforger ~/bin
	mv -vf ~/bin/mindforger ~/bin/mind
	~/bin/mind --version


#
# tools
#


.PHONY: l10n-update
l10n-update: ## update l10n strings of ALL languages from the source code (.ts)
	cd make && ./l10n-update-strings.sh


.PHONY: l10n-check
l10n-check: ## check that l10n strings of ALL languages are in sync (.ts)
	cd make && ./l10n-check-strings.py


.PHONY: l10n-release
l10n-release: ## compile l10n strings of ALL languages to binary catalogs (.qm)
	cd make && ./l10n-release-strings.sh


.PHONY: l10n
l10n: l10n-update l10n-check l10n-release ## update, check and compile l10n of ALL languages


.PHONY: l10n-edit
l10n-edit: ## edit l10n strings of ONE language in Qt Linguist: MF_LANG=en
	cd make && ./l10n-update-strings.sh && ./l10n-edit-and-release.sh $(MF_LANG)


.PHONY: localization
localization: l10n-edit ## DEPRECATED alias of l10n-edit - use l10n or l10n-edit instead


.PHONY: releng-rel-version
releng-rel-version: ## pre-version update finder
	cd .. && git grep -n "2\.1\."


.PHONY: releng-version-set
releng-version-set: ## propagate lib/src/app_info.h version to all release/packaging files
	@echo "Setting version $(MINDFORGER_VERSION) ($(MINDFORGER_VERSION_MAJOR_MINOR)) in release/packaging files..."
	sed -i "s/^version: .*/version: $(MINDFORGER_VERSION_MAJOR_MINOR).{build}/" ../appveyor.yml
	sed -i "s/^export NEW_VERSION_NAME=.*/export NEW_VERSION_NAME=\"$(MINDFORGER_VERSION)\"/" debian/debian-aptly-add-deb.sh
	sed -i "s/^export ARG_VERSION=.*/export ARG_VERSION=\"$(MINDFORGER_VERSION)\"/" debian/debian-make-deb.sh
	sed -i -E "s/^mindforger \([0-9]+\.[0-9]+\.[0-9]+(-[^)]*)\)/mindforger ($(MINDFORGER_VERSION)\1)/" debian/debian/changelog
	sed -i "s/export MFVERSION=\"[0-9.]*\"/export MFVERSION=\"$(MINDFORGER_VERSION)\"/" fedora/fedora-rpm-from-deb.sh
	sed -i "s/^export MINDFORGER_VERSION=.*/export MINDFORGER_VERSION=\"$(MINDFORGER_VERSION)\"/" macos/env.sh
	sed -i "s/^version: .*/version: '$(MINDFORGER_VERSION)'/" snap/snapcraft.yaml
	sed -i -E "s|<release version=\"[^\"]+\" date=\"[^\"]+\"|<release version=\"$(MINDFORGER_VERSION)\" date=\"$$(date +%Y-%m-%d)\"|" flatpak/com.mindforger.MindForger.metainfo.xml
	sed -i "s/^export ARG_VERSION=.*/export ARG_VERSION=\"$(MINDFORGER_VERSION)\"/" tarball/tarball-build.sh
	sed -i -E "s/^mindforger \([0-9]+\.[0-9]+\.[0-9]+(-[^)]*)\)/mindforger ($(MINDFORGER_VERSION)\1)/" ubuntu/debian/changelog
	@echo "Done. Review changes with: git diff"

.PHONY: releng-gh-downloads
releng-gh-downloads: ## show GitHub downloads count w/ optional VERSION=x.y.z release var
	./make/gh-release-downloads.sh dvorka/mindforger $(if $(VERSION),$(VERSION),$(MINDFORGER_PREVIOUS_MINOR_VERSION))

.PHONY: distro-pad-refresh
distro-pad-refresh: ## refresh PAD.xml release fields (version, date, changelog, installer size)
	./make/distro-pad-refresh.py


../lib/tests/Makefile:
	@echo "Generating lib test Makefile..."
	cd ../lib/test && qmake -r mindforger-lib-unit-tests.pro ; cd ../../build


test-lib: clean ## compile and run lib/ unit tests
	cd make && ./test-lib-units.sh


.PHONY: distro-work-clean
distro-work-clean:
	rm -rvf $(MF_MAKER_WORKING_DIR)


$(MF_MAKER_WORKING_DIR):
	mkdir -vp $(MF_MAKER_WORKING_DIR)


$(MINDFORGER_RELEASE_DIR):
	mkdir -v $(MINDFORGER_RELEASE_DIR) || echo "$(MINDFORGER_RELEASE_DIR) already exists"


distro-tarball: distro-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR) ## build tarball distribution
	@echo "Building TARBALL distribution..."
	mkdir -vp $(MF_MAKER_WORKING_DIR)
	cp -vf ./tarball/tarball-build.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./tarball-build.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION)_tarball.tgz $(MINDFORGER_RELEASE_DIR)


distro-deb: distro-work-clean $(MF_MAKER_WORKING_DIR) $(MINDFORGER_RELEASE_DIR) ## build Debian distribution
	@echo "Building DEB distribution..."
	mkdir -vp $(MF_MAKER_WORKING_DIR)
	cp -vf ./debian/debian-make-deb.sh $(MF_MAKER_WORKING_DIR) && cd $(MF_MAKER_WORKING_DIR) && ./debian-make-deb.sh
	cp -vf $(MF_MAKER_WORKING_DIR)/`cd $(MF_MAKER_WORKING_DIR) && ls -d mindforger*`/mindforger_$(MINDFORGER_VERSION)-1_amd64.deb $(MINDFORGER_RELEASE_DIR)


.PHONY: distro-rpm
distro-rpm: ## build .rpm package on Fedora
	@echo "IMPORTANT: this target MUST be run on Fedora!"
	cd fedora && ./fedora-distro-setup.sh
	@echo "Building .rpm package..."
	cp -vf ./fedora-rpm-from-deb.sh ~/alien && cd ~/alien && sudo ./fedora-rpm-from-deb.sh $(MINDFORGER_VERSION)
	@echo "Find .rpm in ~/alien directory"


.PHONY: distro-dmg
distro-dmg: ## build macOS Disk iMaGe .dmg package
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

.PHONY: distro-debian-ppa
distro-debian-aptly-create-ppa:  ## create Debian PPA for all releases
	cd ./debian && ./debian-ppa-aptly-build.sh

#
# ^ Debian
#


.PHONY: distro-ubuntu-deb
distro-ubuntu-deb: ## locally build .deb for any Ubuntu version: DISTRO=bionic
	@echo "Building Ubuntu $(DISTRO) .deb ..."
	@echo "Copying latest version of .sh script to launchpad/ ..."
	@echo "Running .sh script ..."
	@echo "Copying .deb from build area to the current directory ..."
	false


#
# DISTRIBUTION: Snap package (local builds only)
#
# Prerequisites:
#   sudo snap install snapcraft --classic
#   sudo snap install lxd
#   sudo lxd init --auto
#   sudo usermod -aG lxd $USER
#
# build/snap/snapcraft.yaml is the STRICT manifest (Snap Store). distro-snap-classic
# derives the classic manifest (downloadable/sideloaded release) from it via
# build/snap/apply-classic.sh - never hand-edit a classic variant directly.
#

.PHONY: distro-snap-clean
distro-snap-clean: ## remove locally built Snap relics (packed .snap + build-copy snapcraft.yaml) so a stale one is never picked up by mistake
	@echo "Removing locally built Snap artifacts..."
	rm -vf ../mindforger_*.snap
	rm -vf ../snapcraft.yaml


.PHONY: distro-snap
distro-snap: distro-snap-clean ## build strict Snap package for the Snap Store (LXD required)
	@echo "Building Snap $(MINDFORGER_VERSION) ..."
	cd .. && make clean && cd build
	cp -vf snap/snapcraft.yaml ../snapcraft.yaml
	sed -i "s/^version: .*/version: '$(MINDFORGER_VERSION)'/" ../snapcraft.yaml
	cd .. && snapcraft clean && snapcraft pack


.PHONY: distro-snap-wip
distro-snap-wip: ## debug Snap build w/o clean up, but w/ incremental build
	@echo "Building Snap $(MINDFORGER_VERSION) ..."
	cp -vf snap/snapcraft.yaml ../snapcraft.yaml
	sed -i "s/^version: .*/version: '$(MINDFORGER_VERSION)'/" ../snapcraft.yaml
	cd .. && snapcraft pack --debug


.PHONY: distro-snap-classic
distro-snap-classic: distro-snap-clean ## build classic Snap package for downloadable GitHub Release (LXD required)
	@echo "Building classic Snap $(MINDFORGER_VERSION) ..."
	cd .. && make clean && cd build
	cp -vf snap/snapcraft.yaml ../snapcraft.yaml
	sed -i "s/^version: .*/version: '$(MINDFORGER_VERSION)'/" ../snapcraft.yaml
	./snap/apply-classic.sh ../snapcraft.yaml
	cd .. && snapcraft clean && snapcraft pack


.PHONY: distro-snap-remove
distro-snap-remove: ## remove locally installed Snap (requires sudo)
	@echo "Removing Snap package..."
	sudo snap remove mindforger || true
	@echo "DONE Snap removed"


.PHONY: distro-snap-install
distro-snap-install: distro-snap-remove distro-snap ## build and install the STRICT Snap locally (for testing, requires sudo)
	@echo "Installing strict Snap package locally..."
	@SNAP_FILE=$$(ls ../mindforger_*.snap 2>/dev/null | head -1); \
	if [ -z "$$SNAP_FILE" ]; then \
		echo "Error: Snap package not found. Run 'make distro-snap' first."; \
		exit 1; \
	fi; \
	echo "Note: this command requires sudo privileges for snap install"; \
	sudo snap install --dangerous "$$SNAP_FILE"; \
	echo "DONE Snap installed. Run with: mindforger"


.PHONY: distro-snap-install-classic
distro-snap-install-classic: distro-snap-remove distro-snap-classic ## build and install the CLASSIC Snap locally (for testing, requires sudo)
	@echo "Installing classic Snap package locally..."
	@SNAP_FILE=$$(ls ../mindforger_*.snap 2>/dev/null | head -1); \
	if [ -z "$$SNAP_FILE" ]; then \
		echo "Error: Snap package not found. Run 'make distro-snap-classic' first."; \
		exit 1; \
	fi; \
	echo "Note: this command requires sudo privileges for snap install"; \
	sudo snap install --dangerous --classic "$$SNAP_FILE"; \
	echo "DONE Snap installed. Run with: mindforger"


.PHONY: distro-snap-path
distro-snap-path: ## show path to the last built Snap package
	@ls -1 ../mindforger_*.snap 2>/dev/null || echo "No Snap package built yet"


.PHONY: distro-snap-upload
distro-snap-upload: distro-snap ## upload strict Snap package to the Snap Store (amd64)
	@echo "Uploading Snap package to the Snap Store..."
	snapcraft upload --release=stable ../mindforger_$(MINDFORGER_VERSION)_amd64.snap


#
# DISTRIBUTION: Flatpak package (local builds only)
#
# The bundle built here is the one uploaded to a GitHub Release - it is NOT the
# Flathub-hosted build.
#
# Prerequisites:
#   sudo apt install flatpak flatpak-builder   # or dnf/pacman/zypper equivalent
#   flatpak remote-add --if-not-exists --user flathub \
#       https://flathub.org/repo/flathub.flatpakrepo
#   flatpak install --user flathub \
#       org.kde.Platform//5.15-24.08 org.kde.Sdk//5.15-24.08 \
#       io.qt.qtwebengine.BaseApp//5.15-24.08
#
# build/flatpak/com.mindforger.MindForger.yaml is the manifest - Qt 5.15 comes from
# the KDE runtime, Qt WebEngine from Flathub's shared Qt WebEngine base app.
#

.PHONY: distro-flatpak-clean
distro-flatpak-clean: ## remove locally built Flatpak relics (build dir, OSTree repo, bundle) so a stale one is never picked up by mistake
	./flatpak/clean.sh


.PHONY: distro-flatpak-build
distro-flatpak-build: ## build Flatpak bundle for downloadable GitHub Release (flatpak-builder required)
	@echo "Building Flatpak $(MINDFORGER_VERSION) ..."
	./flatpak/build-flatpak.sh


.PHONY: distro-flatpak-path
distro-flatpak-path: ## show path to the last built Flatpak bundle
	@ls -1 ../distro/flatpak/mindforger-*.flatpak 2>/dev/null || echo "No Flatpak bundle built yet"


.PHONY: distro-flatpak-remove
distro-flatpak-remove: ## remove locally installed Flatpak
	@echo "Removing Flatpak package..."
	flatpak uninstall --user -y com.mindforger.MindForger || true
	@echo "DONE Flatpak removed"


.PHONY: distro-flatpak-install
distro-flatpak-install: distro-flatpak-build ## build and install the Flatpak locally (user scope, for testing)
	@echo "Installing Flatpak bundle locally..."
	@BUNDLE=$$(ls -1 ../distro/flatpak/mindforger-*.flatpak 2>/dev/null | head -1); \
	if [ -z "$$BUNDLE" ]; then \
		echo "Error: Flatpak bundle not found. Run 'make distro-flatpak-build' first."; \
		exit 1; \
	fi; \
	flatpak install --user --reinstall -y "$$BUNDLE"; \
	echo "DONE Flatpak installed. Run with: flatpak run com.mindforger.MindForger"


.PHONY: distro-flatpak-run
distro-flatpak-run: ## run the locally installed Flatpak
	flatpak run com.mindforger.MindForger


#
# winget (Windows Package Manager)
#
# Prerequisites and full walkthrough: see build/winget/README.md
#
# Per-release sequence - run AFTER the GitHub release (with the Windows installer
# asset) is live:
#   make distro-winget-from-release VERSION=x.y.z
#   make distro-winget-validate     VERSION=x.y.z
#   make distro-winget-submit-pr    VERSION=x.y.z WINGET_PKGS_DIR=C:\path\to\winget-pkgs
#

.PHONY: distro-winget-from-release
distro-winget-from-release: ## look up the Windows installer on a GitHub release, hash it, and generate winget manifests (VERSION required)
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File build/winget/generate-from-release.ps1 -Version $(VERSION)

.PHONY: distro-winget-validate
distro-winget-validate: ## validate generated winget manifests locally (VERSION required)
	winget validate --manifest distro/winget/manifests/m/MindForger/MindForger/$(VERSION)

.PHONY: distro-winget-submit-pr
distro-winget-submit-pr: ## copy manifests to winget-pkgs fork, commit signed, push, open PR (VERSION and WINGET_PKGS_DIR required)
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File build/winget/submit-pr.ps1 -Version $(VERSION) -WingetPkgsDir "$(WINGET_PKGS_DIR)"

.PHONY: distro-winget-hash-url
distro-winget-hash-url: ## print SHA256 of the published Windows installer without generating manifests (VERSION required)
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File build/winget/sha256-from-url.ps1 -Version $(VERSION)

#
# ^ winget
#


.PHONY: distro-all-clean
distro-all-clean:
	rm -rvf $(MINDFORGER_RELEASE_DIR)


distro-all: distro-all-clean $(MINDFORGER_RELEASE_DIR) distro-tarball distro-deb ## build all distributions
	@echo "Building all $(MINDFORGER_VERSION) distributions"


.PHONY: doc-to-wiki
doc-to-wiki: ## mindforger-documentation to mindforger.wiki
	cd doc && ./mf-doc-to-wiki.py


www-live: ## start the local web server for the www.mindforger.com
	@echo "Starting local web server for www.mindforger.com..."
	# GitHub repository: dvorka/my-webs
	cd ../../../../webs/www.mindforger.com/www && python3 -m http.server 8000


.PHONY: doc-api-reference
api-reference: ## generate Doxygen source code documentation
	cd doxygen && doxygen ./mindforger.cfg


.PHONY: statistic
statistic: ## show source code statistic
	cd make && ./statistic.sh


# eof

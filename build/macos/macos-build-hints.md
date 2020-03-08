# MindForger macOS build: 2020/03/06

HW:

* MacBook Pro Late 2013, CPU i5, 8GB RAM,

---

Prerequisites:

1. xcode (install or check installation)
1. brew (install/reinstall and update)
1. brew install ccache
1. qt: download from qt.com & install:
   * Qt 5.14.1 (all components)
   * Qt Creator
   * Qt qmake
1. PATH (copy .bash_profile section + include .bash_profile to .bashrc for Qt Creator)
   * /Users/dvorka/Qt/5.14.1/clang_64/bin (before PATH)
   * /Users/dvorka/Qt/Tools/CMake/CMake.app/Contents/bin
   * /Users/dvorka/Qt/Tools/QtInstallerFramework/3.2/bin
1. git (clone, submodules)
1. build cmark (skip discount)
1. build discount (mandatory for now > to be removed)

MindForger build:

1. CLI: build mindforger
   * qmake
   * make
1. Qt Creator: build MindForger
   * start QtCreator from terminal to ensure $PATH
   * open mindforger.pro
   * configure project (clang, debug and release)
   * run qmake, clean and build
1. build distribution:
   * cd mindforger/build/macos
   * ./macos-build.sh

---

Troubleshooting

* PROBLEM: brew installation doesn't work
  * reinstall brew:
     * /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  * brew update
  * brew install ccache
  * (brew install hstr)

---

Plan:

* rewrite macOS howto in mindforger-repository:
   * remove brew based Qt > suggest Qt download and installation
   * bottom section to be incorporated to the normal text
   * suggest use of my scripts to get DMG > after build describe how to get DMG using my script

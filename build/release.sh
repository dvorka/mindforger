#!/bin/bash

# IMPROVE: make release version parameter to all build scripts and set it here

export SCRIPT_HOME=`pwd`

# Ubuntu
cp -vf ubuntu/ubuntu-launchpad-releases.sh /home/dvorka/p/mindforger/launchpad
cd /home/dvorka/p/mindforger/launchpad && ./ubuntu-launchpad-releases.sh

# tarball
cd ${SCRIPT_HOME}
cp -vf tarball/tarball-build.sh /home/dvorka/p/mindforger/release
cd /home/dvorka/p/mindforger/release && ./tarball-build.sh

# .deb
cd ${SCRIPT_HOME}
cp -vf debian/debian-make-deb.sh /home/dvorka/p/mindforger/debian
cd /home/dvorka/p/mindforger/debian && ./debian-make-deb.sh

# .rpm (SSH to running Fedora - copy there and back)

# eof

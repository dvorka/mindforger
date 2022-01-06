# Fedora RPM build

Builder:

* Fedora 35 VM @ VirtualBox @ macOS

Builder:

* Fedora 28 VM @ VirtualBox @ Beast

Build steps:

* .deb upload from any Ubuntu to web
* .deb download to VM
* script from this directory used to run
  alien and build .rpm from .deb
* optional compilation of MindForger on VM
  to get the right dependencies and dynamic/static
  libraries versions.

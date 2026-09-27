# MindForger @ Arch Linux

Upstream **reference** packaging of MindForger for [Arch Linux](https://archlinux.org).

MindForger is distributed to Arch users by the [AUR](https://aur.archlinux.org)
(Arch User Repository) package [mindforger](https://aur.archlinux.org/packages/mindforger),
which is maintained by the AUR maintainers, **not** by MindForger upstream. The files
in this directory are the upstream proposal of that package for every new MindForger
release - they are handed over to the AUR maintainers (see below).

```
PKGBUILD   ... AUR build recipe: MindForger built from GitHub release tag w/ Qt WebKit
.SRCINFO   ... AUR metadata generated from PKGBUILD (AUR requires it next to PKGBUILD)
```

## HTML backend: Qt WebKit

The PKGBUILD builds MindForger with **Qt WebKit** (`qmake CONFIG+=mfwebkit`) instead of
the upstream default Qt WebEngine:

* Neither `qt5-webkit` nor `qt5-webengine` are in the official Arch repositories anymore,
  both must be built from AUR on the user's machine.
* `qt5-webkit` builds in about an hour, `qt5-webengine` (Chromium) takes several hours
  and lots of RAM and disk.

Limitations of the Qt WebKit build:

* **source code syntax highlighting** is not available (the option is hidden in the
  preferences) as Qt WebKit's JavaScript engine cannot run the highlight.js bundle
* Qt WebKit is end-of-life and gets no security fixes (MindForger renders your own
  local Markdown, but the exposure is not zero)

Arch users who want the Qt WebEngine features w/o compiling anything can install the
[Flatpak](https://www.mindforger.com/docs/installation.html#flatpak) or
[Snap](https://www.mindforger.com/docs/installation.html#snap) package instead.

## What an Arch user builds

Installing `mindforger` from AUR builds this chain of AUR packages, in this order
(an AUR helper like `yay` or `paru` resolves it automatically):

```
qt5-location    \
qt5-sensors      |-- Qt 5 modules dropped from the official repositories
qt5-webchannel  /
qt5-doc         ... make dependency of qt5-webkit - downloads and configures the whole
                    Qt 5 source tree to generate the documentation
qt5-webkit      ... Qt WebKit
mindforger      ... MindForger itself (a few minutes)
```

Everything else (`qt5-base`, `qt5-tools`, `hunspell`, `curl`, `cmake`, ...) comes from
the official repositories.

## Verify the package

Build, check and install the package in an Arch Linux Docker container (no Arch
machine needed) - from `build/`:

```sh
make distro-arch-pkg
```

The target:

* builds the Qt WebKit chain from AUR on the **first** run (takes hours; the Docker
  layers are cached, so subsequent runs take minutes)
* builds MindForger using `PKGBUILD` - from the GitHub **release tag** `pkgver`,
  NOT from the local working copy, as it verifies what AUR users get
* runs `namcap` (fails on errors), installs the package using `pacman -U` and checks
  installed files and shared libraries
* copies `mindforger-<version>-<pkgrel>-x86_64.pkg.tar.zst` to `../mindforger-arch`

Docker files live in `build/docker/arch/`.

## New MindForger release

1. `make releng-version-set` (from `build/`) updates `pkgver`, resets `pkgrel` to `1`
   and updates `.SRCINFO` - both in this directory.
2. If `PKGBUILD` changed in any other way, then regenerate `.SRCINFO` using
   `make distro-arch-srcinfo`.
3. Once the GitHub release is tagged, run `make distro-arch-pkg`.
4. Hand `PKGBUILD` over to the AUR maintainers - post a comment w/ the diff at
   [aur.archlinux.org/packages/mindforger](https://aur.archlinux.org/packages/mindforger)
   (or push it, if upstream becomes co-maintainer).

Notes:

* `sha256sums` are `SKIP`, because the release tag may still move before the release
  is announced - AUR maintainers pin the checksum of the final tag (`updpkgsums`).
* The CMake policy flag in `build()` is needed only for releases whose `cmark-gfm`
  submodule predates the CMake 4 fix (`cmake_minimum_required` < 3.5).

## Test the AUR package by hand

On an Arch machine, w/ an AUR helper:

```sh
yay -S mindforger
```

... or manually w/ `makepkg`, one AUR package after another in the order given above:

```sh
git clone https://aur.archlinux.org/qt5-location.git
cd qt5-location
makepkg -si
cd ..
# ... repeat for qt5-sensors, qt5-webchannel, qt5-doc and qt5-webkit
```

... and finally MindForger from this directory:

```sh
cd build/arch
makepkg -si
```

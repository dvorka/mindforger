# MindForger Debian Release Process

This document describes how MindForger `.deb` packages are built for Debian
and how they are released to MindForger **Debian PPAs** (Personal Package
Archives) at:

* https://www.mindforger.com/debian-ppa/

TL;DR - releasing a new MindForger version for all supported Debian releases:

```bash
cd build
make distro-debian-add-new-version
# ... then upload distro/debian/ (see Upload)
```

## Contents

* [Overview](#overview)
* [Debian releases](#debian-releases)
* [Directories](#directories)
* [Configuration](#configuration)
* [Prerequisites](#prerequisites)
* [Make targets](#make-targets)
* [New MindForger version release](#new-mindforger-version-release)
* [Add, update or remove a MindForger version](#add-update-or-remove-a-mindforger-version)
* [Add or freeze a Debian release](#add-or-freeze-a-debian-release)
* [Docker builds](#docker-builds)
* [PPA management script](#ppa-management-script)
* [Signing key](#signing-key)
* [Upload](#upload)
* [Installation (for users)](#installation-for-users)
* [Troubleshooting](#troubleshooting)

## Overview

```
 MindForger source code (this Git repository, version in lib/src/app_info.h)
   |
   |  make distro-debian-deb                 ... Docker: one container per Debian release
   v
 ../mindforger-deb/<codename>/mindforger_<version>-1_amd64.deb
   |
   |  make distro-debian-ppa-add             ... copy .deb to the archive
   v
 ~/p/mindforger/debian/aptly/mindforger/<NN>-<codename>/*.deb   <-- SOURCE OF TRUTH
   |
   |  aptly (throwaway aptly root)           ... PPA is rebuilt from the archive
   |  gpg                                    ... PPA is signed w/ MindForger key
   v
 distro/debian/<codename>/{dists,pool,index.html}           ... Git ignored
   |
   |  make distro-debian-ppa-verify          ... apt install in Docker debian:<codename>
   |  manual upload
   v
 https://www.mindforger.com/debian-ppa/<codename>/
   |
   |  apt update && apt install mindforger
   v
 users
```

Key design decisions:

* **One PPA per Debian release** - `.deb` packages for different Debian
  releases have the same file name (e.g. `mindforger_2.5.0-1_amd64.deb`)
  but different content (dependencies), therefore they cannot share one
  aptly pool. Each release is published by aptly with its own prefix, e.g.
  `trixie/dists/trixie` and `trixie/pool`, which is exactly the layout of
  `https://www.mindforger.com/debian-ppa/`.
* **The `.deb` archive is the source of truth** - add/update/remove change the
  archive and then the PPA of the Debian release is **rebuilt from scratch**
  by aptly in a throwaway aptly root. There is no persistent `~/.aptly` state
  which could be lost or get out of sync - PPAs can be regenerated at any
  time using `make distro-debian-ppa-sync`.
* `.deb` packages are built in **Docker** - no Debian machine or VM is needed.

## Debian releases

Debian releases are listed in [debian-releases.conf](debian-releases.conf) -
the one and only list used by all scripts and make targets:

| Number | Codename | Status    | HTML backend | Notes                           |
|--------|----------|-----------|--------------|---------------------------------|
| 13     | trixie   | supported | webengine    | Qt WebKit is gone from Debian   |
| 12     | bookworm | supported | webkit       |                                 |
| 11     | bullseye | frozen    | webkit       | LTS ended Aug 2026              |
| 10     | buster   | frozen    | -            |                                 |
| 9      | stretch  | frozen    | -            |                                 |

* `supported` - new MindForger versions are built and added to the PPA.
* `frozen` - the PPA is kept published (and re-signed when synced), but no new
  MindForger versions are added.

## Directories

| Path | Description |
|------|-------------|
| `build/debian/debian-releases.conf` | Debian releases list (status, HTML backend) |
| `build/debian/debian-releases.sh`   | library which reads `debian-releases.conf` |
| `build/debian/debian-config.sh`     | **configuration**: paths + signing key (see [Configuration](#configuration)) |
| `build/debian/debian-ppa.sh`        | PPA management script (aptly, gpg, Docker verification) |
| `build/debian/index-ppa.html`       | template of `index.html` of a PPA (installation steps) |
| `build/debian/index-all-ppas.html`  | template of `/debian-ppa/index.html`: table of Debian releases (status, latest MF version), installation steps and signing key notes |
| `build/docker/debian-deb/`          | Docker-based `.deb` builder (one image per Debian release) |
| `build/debian/debian/`              | Debian packaging (control, rules, ...) |
| `../mindforger-deb/<codename>/`     | `.deb` packages built by Docker (`MF_DEBIAN_DEB_DIR`) |
| `~/p/mindforger/debian/aptly/mindforger/<NN>-<codename>/` | `.deb` archive - source of truth (`MF_DEBIAN_ARCHIVE_DIR`) |
| `distro/debian/`                   | generated PPAs to be uploaded - Git ignored, rebuilt from the archive anytime (`MF_DEBIAN_PPA_DIR`) |

Paths in the last three rows are defaults - see [Configuration](#configuration).

**Back up** the `.deb` archive - it is NOT in Git and it is the only thing
which is needed to regenerate all PPAs.

## Configuration

[debian-config.sh](debian-config.sh) is the **one and only place** where
paths and the signing key are configured. It is sourced by both
`debian-ppa.sh` and `build/docker/debian-deb/build-deb.sh`, make targets
do not define any paths.

| Variable | Default | Description |
|----------|---------|-------------|
| `MF_DEBIAN_ARCHIVE_DIR` | `~/p/mindforger/debian/aptly/mindforger` | MASTER `.deb` archive (source of truth, not in Git) |
| `MF_DEBIAN_PPA_DIR` | `<repo>/distro/debian` | generated PPAs to be uploaded (Git ignored) |
| `MF_DEBIAN_DEB_DIR` | `<repo>/../mindforger-deb` | `.deb` packages built by Docker |
| `MF_DEBIAN_GPG_KEY` | `E3E8520DE3820D8C1A724BB7B72E4F7F24AF591D` | fingerprint of the PPA signing key |
| `MF_DEBIAN_GPG_KEY_URL` | `https://www.mindforger.com/gpgpubkey.txt` | public key as published on the web |

Every value can be overridden by the environment variable of the same name.

Transparency - every command which writes anything:

* prints the configuration in effect incl. the source of each value
  (`default` or `env`)
* prints `WRITE: ...` line **before** it modifies the master archive
  (`MASTER archive add|update|remove`) or replaces a PPA directory
* `make distro-debian-ppa-list` prints the configuration and per-release
  archive and PPA directories as well

Safety - the master archive directory is **never created** by the scripts
(only its `<NN>-<codename>/` subdirectories are). If it does not exist, all
commands which write fail - a PPA rebuilt from an empty or wrong archive
would drop all previously released versions from the web on the next upload.

### Moving to a new machine

1. Restore the master archive (backup) to its new location, e.g.
   `/data/mindforger/debian/aptly/mindforger/`
2. Either keep the default path (`~/p/mindforger/debian/aptly/mindforger/`),
   or set the new one in your shell profile (`~/.bashrc`, `~/.zshrc`):

   ```bash
   export MF_DEBIAN_ARCHIVE_DIR=/data/mindforger/debian/aptly/mindforger
   ```

   (alternatively change the default in `debian-config.sh` and commit it)
3. Import the signing key: `gpg --import <secret key backup>`
4. `make devenv-debian-ppa` and check the result:

   ```bash
   make distro-debian-ppa-key-check
   make distro-debian-ppa-list        # archive path (env) + versions
   make distro-debian-ppa-sync        # regenerate PPAs to distro/debian/
   ```

## Prerequisites

* Docker (the user must be able to run `docker` w/o `sudo`).
* aptly, gpg, curl and dpkg-dev:

  ```bash
  make devenv-debian-ppa
  ```

* Secret MindForger signing key in `~/.gnupg` (see [Signing key](#signing-key)):

  ```bash
  make distro-debian-ppa-key-check
  ```

## Make targets

All targets are in `build/Makefile` - run them from the `build/` directory.
Parameters:

* `DEBIAN_RELEASE` - Debian release codename e.g. `trixie`
* `MF_VERSION` - MindForger version e.g. `2.5.0` (default: current version
  from `lib/src/app_info.h`)

| Target | Description |
|--------|-------------|
| `distro-debian-add-new-version` | **the release**: key check, build `.deb`s for all supported releases, add them to PPAs, verify |
| `distro-debian-deb` | build `.deb` in Docker for `DEBIAN_RELEASE` (default: all supported) |
| `distro-debian-ppa-add` | add `MF_VERSION` `.deb` to `DEBIAN_RELEASE` PPA |
| `distro-debian-ppa-update` | replace `MF_VERSION` `.deb` in `DEBIAN_RELEASE` PPA |
| `distro-debian-ppa-remove` | remove `MF_VERSION` from `DEBIAN_RELEASE` PPA |
| `distro-debian-ppa-sync` | rebuild and re-sign `DEBIAN_RELEASE` PPA from the archive (default: all) |
| `distro-debian-ppa-list` | list archived and published MF versions (default: all releases) |
| `distro-debian-ppa-index` | regenerate `index.html` pages only (e.g. after template change) - no re-signing |
| `distro-debian-ppa-verify` | install MF from the local PPA in `debian:<codename>` container (default: all supported) |
| `distro-debian-ppa-key-check` | check validity of the signing key |
| `devenv-debian-ppa` | install aptly & co. |

Examples:

```bash
make distro-debian-deb DEBIAN_RELEASE=trixie
make distro-debian-ppa-add DEBIAN_RELEASE=trixie MF_VERSION=2.5.0
make distro-debian-ppa-update DEBIAN_RELEASE=trixie MF_VERSION=2.5.0
make distro-debian-ppa-remove DEBIAN_RELEASE=bookworm MF_VERSION=1.54.0
make distro-debian-ppa-list
make distro-debian-ppa-sync
make distro-debian-ppa-verify DEBIAN_RELEASE=trixie
```

Legacy host build of `.deb` (`make distro-deb`, `debian-make-deb.sh`) is
still available - it is used for Debian mentors upload
(`debian-mentors-upload.sh`), not for PPAs.

## New MindForger version release

1. Set the version (see `make releng-version-set`) - `lib/src/app_info.h`
   is the authoritative version source.
2. Release:

   ```bash
   cd build
   make distro-debian-add-new-version
   ```

   which:
   * checks the signing key (`distro-debian-ppa-key-check`)
   * builds `.deb` for all **supported** Debian releases in Docker
   * adds the `.deb`s to the archive and rebuilds + signs the PPAs
     (if the version is already there, it is updated instead)
   * regenerates `index.html` pages
   * verifies that MindForger can be installed from every supported PPA
     in a clean `debian:<codename>` container using the release's own apt
3. [Upload](#upload) the PPAs.

## Add, update or remove a MindForger version

```bash
make distro-debian-deb DEBIAN_RELEASE=trixie        # build .deb first (add/update)
make distro-debian-ppa-add DEBIAN_RELEASE=trixie MF_VERSION=2.5.0
make distro-debian-ppa-update DEBIAN_RELEASE=trixie MF_VERSION=2.5.0
make distro-debian-ppa-remove DEBIAN_RELEASE=trixie MF_VERSION=2.4.0
```

* `add` fails if the version is already in the PPA, `update` fails if it is not.
* New versions can be added only to **supported** releases.
* The last package of a PPA cannot be removed.
* Each of these commands rebuilds and re-signs the PPA and its `index.html`.
* **Update caveat**: apt does not re-download a package whose version it
  already has installed - users who installed the previous build of the
  same version will not get the updated one. Prefer releasing a new
  (patch) version.

## Add or freeze a Debian release

Add a new Debian release (e.g. `forky`):

1. Add a line to [debian-releases.conf](debian-releases.conf) with
   `supported` status and the HTML backend (`webengine` for trixie+).
2. `make distro-debian-deb DEBIAN_RELEASE=forky` - fix the Docker build if
   needed (see [Docker builds](#docker-builds)).
3. `make distro-debian-ppa-add DEBIAN_RELEASE=forky MF_VERSION=x.y.z`
   (archive dir `<NN>-forky/` is created automatically).
4. `make distro-debian-ppa-verify DEBIAN_RELEASE=forky`
5. [Upload](#upload) - the new PPA is listed in the top-level `index.html`.

Freeze a Debian release: change its status to `frozen`. Its PPA stays
published, but it is not built and no new versions can be added.

## Docker builds

`build/docker/debian-deb/`:

* `build-deb.sh` - builds Docker image `mindforger-deb-builder:<codename>`
  from `debian:<codename>` for every requested release and runs it w/ the
  repository mounted read-only. Releases and their HTML backend are read
  from `debian-releases.conf`.
* `Dockerfile` - installs build dependencies; `MF_HTML_BACKEND` build
  argument selects `libqt5webkit5-dev` (webkit) or `qtwebengine5-dev`
  (webengine). bullseye uses `snapshot.debian.org` as its archive has moved.
* `container-build.sh` - runs in the container: copies sources, builds
  `cmark-gfm`, copies `build/debian/debian`, patches **its own copy**
  of `control`/`rules` to Qt WebEngine for webengine releases, and
  builds the binary package using `dpkg-buildpackage -us -uc -b`.
  `.deb` packages are unsigned - the PPA (repository metadata) is signed.

Output: `../mindforger-deb/<codename>/mindforger_<version>-1_amd64.deb`

```bash
./docker/debian-deb/build-deb.sh trixie                # one release
./docker/debian-deb/build-deb.sh                       # all supported
./docker/debian-deb/build-deb.sh bookworm /tmp/debs    # custom output dir
```

## PPA management script

`build/debian/debian-ppa.sh` is used by the make targets:

```
./debian-ppa.sh add       <codename> <version>
./debian-ppa.sh update    <codename> <version>
./debian-ppa.sh remove    <codename> <version>
./debian-ppa.sh release   <version>              # add/update in ALL supported
./debian-ppa.sh sync      <codename|all>         # rebuild PPA(s) from the archive
./debian-ppa.sh list      [codename]
./debian-ppa.sh index                            # regenerate index.html pages
./debian-ppa.sh verify    <codename|supported>
./debian-ppa.sh key-check
```

PPA rebuild (`sync`) steps for a release:

```bash
aptly repo create -distribution=<codename> -component=main mindforger-<codename>
aptly repo add mindforger-<codename> <archive>/<NN>-<codename>/
aptly publish repo -architectures=amd64 -origin=MindForger -label=MindForger \
    -gpg-key=<fingerprint> mindforger-<codename> <codename>
# ^ publishes to <aptly root>/public/<codename>/{dists,pool}
#   which replaces <PPA dir>/<codename>/
```

`verify` runs `apt-get update` and `apt-get install mindforger` in a clean
`debian:<codename>` container with the local PPA mounted as a `file:`
repository and the signing key as `signed-by` keyring. It fails on any apt
warning/error (e.g. rejected signature) and checks that the newest
version was installed.

## Signing key

PPAs are signed by MindForger GPG key:

```
pub   rsa2048/B72E4F7F24AF591D 2014-01-04 [SC]
      E3E8 520D E382 0D8C 1A72  4BB7 B72E 4F7F 24AF 591D
uid   Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>
sub   rsa2048/4F633C0D1283AE43 2014-01-04 [E]
```

* the public key is published at https://www.mindforger.com/gpgpubkey.txt
* the key does not expire
* `make distro-debian-ppa-key-check` checks: presence of the secret key,
  revocation/expiration, SHA-1 self-signatures, fingerprint of the published
  key, and signs a test file which is verified by Debian trixie `sqv`
  (the verifier used by apt since trixie)

### SHA-1 self-signatures (one-time fix)

Debian trixie apt uses Sequoia (`sqv`) which **rejects keys whose user ID
and subkey binding signatures use SHA-1** (the policy grace period ended
2026-02-01). This key was created in 2014 w/ SHA-1 self-signatures, so the
PPAs signed by it cannot be used on trixie:

```
Signing key on E3E8520DE3820D8C1A724BB7B72E4F7F24AF591D is not bound:
  because: SHA1 is not considered secure since 2023-02-01T00:00:00Z
```

Fix - refresh the self-signatures w/ SHA-512 (the fingerprint stays the same):

```bash
# re-sign primary key's user ID binding and subkey binding w/ SHA-512
gpg --cert-digest-algo SHA512 --quick-set-expire E3E8520DE3820D8C1A724BB7B72E4F7F24AF591D 0
gpg --cert-digest-algo SHA512 --quick-set-expire E3E8520DE3820D8C1A724BB7B72E4F7F24AF591D 0 \
    054E37048CA51950451A2E4D4F633C0D1283AE43
# optional: trust the key locally (removes [unknown] from gpg listings)
gpg --edit-key E3E8520DE3820D8C1A724BB7B72E4F7F24AF591D trust
# export the public key w/ the new self-signatures only
gpg --armor --export --export-options export-minimal \
    E3E8520DE3820D8C1A724BB7B72E4F7F24AF591D > ~/p/mindforger/debian/gpgpubkey.txt
# check
make distro-debian-ppa-key-check
```

Then:

1. Re-sign **all** PPAs - existing signatures predate the new self-signatures
   and would still be rejected by trixie: `make distro-debian-ppa-sync`
2. Upload `gpgpubkey.txt` to https://www.mindforger.com/gpgpubkey.txt
   along with the PPAs. Users of older Debian releases do not need to
   do anything; trixie users must use the refreshed key.

### RSA 2048 deadline

Debian apt accepts RSA 2048 keys **until 2030-02-01** (see
`/usr/share/apt/default-sequoia.config` on trixie). Before that date a new
key (e.g. ed25519 or rsa4096) must be created, published, PPAs re-signed
(`MF_DEBIAN_GPG_KEY` + `make distro-debian-ppa-sync`) and users asked to
import the new key. `key-check` warns about it.

## Upload

Upload is manual. After a release (or `sync`) upload the content of:

```
distro/debian/
  index.html          ... list of all PPAs
  bookworm/
    index.html        ... installation steps
    dists/
    pool/
  trixie/
  ...
```

to https://www.mindforger.com/debian-ppa/ - replace the release
directories completely (stale `dists/` files must not survive).
If the signing key was refreshed, upload also `gpgpubkey.txt` to
https://www.mindforger.com/gpgpubkey.txt

Check after the upload:

```bash
curl -sI https://www.mindforger.com/debian-ppa/trixie/dists/trixie/InRelease
```

## Installation (for users)

MindForger installation from the PPA - replace `trixie` with your Debian
release codename (`lsb_release -cs`):

```bash
# create directory for APT repository keys (if it does not exist)
sudo install -d -m 0755 /etc/apt/keyrings
# download MindForger GPG key and store it in the binary format used by APT
wget -qO- https://www.mindforger.com/gpgpubkey.txt \
    | gpg --dearmor | sudo tee /etc/apt/keyrings/mindforger.gpg > /dev/null
echo "deb [signed-by=/etc/apt/keyrings/mindforger.gpg] https://www.mindforger.com/debian-ppa/trixie trixie main" \
    | sudo tee /etc/apt/sources.list.d/mindforger.list
sudo apt update
sudo apt install mindforger
```

`apt-key` (used by older instructions) is deprecated and it is not
available on Debian trixie.

## Troubleshooting

* **`key-check` reports SHA-1 self-signatures** - see
  [SHA-1 self-signatures](#sha-1-self-signatures-one-time-fix).
* **`verify` fails w/ `The following signatures couldn't be verified`**
  or `Signing key ... is not bound` - the key was refreshed after the PPA was
  signed: `make distro-debian-ppa-sync DEBIAN_RELEASE=<codename>`.
* **aptly asks for the passphrase / signing fails** - gpg-agent needs a
  pinentry; run make from a terminal (`GPG_TTY` is set by the script).
* **`sudo apt-get install aptly` fails w/ 401 Unauthorized (esm.ubuntu.com)**
  - Ubuntu Pro ESM repository is not authorized; install the non-ESM
  version: `sudo apt-get install aptly=1.5.0+ds1-2ubuntu0.24.04.3`
* **bullseye Docker build fails to download packages** - bullseye moved to
  `snapshot.debian.org`, the Dockerfile handles it; rebuild the image.
* **A version is in the archive, but not in the PPA** - run
  `make distro-debian-ppa-sync DEBIAN_RELEASE=<codename>`, and check it with
  `make distro-debian-ppa-list`.

# MindForger Fedora COPR - cheatsheet

MindForger is built for all **supported** Fedora releases (`fedora-releases.conf`)
in Fedora COPR, which hosts and signs the `dnf` repository:

* https://copr.fedorainfracloud.org/coprs/dvorka/mindforger/

Users install it by:

```sh
sudo dnf copr enable dvorka/mindforger
sudo dnf install mindforger
```

Host prerequisites: **Linux** w/ GNU tools (`tar`, `date`) and **Docker** -
`rpmbuild` and `copr-cli` run in the `build/docker/fedora-rpm/` toolbox
container (like the Debian tooling, macOS is not supported as a release host).
Git submodules must be initialized (`git submodule update --init --recursive`).
Paths, COPR project and architectures are configured in `fedora-config.sh`
(env overrides).

## 1. First time setup (once)

1. Create a Fedora Account System (FAS) account: https://accounts.fedoraproject.org
2. Log in to COPR with it: https://copr.fedorainfracloud.org (the 1st login creates the COPR user)
3. Get API token at https://copr.fedorainfracloud.org/api/ and save the shown
   `[copr-cli]` section to `~/.config/copr` (`chmod 600 ~/.config/copr`)
4. Check the token:
   ```sh
   cd build && make distro-fedora-copr-token-check
   ```
5. Create the COPR project w/ chroots of **supported** releases from
   `fedora-releases.conf` (x86_64 + aarch64):
   ```sh
   # CWD is ./build
   ./fedora/fedora-copr.sh copr-cli create mindforger \
       --chroot fedora-44-x86_64 --chroot fedora-44-aarch64 \
       --chroot fedora-43-x86_64 --chroot fedora-43-aarch64 \
       --description "Thinking notebook and Markdown IDE" \
       --instructions "sudo dnf copr enable dvorka/mindforger && sudo dnf install mindforger"
   ```

## 2. Release a new MindForger version

Prerequisite: the release is committed (clean working tree incl. submodules),
version is set in `lib/src/app_info.h`.

```sh
cd build
make distro-fedora-add-new-version
```

It runs: token check, local `.rpm` pre-flight build for all supported
releases (Docker), COPR build of all chroots (waits for it), download of the
COPR `.rpm`s and installation check from COPR in clean `fedora:NN` containers.

Then attach `MF_FEDORA_RPM_DIR/copr/*.rpm` (default `distro/fedora/copr/`)
to the GitHub release.

Individual steps (when something fails):

```sh
make distro-fedora-srpm                          # tarball + SRPM only
make distro-fedora-rpm FEDORA_RELEASE=44         # local pre-flight .rpm build
make distro-fedora-copr-release                  # COPR build + .rpm download
make distro-fedora-copr-verify FEDORA_RELEASE=44 # install from COPR in Docker
```

Re-spin packaging of the same MindForger version (spec fix):
`MF_RPM_RELEASE=2 ./fedora/fedora-copr.sh release`

## 3. New / frozen Fedora release

* **New** Fedora release (e.g. 45):
  1. check Qt 5 WebEngine is still available:
     `docker run --rm fedora:45 dnf -q repoquery qt5-qtwebengine-devel`
  2. add `45 supported` to `fedora-releases.conf`
  3. enable its chroots in COPR (CWD is `./build`):
     `./fedora/fedora-copr.sh copr-cli modify mindforger --chroot fedora-45-x86_64 --chroot fedora-45-aarch64 ...`
     (`--chroot` list **replaces** the project chroots - list all of them) or use the COPR web UI
  4. `make distro-fedora-add-new-version`
* **Freeze** a release: change its status to `frozen` (no new builds)
* **EOL** release: delete its line (COPR removes EOL chroots itself)

## 4. COPR API token renewal

Tokens expire (`token-check` warns 30 days ahead): get a new one at
https://copr.fedorainfracloud.org/api/, overwrite `~/.config/copr` and run
`make distro-fedora-copr-token-check`.

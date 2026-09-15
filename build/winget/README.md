# MindForger winget distribution

winget (Windows Package Manager) lets Windows users install MindForger with:

```
winget install MindForger.MindForger
```

Packages are distributed through the community repository
[microsoft/winget-pkgs](https://github.com/microsoft/winget-pkgs) - there is no
"upload" step like the Snap Store; instead a pull request with YAML manifests is
opened against that repository, and a bot (`wingetbot`) validates the installer
before a Microsoft/community moderator merges it.

This directory contains the tooling used to generate and submit those manifests:

| File | Purpose |
|---|---|
| `generate-from-release.ps1` | Looks up the Windows installer on a GitHub release, downloads it, hashes it, and writes the three manifest YAML files |
| `sha256-from-url.ps1` | Prints just the SHA256 of a published installer (no manifest generation) |
| `submit-pr.ps1` | Copies generated manifests into a local `winget-pkgs` fork, commits, pushes, and opens the PR |

All three scripts are driven through `make` targets defined in `build/Makefile`
(see the `winget` section there) and **must run on Windows** - `winget validate`
and `winget install` are Windows-only, and the scripts are PowerShell.

## Package identity

| Field | Value |
|---|---|
| **Package identifier** | `MindForger.MindForger` |
| **Publisher** | Martin Dvorak |
| **Publisher URL** | https://www.mindforger.com |
| **License** | GPL-2.0-only |
| **AppId (Inno Setup)** | `{A1A3DAE4-FD5C-4600-B75D-D8895AA99693}` - from `build/windows/installer/mindforger-setup.iss`; must stay unchanged across releases |

Generated manifests are written to
`distro/winget/manifests/m/MindForger/MindForger/<version>/` (gitignored - this is
scratch output, not something this repository tracks; the real target of a
release is the PR opened against `winget-pkgs`).

## One-time setup

1. **Fork `microsoft/winget-pkgs`** on GitHub and clone the fork locally (on the
   Windows machine you will run these scripts from):
   ```
   # fork at: https://github.com/microsoft/winget-pkgs
   git clone git@github.com:<your-github-user>/winget-pkgs.git C:\path\to\winget-pkgs
   ```

2. **Point `WINGET_PKGS_DIR` at the fork**, either in the UI (This
   Computer/Properties/Advanced System Settings/Environment Variables/User) or
   via PowerShell:
   ```powershell
   [System.Environment]::SetEnvironmentVariable('WINGET_PKGS_DIR', 'C:\path\to\winget-pkgs', 'User')
   ```
   Verify in a new terminal:
   ```
   echo %WINGET_PKGS_DIR%
   ```

3. **Install prerequisites** (one-time):
   ```
   winget install Microsoft.Winget.Client    # winget CLI (for local validation)
   winget install GitHub.cli                 # gh CLI (for PR submission)
   gh auth login                             # authenticate gh
   ```

## Registering MindForger for the first time

The first submission is not special from a tooling point of view - it is just
the first PR against `manifests/m/MindForger/MindForger/<version>/` in
`winget-pkgs`, created the same way every subsequent release is. Follow the
"New release" steps below using the first version you want to publish (does not
have to be the current latest - any released version with a published Windows
installer works).

The **one thing that differs the first time**: after opening the PR, a bot
(`microsoft-github-policy-service`) will ask you to agree to the repository's
contribution policy. Reply on the PR with exactly:

```
@microsoft-github-policy-service agree
```

After that first PR is merged, `winget install MindForger.MindForger` works, and
every later release is just "New release" below with a bumped version - winget
treats it as an update to the existing package, not a new registration.

## New release: step by step

Run this **after** the GitHub release for the new version is live and has a
`windows-installer-mindforger-*.exe` asset attached (see
[Where the installer asset comes from](#where-the-installer-asset-comes-from)
below) - the winget manifest's SHA256 must match the exact bytes users will
download, so it is always derived from the published release, never from a
local build.

```
make distro-winget-from-release VERSION=2.3.0
make distro-winget-validate VERSION=2.3.0
make distro-winget-submit-pr VERSION=2.3.0 WINGET_PKGS_DIR=C:\path\to\winget-pkgs
```

Then check your PR and the automated validation:

* https://github.com/microsoft/winget-pkgs/pulls

If `wingetbot` reports a manifest problem, fix the generator or the manifest
fields, re-run `distro-winget-from-release`, and push an update to the same PR
branch (or close it and resubmit with `distro-winget-submit-pr`, which always
recreates the branch from a fresh sync with upstream `master`).

### Where the installer asset comes from

Unlike the Debian/Snap/macOS builds, there is no automated workflow that
publishes the Windows installer straight to a GitHub release: `.github/workflows/build-win.yml`
builds `mindforger-setup.exe` and attaches it as a **CI build artifact** on the
release pull request (source branch `rel/<major.minor.patch>`, target `master`),
but a human downloads that artifact and uploads it to the GitHub release by
hand, renamed to `windows-installer-mindforger-<version>[-<commit>][-<suffix>].exe`
(the exact suffix has varied across releases, e.g.
`windows-installer-mindforger-2.1.0-ff388a4.exe`).

Because the filename is not predictable from the version alone,
`generate-from-release.ps1` queries the GitHub Releases API for the release
tagged `<version>` and picks the first asset matching
`windows-installer-mindforger-*.exe`, rather than constructing the download URL
directly. Make sure that asset is uploaded to the release **before** running
`make distro-winget-from-release`.

### Overall flow

```
git tag {version} && git push origin {version}
          |
          v
release PR (rel/{version} -> master) triggers build-win.yml
  builds installer, uploads it as a CI artifact (not a release asset)
          |
          v
maintainer downloads the CI artifact and uploads it to the GitHub release,
renamed to windows-installer-mindforger-{version}[-...].exe
          |
          v
make distro-winget-from-release VERSION={version}
  looks up the asset via the GitHub API - downloads it - computes SHA256 - writes YAML manifests
          |
          v
make distro-winget-validate VERSION={version}
          |
          v
make distro-winget-submit-pr VERSION={version} WINGET_PKGS_DIR=C:\path\to\winget-pkgs
  copies manifests to fork - signed commit - pushes - opens PR
          |
          v
wingetbot validates PR -> maintainer merges -> winget install MindForger.MindForger works
```

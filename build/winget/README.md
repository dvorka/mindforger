# MindForger winget Package

MindForger is published to the [Windows Package Manager](https://learn.microsoft.com/windows/package-manager/)
as [`MartinDvorak.MindForger`](https://github.com/microsoft/winget-pkgs/tree/master/manifests/m/MartinDvorak/MindForger):

```
winget install MartinDvorak.MindForger
```

## Who Submits New Versions

New versions are typically submitted to [microsoft/winget-pkgs](https://github.com/microsoft/winget-pkgs)
by a community contributor (using [komac](https://github.com/russellbanks/Komac)) shortly
after a GitHub release with the Windows installer is published. There is no package
ownership in winget-pkgs - **anyone** can submit a new version, so if a release does
not show up in winget within a few days, use the scripts below.

Before submitting, check that the version is not already there or in an open PR:

- https://github.com/microsoft/winget-pkgs/tree/master/manifests/m/MartinDvorak/MindForger
- https://github.com/microsoft/winget-pkgs/pulls?q=MartinDvorak.MindForger

## Prerequisites

- Windows with PowerShell 5.1+ and `winget`
- GitHub release `x.y.z` with exactly one `windows-installer-mindforger-*.exe` asset
- fork of [microsoft/winget-pkgs](https://github.com/microsoft/winget-pkgs) cloned locally
- [gh CLI](https://cli.github.com/) authenticated: `winget install GitHub.cli` and `gh auth login`

## Submitting a New Version

Run from the `build/` directory:

```
make distro-winget-from-release VERSION=x.y.z
make distro-winget-validate     VERSION=x.y.z
make distro-winget-submit-pr    VERSION=x.y.z WINGET_PKGS_DIR=C:\path\to\winget-pkgs
```

1. `distro-winget-from-release` finds the installer on the GitHub release, downloads it,
   computes its SHA256 and writes the manifests to
   `distro/winget/manifests/m/MartinDvorak/MindForger/x.y.z/`.
2. `distro-winget-validate` validates the manifests with `winget validate`.
   Optionally test the install in [Windows Sandbox](https://learn.microsoft.com/windows/security/application-security/application-isolation/windows-sandbox/windows-sandbox-overview):
   `winget install --manifest ..\distro\winget\manifests\m\MartinDvorak\MindForger\x.y.z`
3. `distro-winget-submit-pr` syncs the fork's `master` with upstream (**destructive** - it
   refuses to run on a dirty fork), commits the manifests to branch
   `MartinDvorak.MindForger-x.y.z`, pushes it and opens the PR. Then watch the wingetbot
   validation on the PR.

To just print the installer SHA256: `make distro-winget-hash-url VERSION=x.y.z`

## Notes

- `ProductCode` is the Inno Setup `AppId` from `build/windows/installer/mindforger-setup.iss` - keep
  `$AppId` in `generate-from-release.ps1` in sync with it.
- Manifests must be UTF-8 **without** BOM - the generator takes care of it.
- Keep the generated metadata (`Moniker`, `FileExtensions`, tags, ...) in line with the latest
  manifest in winget-pkgs so that a submission does not drop fields added by other contributors.

# MindForger Chocolatey Package

MindForger is published to the [Chocolatey Community Repository](https://community.chocolatey.org/)
as [`mindforger`](https://community.chocolatey.org/packages/mindforger):

```
choco install mindforger
```

## Who Submits New Versions

Unlike winget, Chocolatey packages are **owned** by maintainer accounts - only the
owners can push new versions and there is no community bot (like komac for winget)
which would do it for you. Therefore every MindForger release must be pushed by
the maintainer using the steps below.

Every pushed version goes through [moderation](https://docs.chocolatey.org/en-us/community-repository/moderation/):
automated validator and verifier checks for every version, plus a human review for
the **first** version of the package, which can take days to weeks. A version is not
installable via `choco install mindforger` until it is approved.

## Prerequisites

- Windows with PowerShell 5.1+
- Chocolatey: `winget install Chocolatey.Chocolatey`
- account at [community.chocolatey.org](https://community.chocolatey.org/account/Register)
  and its API key registered (one-time):
  `choco apikey --key <API key> --source https://push.chocolatey.org/`
- GitHub release `x.y.z` with exactly one `windows-installer-mindforger-*.exe` asset

## Submitting a New Version

Run from the `build/` directory:

```
make distro-choco-from-release VERSION=x.y.z
make distro-choco-pack         VERSION=x.y.z
make distro-choco-test         VERSION=x.y.z
make distro-choco-push         VERSION=x.y.z
```

1. `distro-choco-from-release` finds the installer on the GitHub release, downloads it,
   computes its SHA256 and writes the package sources to
   `distro/chocolatey/mindforger/x.y.z/`.
2. `distro-choco-pack` builds `mindforger.x.y.z.nupkg` in the same directory.
3. `distro-choco-test` installs the package from the local directory and uninstalls it.
   Run it in an **elevated** shell, ideally in
   [Windows Sandbox](https://learn.microsoft.com/windows/security/application-security/application-isolation/windows-sandbox/windows-sandbox-overview)
   or a VM, and check that MindForger starts in between.
4. `distro-choco-push` pushes the package to the community repository. Then watch the
   moderation status on the package page and in the e-mails from the validator.

## Notes

- The package does not embed the installer, it downloads it from the GitHub release
  and verifies it with the SHA256 - so no `VERIFICATION.txt` is needed.
- The installer asset lookup is shared with winget (`build/winget/resolve-release-asset.ps1`),
  so both channels always point to the same binary.
- Keep the silent install flags and the package metadata (description, tags, ...) in
  sync with `build/winget/generate-from-release.ps1`.
- A pushed version cannot be overwritten - if the package is broken, fix it and push
  a [package fix version](https://docs.chocolatey.org/en-us/create/create-packages/#package-fix-version-notation)
  `x.y.z.YYYYMMDD`.

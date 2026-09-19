# MindForger thinking notebook
#
# Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>
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

# Look up the published MindForger Windows installer on a GitHub release, download
# it, compute its SHA256, and generate the three winget manifest YAML files ready
# for submission to microsoft/winget-pkgs.
#
# The asset is looked up via the GitHub Releases API instead of being guessed
# from the version alone - see resolve-release-asset.ps1 for why.
#
# Usage:
#   powershell.exe -NoProfile -ExecutionPolicy Bypass `
#       -File build\winget\generate-from-release.ps1 -Version 2.3.0

param (
    [Parameter(Mandatory = $true)]
    [string]$Version
)

$ErrorActionPreference = "Stop"

$ScriptDir   = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path (Join-Path $ScriptDir "..\..")

. (Join-Path $ScriptDir "resolve-release-asset.ps1")

$GithubOrg  = "dvorka"
$GithubRepo = "mindforger"
$PackageId  = "MindForger.MindForger"

# Inno Setup AppId from build/windows/installer/mindforger-setup.iss - keep in sync
$AppId = "{A1A3DAE4-FD5C-4600-B75D-D8895AA99693}"

# winget manifest schema version - keep the ManifestVersion fields and the
# yaml-language-server schema URLs in sync by deriving both from this value
$ManifestVersion = "1.12.0"
$SchemaBaseUrl   = "https://aka.ms/winget-manifest"

# -- Step 1: find the Windows installer asset on the GitHub release ------------

$Asset = Resolve-MindForgerReleaseAsset `
    -Version $Version -GithubOrg $GithubOrg -GithubRepo $GithubRepo
$InstallerName = $Asset.name
$InstallerUrl  = $Asset.browser_download_url

# -- Step 2: download and hash --------------------------------------------------

Write-Host "Downloading $InstallerUrl ..." -ForegroundColor Cyan
$TmpFile = [System.IO.Path]::Combine([System.IO.Path]::GetTempPath(), $InstallerName)
Invoke-WebRequest -Uri $InstallerUrl -OutFile $TmpFile -UseBasicParsing
$Sha256 = (Get-FileHash $TmpFile -Algorithm SHA256).Hash
Remove-Item $TmpFile -Force
Write-Host "SHA256: $Sha256" -ForegroundColor Green

# -- Step 3: prepare output directory ------------------------------------------

$OutDir = Join-Path $ProjectRoot "distro\winget\manifests\m\MindForger\MindForger\$Version"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

# UTF-8 without BOM - winget rejects files with a BOM
$Utf8NoBom = New-Object System.Text.UTF8Encoding $false

# -- Step 4: version manifest ---------------------------------------------------

$VersionYaml = @"
# yaml-language-server: `$schema=$SchemaBaseUrl.version.$ManifestVersion.schema.json
PackageIdentifier: $PackageId
PackageVersion: $Version
DefaultLocale: en-US
ManifestType: version
ManifestVersion: $ManifestVersion
"@
[System.IO.File]::WriteAllText(
    (Join-Path $OutDir "$PackageId.yaml"), $VersionYaml, $Utf8NoBom)

# -- Step 5: installer manifest -------------------------------------------------

$InstallerYaml = @"
# yaml-language-server: `$schema=$SchemaBaseUrl.installer.$ManifestVersion.schema.json
PackageIdentifier: $PackageId
PackageVersion: $Version
InstallerType: inno
Scope: machine
InstallModes:
  - interactive
  - silent
Installers:
  - Architecture: x64
    InstallerUrl: $InstallerUrl
    InstallerSha256: $Sha256
    ProductCode: '${AppId}_is1'
ManifestType: installer
ManifestVersion: $ManifestVersion
"@
[System.IO.File]::WriteAllText(
    (Join-Path $OutDir "$PackageId.installer.yaml"), $InstallerYaml, $Utf8NoBom)

# -- Step 6: locale manifest ----------------------------------------------------

$LocaleYaml = @"
# yaml-language-server: `$schema=$SchemaBaseUrl.defaultLocale.$ManifestVersion.schema.json
PackageIdentifier: $PackageId
PackageVersion: $Version
PackageLocale: en-US
Publisher: Martin Dvorak
PublisherUrl: https://www.mindforger.com
PublisherSupportUrl: https://github.com/$GithubOrg/$GithubRepo/issues
Author: Martin Dvorak
PackageName: MindForger
PackageUrl: https://www.mindforger.com
License: GPL-2.0-or-later
LicenseUrl: https://github.com/$GithubOrg/$GithubRepo/blob/master/LICENSE
ShortDescription: MindForger Thinking Notebook and Markdown IDE
Description: |-
  MindForger is an open, free, and privacy-respecting thinking notebook and
  Markdown IDE for personal knowledge management, note taking, and organizing
  your thoughts, notes, and notebooks as plain Markdown files.
Tags:
  - markdown
  - notes
  - notebook
  - knowledge-management
  - pkm
  - personal-wiki
  - thinking-tool
  - ide
ReleaseNotesUrl: https://github.com/$GithubOrg/$GithubRepo/releases/tag/$Version
ManifestType: defaultLocale
ManifestVersion: $ManifestVersion
"@
[System.IO.File]::WriteAllText(
    (Join-Path $OutDir "$PackageId.locale.en-US.yaml"), $LocaleYaml, $Utf8NoBom)

# -- Done -----------------------------------------------------------------------

Write-Host ""
Write-Host "Manifests written to: $OutDir" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  make distro-winget-validate VERSION=$Version"
Write-Host "  ... then copy manifests to your winget-pkgs fork and open a PR"

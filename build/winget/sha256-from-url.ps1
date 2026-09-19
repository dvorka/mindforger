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

# Compute SHA256 of the published MindForger Windows installer by looking it up
# on a GitHub release and downloading it. Use this when you just need the hash
# (e.g. to double check a manifest) without regenerating the manifest files.
#
# Usage:
#   powershell.exe -NoProfile -ExecutionPolicy Bypass `
#       -File build\winget\sha256-from-url.ps1 -Version 2.3.0

param (
    [Parameter(Mandatory = $true)]
    [string]$Version
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

. (Join-Path $ScriptDir "resolve-release-asset.ps1")

$GithubOrg  = "dvorka"
$GithubRepo = "mindforger"

# same asset lookup as generate-from-release.ps1 - so that the hash printed here
# and the hash written to the manifest can never come from different binaries
$Asset = Resolve-MindForgerReleaseAsset `
    -Version $Version -GithubOrg $GithubOrg -GithubRepo $GithubRepo

$Url = $Asset.browser_download_url
$TmpFile = [System.IO.Path]::Combine([System.IO.Path]::GetTempPath(), $Asset.name)

Write-Host "Downloading $Url ..."
Invoke-WebRequest -Uri $Url -OutFile $TmpFile -UseBasicParsing

$Hash = (Get-FileHash $TmpFile -Algorithm SHA256).Hash
Remove-Item $TmpFile -Force

Write-Host ""
Write-Host "SHA256: $Hash"
Write-Host ""
Write-Host "Paste this SHA256 into the InstallerSha256 field of your winget manifest."
Write-Host "(To generate the full manifests and hash in one step, use: make distro-winget-from-release VERSION=$Version)"

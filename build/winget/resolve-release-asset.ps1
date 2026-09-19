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

# Shared helper: resolve the Windows installer asset of a GitHub release.
#
# The installer is uploaded to the release by hand and its name is not fixed
# across releases (it carries a short git commit hash, and sometimes a Qt
# version suffix, e.g. "windows-installer-mindforger-2.1.0-ff388a4.exe"), so it
# is looked up via the GitHub Releases API instead of being guessed from the
# version alone.
#
# Dot-source this file to use Resolve-MindForgerReleaseAsset:
#   . (Join-Path $PSScriptRoot "resolve-release-asset.ps1")

# asset name pattern of the Windows installer published on a GitHub release
$script:MindForgerInstallerPattern = '^windows-installer-mindforger-.*\.exe$'

function Resolve-MindForgerReleaseAsset {
    <#
    .SYNOPSIS
    Return the single Windows installer asset of the given MindForger release.

    .DESCRIPTION
    Queries the GitHub Releases API for the release tagged $Version and returns
    the asset matching the Windows installer naming pattern. Exactly one match
    is required: zero matches means the installer has not been uploaded yet,
    and more than one match is ambiguous (a rebuild or another architecture)
    and would silently publish the URL and hash of the wrong binary.
    #>
    param (
        [Parameter(Mandatory = $true)]
        [string]$Version,

        [Parameter(Mandatory = $true)]
        [string]$GithubOrg,

        [Parameter(Mandatory = $true)]
        [string]$GithubRepo
    )

    $ReleaseUrl = "https://github.com/$GithubOrg/$GithubRepo/releases/tag/$Version"
    $ApiUrl     = "https://api.github.com/repos/$GithubOrg/$GithubRepo/releases/tags/$Version"

    Write-Host "Looking up release $Version on GitHub..." -ForegroundColor Cyan
    try {
        $Release = Invoke-RestMethod -Uri $ApiUrl -Headers @{ "User-Agent" = "mindforger-winget-script" }
    } catch {
        throw "Failed to query the GitHub release API for $Version ($($_.Exception.Message)).`nCheck $ReleaseUrl"
    }

    # wrap in @() so that a single match is still an array and .Count is reliable
    $Assets = @($Release.assets | Where-Object { $_.name -match $script:MindForgerInstallerPattern })

    if ($Assets.Count -eq 0) {
        throw "No windows-installer-mindforger-*.exe asset found on release $Version.`nUpload the Windows installer to the release first - see $ReleaseUrl"
    }
    if ($Assets.Count -gt 1) {
        $Names = ($Assets | ForEach-Object { $_.name }) -join "`n  "
        throw "Expected exactly one windows-installer-mindforger-*.exe asset on release $Version, found $($Assets.Count):`n  $Names`nRemove the stale asset from $ReleaseUrl so that the manifest cannot be generated from the wrong binary."
    }

    $Asset = $Assets[0]

    # the asset is renamed by hand on upload, so its name and the release tag can
    # drift apart (release 2.0.0 carries windows-installer-mindforger-2.0.1-*.exe)
    if ($Asset.name -notmatch [regex]::Escape($Version)) {
        Write-Warning "Asset '$($Asset.name)' does not contain the version '$Version' - verify it is the right binary before submitting the manifest."
    }

    Write-Host "Found asset: $($Asset.name)" -ForegroundColor Green
    return $Asset
}

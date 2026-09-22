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

# Copy generated winget manifests to a local winget-pkgs fork, commit on a new
# branch, push, and open a pull request against microsoft/winget-pkgs.
#
# Prerequisites:
#   - Manifests already generated: make distro-winget-from-release VERSION=x.y.z
#   - winget-pkgs fork cloned locally; path set via -WingetPkgsDir or $env:WINGET_PKGS_DIR
#   - gh CLI installed and authenticated: winget install GitHub.cli && gh auth login
#
# Usage:
#   make distro-winget-submit-pr VERSION=2.3.0 WINGET_PKGS_DIR=C:\path\to\winget-pkgs
# or directly:
#   powershell.exe -NoProfile -ExecutionPolicy Bypass `
#       -File build\winget\submit-pr.ps1 `
#       -Version 2.3.0 `
#       -WingetPkgsDir C:\path\to\winget-pkgs

param (
    [Parameter(Mandatory = $true)]
    [string]$Version,

    [string]$WingetPkgsDir = $env:WINGET_PKGS_DIR
)

$ErrorActionPreference = "Stop"

function Invoke-Git {
    # run git and fail loudly - a failing native command does not raise a
    # PowerShell exception, so an unchecked failure would let the script carry
    # on against a stale or wrong repository state (and 'git reset --hard' then
    # discards whatever happens to be checked out)
    param (
        [Parameter(Mandatory = $true, ValueFromRemainingArguments = $true)]
        [string[]]$Arguments
    )

    & git @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "git $($Arguments -join ' ') failed (exit $LASTEXITCODE)"
    }
}

if (-not $WingetPkgsDir) {
    Write-Error "winget-pkgs directory not specified.`nPass -WingetPkgsDir or set the WINGET_PKGS_DIR environment variable."
}
if (-not (Test-Path (Join-Path $WingetPkgsDir ".git"))) {
    Write-Error "'$WingetPkgsDir' is not a git repository.`nClone your winget-pkgs fork there first."
}

$ScriptDir   = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path (Join-Path $ScriptDir "..\..")
$PackageId   = "MindForger.MindForger"
$ManifestSrc = Join-Path $ProjectRoot "distro\winget\manifests\m\MindForger\MindForger\$Version"
$ManifestDst = Join-Path $WingetPkgsDir "manifests\m\MindForger\MindForger\$Version"
$BranchName  = "$PackageId-$Version"
$IsNew       = -not (Test-Path (Join-Path $WingetPkgsDir "manifests\m\MindForger"))

if (-not (Test-Path $ManifestSrc)) {
    Write-Error "Manifests not found at $ManifestSrc`nRun 'make distro-winget-from-release VERSION=$Version' first."
}

$Verb      = if ($IsNew) { "New package" } else { "Update" }
$CommitMsg = "${Verb}: $PackageId version $Version"
$PrTitle   = $CommitMsg

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "winget PR submission" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host "Package:   $PackageId $Version"
Write-Host "Fork dir:  $WingetPkgsDir"
Write-Host "Branch:    $BranchName"
Write-Host "PR title:  $PrTitle"
Write-Host ""

Push-Location $WingetPkgsDir
try {
    # refuse to touch a dirty fork - the sync below is destructive
    $Dirty = git status --porcelain
    if ($LASTEXITCODE -ne 0) { throw "git status failed (exit $LASTEXITCODE)" }
    if ($Dirty) {
        throw "'$WingetPkgsDir' has uncommitted changes.`nCommit, stash or discard them first - syncing the fork resets master with 'git reset --hard'."
    }

    # sync fork master with upstream (microsoft/winget-pkgs)
    Write-Host "Syncing fork master with upstream..." -ForegroundColor Yellow
    Invoke-Git fetch https://github.com/microsoft/winget-pkgs.git master
    Invoke-Git checkout master
    Invoke-Git reset --hard FETCH_HEAD

    # create fresh branch
    Write-Host "Creating branch $BranchName..." -ForegroundColor Yellow
    if (git branch --list $BranchName) { Invoke-Git branch -D $BranchName }
    Invoke-Git checkout -b $BranchName

    # copy manifests
    New-Item -ItemType Directory -Force -Path $ManifestDst | Out-Null
    Copy-Item -Path "$ManifestSrc\*" -Destination $ManifestDst -Force
    Write-Host "Manifests copied to $ManifestDst" -ForegroundColor Green

    # stage
    Invoke-Git add "manifests\m\MindForger\MindForger\$Version"

    # commit - try with GPG signature, fall back to unsigned
    Write-Host "Committing..." -ForegroundColor Yellow
    # signing is best effort, so this one call is deliberately not Invoke-Git
    git commit -S -m $CommitMsg 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "GPG-signed commit failed; committing without signature..." -ForegroundColor Yellow
        Invoke-Git commit -m $CommitMsg
    }

    # push branch to fork
    Write-Host "Pushing branch to fork..." -ForegroundColor Yellow
    Invoke-Git push origin $BranchName --force-with-lease

    # open PR against microsoft/winget-pkgs
    Write-Host "Opening PR against microsoft/winget-pkgs..." -ForegroundColor Yellow
    $PrUrl = gh pr create `
        --repo microsoft/winget-pkgs `
        --title $PrTitle `
        --body "Automated submission for ``$PackageId`` $Version." `
        --base master
    if ($LASTEXITCODE -ne 0) { throw "gh pr create failed (exit $LASTEXITCODE)" }

    Write-Host ""
    Write-Host "================================================" -ForegroundColor Green
    Write-Host "PR submitted: $PrUrl" -ForegroundColor Green
    Write-Host "================================================" -ForegroundColor Green
    Write-Host "Monitor wingetbot validation at the URL above."
} finally {
    Pop-Location
}
